/*
 * ft_popen - popen関数の実装 解説版
 * 
 * 問題: popen()ライブラリ関数を再実装する
 * 
 * popen()の動作:
 * - 子プロセスでコマンドを実行
 * - 親プロセスとパイプで接続
 * - 'r': 子プロセスの出力を読み取り用ファイルディスクリプタで返す
 * - 'w': 子プロセスの入力に書き込み用ファイルディスクリプタで返す
 * 
 * 使用システムコール:
 * - pipe(): パイプ作成
 * - fork(): プロセス分岐
 * - dup2(): ファイルディスクリプタ複製
 * - execvp(): プログラム実行
 */

#include <unistd.h>
#include <sys/wait.h>

/*
 * ft_popen: popen関数の実装
 * 
 * @param file: 実行するプログラム名
 * @param argv: プログラムの引数配列（NULL終端）
 * @param type: 'r'（読み取り）または 'w'（書き込み）
 * @return: 成功時はファイルディスクリプタ、失敗時は-1
 */
int ft_popen(const char *file, char *const argv[], char type)
{
    // 引数チェック
    if (type != 'r' && type != 'w') 
        return -1;
    
    // パイプ作成
    int pipefd[2];
    if (pipe(pipefd) == -1) 
        return -1;
    
    // プロセス分岐
    pid_t pid = fork();
    if (pid == -1) 
    { 
        // fork失敗時はパイプを閉じる
        close(pipefd[0]); 
        close(pipefd[1]); 
        return -1; 
    }
    
    // 子プロセス
    if (pid == 0)
    {
        if (type == 'r')
        {
            // 読み取りモード: 子プロセスの標準出力をパイプに接続
            close(pipefd[0]);           // 読み取り側を閉じる
            dup2(pipefd[1], 1);         // 標準出力をパイプの書き込み側に複製
            close(pipefd[1]);           // 元のファイルディスクリプタを閉じる
        }
        else // type == 'w'
        {
            // 書き込みモード: 子プロセスの標準入力をパイプに接続
            close(pipefd[1]);           // 書き込み側を閉じる
            dup2(pipefd[0], 0);         // 標準入力をパイプの読み取り側に複製
            close(pipefd[0]);           // 元のファイルディスクリプタを閉じる
        }
        
        // プログラム実行
        execvp(file, argv);
        exit(1);  // execvp失敗時
    }
    
    // 親プロセス
    if (type == 'r')
    {
        // 読み取りモード: 書き込み側を閉じて読み取り側を返す
        close(pipefd[1]);
        return pipefd[0];
    }
    else // type == 'w'
    {
        // 書き込みモード: 読み取り側を閉じて書き込み側を返す
        close(pipefd[0]);
        return pipefd[1];
    }
}/
*
 * 使用例とデータフローの解説:
 * 
 * 例1: 読み取りモード ('r')
 * int fd = ft_popen("ls", (char *const []){"ls", NULL}, 'r');
 * 
 * データフロー:
 * 子プロセス: ls コマンド → 標準出力 → パイプ書き込み側
 * 親プロセス: パイプ読み取り側 ← fd ← read()
 * 
 * 例2: 書き込みモード ('w')  
 * int fd = ft_popen("grep", (char *const []){"grep", "pattern", NULL}, 'w');
 * 
 * データフロー:
 * 親プロセス: write() → fd → パイプ書き込み側
 * 子プロセス: パイプ読み取り側 → 標準入力 → grep コマンド
 * 
 * 重要なポイント:
 * 1. パイプは一方向通信（読み取り側と書き込み側）
 * 2. 使わない側のファイルディスクリプタは必ず閉じる
 * 3. dup2()で標準入出力を置き換える
 * 4. 子プロセスでexecvp()実行後は元のプログラムは消える
 * 5. ファイルディスクリプタのリークを防ぐため適切にclose()
 * 
 * エラーハンドリング:
 * - pipe()失敗: パイプ作成不可
 * - fork()失敗: プロセス作成不可、パイプを閉じてから-1を返す
 * - 不正なtype: 'r'と'w'以外は-1を返す
 */