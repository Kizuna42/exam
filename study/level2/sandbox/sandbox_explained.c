/*
 * sandbox - 関数実行の安全性チェック 解説版
 * 
 * 問題: 与えられた関数を安全に実行し、結果を判定
 * 
 * 判定基準:
 * - Nice function (1): 正常終了（exit code 0）
 * - Bad function (0): 異常終了、シグナル、タイムアウト
 * - Error (-1): sandbox関数自体のエラー
 * 
 * 使用技術:
 * - fork(): 子プロセスで関数実行（親プロセスを保護）
 * - alarm(): タイムアウト設定
 * - waitpid(): 子プロセスの状態監視
 * - シグナルハンドリング
 * 
 * 重要な制約: 
 * - 許可関数のみ使用可能: fork, waitpid, exit, alarm, sigaction, kill, printf, strsignal,
 *   errno, sigaddset, sigemptyset, sigfillset, sigdelset, sigismember
 * - memset() は使用不可（許可関数リストにない）
 */

#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

/*
 * sandbox: 関数を安全に実行して結果を判定
 * 
 * @param f: 実行する関数ポインタ
 * @param timeout: タイムアウト時間（秒）
 * @param verbose: 詳細メッセージ出力フラグ
 * @return: 1=Nice, 0=Bad, -1=Error
 */
int sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
    // 子プロセスを作成
    pid_t pid = fork();
    if (pid == -1) 
        return -1;  // fork失敗
    
    // 子プロセス
    if (pid == 0)
    {
        // 関数を実行
        f();
        exit(0);  // 正常終了
    }
    
    // 親プロセス: シグナルハンドラー設定（SIG_IGNで無視）
    struct sigaction sa_old, sa_new;
    sigemptyset(&sa_new.sa_mask);
    sa_new.sa_handler = SIG_IGN;
    sa_new.sa_flags = 0;
    if (sigaction(SIGALRM, &sa_new, &sa_old) == -1)
        return -1;
    
    // タイムアウト設定
    alarm(timeout);
    
    int status;
    pid_t result = waitpid(pid, &status, 0);
    
    // アラーム無効化とシグナルハンドラー復元
    alarm(0);
    sigaction(SIGALRM, &sa_old, NULL);
    
    // waitpid失敗の場合
    if (result == -1)
    {
        // EINTR = アラームによる割り込み（タイムアウト）
        if (errno == EINTR)
        {
            // 子プロセスを強制終了
            kill(pid, SIGKILL);
            waitpid(pid, NULL, 0);  // ゾンビプロセス回避
            
            if (verbose) 
                printf("Bad function: timed out after %u seconds\n", timeout);
            return 0;
        }
        return -1;  // その他のエラー
    }
    
    // 子プロセスが正常終了した場合
    if (WIFEXITED(status))
    {
        int exit_code = WEXITSTATUS(status);
        if (exit_code == 0)
        {
            // 正常終了（exit code 0）
            if (verbose) 
                printf("Nice function!\n");
            return 1;
        }
        else
        {
            // 異常終了（exit code != 0）
            if (verbose) 
                printf("Bad function: exited with code %d\n", exit_code);
            return 0;
        }
    }
    // 子プロセスがシグナルで終了した場合
    else if (WIFSIGNALED(status))
    {
        int sig = WTERMSIG(status);
        if (verbose) 
            printf("Bad function: %s\n", strsignal(sig));
        return 0;
    }
    
    return 0;  // その他の場合
}/*

 * 実行例と詳細解説:
 * 
 * 例1: 正常な関数
 * void good_func(void) { return; }
 * int result = sandbox(good_func, 5, true);
 * → 出力: "Nice function!"
 * → 戻り値: 1
 * 
 * 例2: 異常終了する関数
 * void bad_func(void) { exit(42); }
 * int result = sandbox(bad_func, 5, true);
 * → 出力: "Bad function: exited with code 42"
 * → 戻り値: 0
 * 
 * 例3: セグフォルトする関数
 * void crash_func(void) { *(int*)0 = 42; }
 * int result = sandbox(crash_func, 5, true);
 * → 出力: "Bad function: Segmentation fault"
 * → 戻り値: 0
 * 
 * 例4: 無限ループする関数
 * void loop_func(void) { while(1); }
 * int result = sandbox(loop_func, 3, true);
 * → 出力: "Bad function: timed out after 3 seconds"
 * → 戻り値: 0
 * 
 * タイムアウト処理の詳細:
 * 1. sigaction()でSIGALRMハンドラーをSIG_IGN（無視）に設定
 * 2. alarm(timeout)でタイマー設定
 * 3. 子プロセスがtimeout秒以内に終了しない場合
 * 4. SIGALRMシグナルが親プロセスに送信される
 * 5. シグナルは無視されるがwaitpid()がEINTRエラーで中断される
 * 6. kill(pid, SIGKILL)で子プロセスを強制終了
 * 7. waitpid()でゾンビプロセスを回収
 * 8. sigaction()で元のシグナルハンドラーを復元
 * 
 * プロセス状態の判定:
 * - WIFEXITED(status): 正常終了かチェック
 * - WEXITSTATUS(status): 終了コードを取得
 * - WIFSIGNALED(status): シグナルで終了したかチェック
 * - WTERMSIG(status): 終了シグナルを取得
 * 
 * 重要なポイント:
 * 1. fork()で親プロセスを保護
 * 2. sigaction()による適切なシグナル制御
 * 3. alarm()でタイムアウト制御
 * 4. ゾンビプロセスの回避
 * 5. 適切なエラーハンドリング
 * 6. シグナルの種類に応じた詳細メッセージ
 * 7. 許可関数の制約を遵守（memset使用不可）
 * 
 * 実装上の注意事項:
 * - sigemptyset()で構造体を初期化（memsetの代替）
 * - SIG_IGNでシグナルを無視設定
 * - 元のシグナルハンドラーを必ず復元
 */