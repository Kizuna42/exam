/*
 * N-Queens Problem - 解説版
 * 
 * 問題: N×Nのチェス盤にN個のクイーンを配置し、
 * どのクイーンも他のクイーンを攻撃できない配置を全て見つける
 * 
 * クイーンの攻撃範囲:
 * - 同じ行（横）
 * - 同じ列（縦）  
 * - 同じ対角線（斜め）
 * 
 * アルゴリズム: バックトラッキング
 * 1. 左の列から順番にクイーンを配置
 * 2. 各列で全ての行を試す
 * 3. 安全な位置が見つかったら次の列へ
 * 4. 全列に配置できたら解として出力
 * 5. 行き詰まったら前の列に戻って別の位置を試す
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/*
 * is_safe: 指定位置にクイーンを配置しても安全かチェック
 * 
 * @param tab: 各列のクイーンの行位置を記録する配列
 * @param col: 現在チェックしている列
 * @param row: 現在チェックしている行
 * @return: 安全なら1、危険なら0
 */
int is_safe(int *tab, int col, int row)
{
    // 既に配置されたクイーン（0列目からcol-1列目まで）をチェック
    for(int i = 0; i < col; i++)
    {
        // 同じ行にクイーンがあるかチェック
        if(tab[i] == row)
            return 0;
        
        // 対角線上にクイーンがあるかチェック
        // abs(i - col) == abs(tab[i] - row) なら対角線上
        // 列の差 == 行の差 なら斜めの関係
        if(abs(i - col) == abs(tab[i] - row))
            return 0;
    }
    return 1; // 安全
}

/*
 * solve: バックトラッキングでN-Queens問題を解く
 * 
 * @param tab: 各列のクイーンの行位置を記録する配列
 * @param n: チェス盤のサイズ（N×N）
 * @param col: 現在処理している列
 */
void solve(int *tab, int n, int col)
{
    // ベースケース: 全ての列にクイーンを配置できた
    if(col == n)
    {
        // 解を出力（各列のクイーンの行番号）
        for(int i = 0; i < n; i++)
        {
            printf("%d", tab[i]);
            if(i < n - 1)
                printf(" ");
        }
        printf("\n");
        return;
    }
    
    // 現在の列の全ての行を試す
    for(int i = 0; i < n; i++)
    {
        // この位置が安全かチェック
        if(is_safe(tab, col, i))
        {
            // クイーンを配置
            tab[col] = i;
            
            // 次の列を再帰的に処理
            solve(tab, n, col + 1);
            
            // バックトラック（自動的に次のループで上書きされる）
        }
    }
}

int main(int ac, char **argv)
{
    // 引数チェック
    if(ac != 2)
        return 0;
    // チェス盤のサイズを取得
    int n = atoi(argv[1]);
    // 各列のクイーンの行位置を記録する配列を確保
    // tab[i] = j は「i列目のクイーンがj行目にある」を意味
    int *tab = calloc(sizeof(int), n);
    // 0列目から開始してN-Queens問題を解く
    solve(tab, n, 0);
    return 0;
}

/*
 * 実行例:
 * ./n_queens 4
 * 出力:
 * 1 3 0 2
 * 2 0 3 1
 *
 * 解釈:
 * 1 3 0 2 は以下の配置を意味:
 * . Q . .  (0行目: 1列目にクイーン)
 * . . . Q  (1行目: 3列目にクイーン)
 * Q . . .  (2行目: 0列目にクイーン)
 * . . Q .  (3行目: 2列目にクイーン)
 */