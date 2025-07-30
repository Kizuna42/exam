/*
 * Permutations - 順列生成 解説版
 * 
 * 問題: 与えられた文字列の全ての順列をアルファベット順で出力
 * 
 * アルゴリズム:
 * 1. 再帰的に全ての順列を生成（Heap's algorithm変形）
 * 2. 生成した順列を配列に保存
 * 3. qsortでアルファベット順にソート
 * 4. 順番に出力
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * swap: 2つの文字を交換
 * ポインタを使って実際の値を交換
 */
void swap(char *a, char *b)
{
    char temp = *a;
    *a = *b;
    *b = temp;
}

/*
 * permute: 再帰的に順列を生成
 * 
 * @param str: 順列を生成する文字列
 * @param l: 固定する部分の開始位置（left）
 * @param r: 文字列の最後のインデックス（right）
 * @param result: 生成した順列を保存する配列
 * @param count: 現在生成した順列の数
 * 
 * 動作原理:
 * - l == r の時: 1つの順列が完成
 * - そうでない時: l位置に各文字を順番に配置し、残りを再帰処理
 */
void permute(char *str, int l, int r, char **result, int *count)
{
    // ベースケース: 全ての位置が決まった
    if (l == r)
    {
        // 現在の順列をコピーして保存
        result[*count] = malloc(strlen(str) + 1);
        strcpy(result[*count], str);
        (*count)++;
    }
    else
    {
        // l位置に各文字を順番に配置
        for (int i = l; i <= r; i++)
        {
            // l位置とi位置の文字を交換
            swap(&str[l], &str[i]);
            
            // 残りの部分(l+1からr)を再帰的に処理
            permute(str, l + 1, r, result, count);
            
            // バックトラック: 元の状態に戻す
            swap(&str[l], &str[i]);
        }
    }
}

/*
 * compare: qsort用の比較関数
 * 文字列をアルファベット順で比較
 */
int compare(const void *a, const void *b)
{
    return strcmp(*(char**)a, *(char**)b);
}

int main(int ac, char **av)
{
    // 引数チェック
    if (ac != 2) return 1;
    
    char *str = av[1];
    int len = strlen(str);
    
    // n文字の順列数はn!個
    int fact = 1;
    for (int i = 2; i <= len; i++) 
        fact *= i;
    
    // 順列を保存する配列を確保
    char **result = malloc(fact * sizeof(char*));
    int count = 0;
    
    // 全ての順列を生成
    permute(str, 0, len - 1, result, &count);
    
    // アルファベット順にソート
    qsort(result, count, sizeof(char*), compare);
    
    // 順列を出力してメモリ解放
    for (int i = 0; i < count; i++)
    {
        puts(result[i]);
        free(result[i]);
    }
    
    free(result);
    return 0;
}

/*
 * 実行例:
 * ./permutation abc
 * 出力:
 * abc
 * acb  
 * bac
 * bca
 * cab
 * cba
 * 
 * 動作の流れ（"abc"の場合）:
 * 1. permute("abc", 0, 2) 呼び出し
 * 2. i=0: swap不要, permute("abc", 1, 2)
 *    - i=1: swap不要, permute("abc", 2, 2) → "abc"保存
 *    - i=2: swap(b,c), permute("acb", 2, 2) → "acb"保存, swap戻す
 * 3. i=1: swap(a,b), permute("bac", 1, 2)
 *    - 同様に"bac", "bca"を生成
 * 4. i=2: swap(a,c), permute("cba", 1, 2)  
 *    - 同様に"cab", "cba"を生成
 * 5. ソートして出力
 */