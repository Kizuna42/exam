/*
 * picoshell - パイプライン実行 解説版
 * 
 * 問題: シェルのパイプライン機能を実装
 * 例: ls | grep pattern | sort
 * 
 * 動作原理:
 * 1. 複数のコマンドを順次実行
 * 2. 前のコマンドの出力を次のコマンドの入力に接続
 * 3. 全ての子プロセスの終了を待つ
 * 
 * パイプライン構造:
 * cmd1 → pipe1 → cmd2 → pipe2 → cmd3
 */

#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

/*
 * picoshell: パイプライン実行関数
 * 
 * @param cmds: コマンド配列の配列（NULL終端）
 *              cmds[0] = {"ls", NULL}
 *              cmds[1] = {"grep", "pattern", NULL}  
 *              cmds[2] = NULL
 * @return: 成功時0、失敗時1
 */
int picoshell(char **cmds[])
{
    int i = 0, prev_pipe = -1;
    pid_t *pids;
    
    // コマンド数をカウント
    while (cmds[i]) i++;
    if (i == 0) return 0;  // コマンドがない場合
    
    // プロセスID保存用配列を確保
    pids = malloc(i * sizeof(pid_t));
    if (!pids) return 1;
    
    // 各コマンドを順次実行
    for (int j = 0; j < i; j++)
    {
        int pipefd[2];
        
        // 最後のコマンド以外はパイプを作成
        if (j < i - 1 && pipe(pipefd) == -1) 
        { 
            free(pids); 
            return 1; 
        }
        
        // 子プロセスを作成
        pid_t pid = fork();
        if (pid == -1) 
        { 
            free(pids); 
            return 1; 
        }
        
        // 子プロセス
        if (pid == 0)
        {
            // 前のパイプがある場合、標準入力に接続
            if (prev_pipe != -1) 
            { 
                dup2(prev_pipe, 0);     // 標準入力を前のパイプに接続
                close(prev_pipe);       // 元のファイルディスクリプタを閉じる
            }
            
            // 次のパイプがある場合、標準出力に接続
            if (j < i - 1) 
            { 
                dup2(pipefd[1], 1);     // 標準出力を現在のパイプに接続
                close(pipefd[0]);       // 読み取り側を閉じる
                close(pipefd[1]);       // 書き込み側を閉じる
            }
            
            // コマンド実行
            execvp(cmds[j][0], cmds[j]);
            exit(1);  // execvp失敗時
        }
        
        // 親プロセス
        pids[j] = pid;
        
        // 前のパイプを閉じる
        if (prev_pipe != -1) 
            close(prev_pipe);
        
        // 現在のパイプの書き込み側を閉じ、読み取り側を保存
        if (j < i - 1) 
        { 
            close(pipefd[1]); 
            prev_pipe = pipefd[0]; 
        }
    }
    
    // 最後のパイプを閉じる
    if (prev_pipe != -1) 
        close(prev_pipe);
    
    // 全ての子プロセスの終了を待つ
    for (int j = 0; j < i; j++)
        waitpid(pids[j], NULL, 0);
    
    free(pids);
    return 0;
}/*

 * パイプライン実行の詳細な流れ:
 * 
 * 例: ls | grep pattern | sort (3つのコマンド)
 * 
 * 初期状態:
 * prev_pipe = -1
 * 
 * j=0 (ls コマンド):
 * 1. pipe(pipefd) → pipefd[0]=読み取り, pipefd[1]=書き込み
 * 2. fork() → 子プロセス作成
 * 3. 子プロセス:
 *    - prev_pipe == -1 なので標準入力はそのまま
 *    - j < i-1 なので dup2(pipefd[1], 1) → 標準出力をパイプに接続
 *    - execvp("ls", {"ls", NULL})
 * 4. 親プロセス:
 *    - close(pipefd[1]) → 書き込み側を閉じる
 *    - prev_pipe = pipefd[0] → 読み取り側を保存
 * 
 * j=1 (grep コマンド):
 * 1. pipe(pipefd2) → 新しいパイプ作成
 * 2. fork() → 子プロセス作成  
 * 3. 子プロセス:
 *    - dup2(prev_pipe, 0) → 標準入力を前のパイプに接続
 *    - dup2(pipefd2[1], 1) → 標準出力を新しいパイプに接続
 *    - execvp("grep", {"grep", "pattern", NULL})
 * 4. 親プロセス:
 *    - close(prev_pipe) → 前のパイプの読み取り側を閉じる
 *    - close(pipefd2[1]) → 新しいパイプの書き込み側を閉じる
 *    - prev_pipe = pipefd2[0] → 新しいパイプの読み取り側を保存
 * 
 * j=2 (sort コマンド):
 * 1. 最後のコマンドなのでパイプ作成なし
 * 2. fork() → 子プロセス作成
 * 3. 子プロセス:
 *    - dup2(prev_pipe, 0) → 標準入力を前のパイプに接続
 *    - j == i-1 なので標準出力はそのまま（端末に出力）
 *    - execvp("sort", {"sort", NULL})
 * 4. 親プロセス:
 *    - close(prev_pipe) → 最後のパイプを閉じる
 * 
 * 最終的なデータフロー:
 * ls → pipe1 → grep → pipe2 → sort → 端末
 * 
 * 重要なポイント:
 * 1. 各プロセスは必要なファイルディスクリプタのみ保持
 * 2. 使わないファイルディスクリプタは必ず閉じる
 * 3. 親プロセスは全ての子プロセスの終了を待つ
 * 4. メモリリークを防ぐためpids配列を解放
 */