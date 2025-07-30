/*
 * RIP - 括弧バランス修正 解説版
 * 
 * 問題: 不正な括弧列から最小数の括弧を削除（スペースに置換）して
 * 正しい括弧列にする全ての方法を見つける
 * 
 * アルゴリズム:
 * 1. 削除すべき開き括弧と閉じ括弧の数を計算
 * 2. バックトラッキングで全ての削除パターンを試す
 * 3. 重複を除去して出力
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * is_valid: 括弧列が正しいかチェック
 * 
 * @param s: チェックする文字列
 * @return: 正しければ1、不正なら0
 * 
 * 正しい括弧列の条件:
 * 1. どの時点でも閉じ括弧の数が開き括弧の数を超えない
 * 2. 最終的に開き括弧と閉じ括弧の数が等しい
 */
int is_valid(char *s)
{
    int count = 0;
    for (int i = 0; s[i]; i++)
    {
        if (s[i] == '(') 
            count++;
        else if (s[i] == ')') 
            count--;
        
        // 閉じ括弧が開き括弧より多くなったら不正
        if (count < 0) 
            return 0;
    }
    // 最終的に開き括弧と閉じ括弧が同数なら正しい
    return count == 0;
}

/*
 * solve: バックトラッキングで括弧削除パターンを探索
 * 
 * @param s: 元の文字列
 * @param idx: 現在処理している文字のインデックス
 * @param open_rem: 削除すべき開き括弧の残り数
 * @param close_rem: 削除すべき閉じ括弧の残り数  
 * @param current: 現在構築中の文字列
 * @param pos: currentの現在位置
 * @param results: 結果を保存する配列
 * @param count: 結果の数
 */
void solve(char *s, int idx, int open_rem, int close_rem, char *current, int pos, char **results, int *count)
{
    // ベースケース: 全ての文字を処理完了
    if (idx == strlen(s))
    {
        // 削除すべき括弧が全て削除され、かつ正しい括弧列の場合
        if (open_rem == 0 && close_rem == 0 && is_valid(current))
        {
            current[pos] = '\0';
            
            // 重複チェック
            for (int i = 0; i < *count; i++)
                if (strcmp(results[i], current) == 0) 
                    return;
            
            // 新しい解として保存
            results[*count] = malloc(strlen(current) + 1);
            strcpy(results[*count], current);
            (*count)++;
        }
        return;
    }
    
    char ch = s[idx];
    
    // 選択1: 現在の括弧を削除（スペースに置換）
    if ((ch == '(' && open_rem > 0) || (ch == ')' && close_rem > 0))
    {
        current[pos] = ' ';
        solve(s, idx + 1, 
              (ch == '(') ? open_rem - 1 : open_rem, 
              (ch == ')') ? close_rem - 1 : close_rem, 
              current, pos + 1, results, count);
    }
    
    // 選択2: 現在の文字をそのまま保持
    current[pos] = ch;
    solve(s, idx + 1, open_rem, close_rem, current, pos + 1, results, count);
}

int main(int ac, char **av)
{
    if (ac != 2) return 1;
    
    char *s = av[1];
    int len = strlen(s);
    
    // 削除すべき括弧の数を計算
    int open_rem = 0, close_rem = 0;
    
    for (int i = 0; i < len; i++)
    {
        if (s[i] == '(') 
        {
            open_rem++;
        }
        else if (s[i] == ')')
        {
            if (open_rem > 0) 
                open_rem--;  // 対応する開き括弧があるので相殺
            else 
                close_rem++; // 対応する開き括弧がないので削除対象
        }
    }
    
    // 結果保存用の配列を確保
    char **results = malloc(1000 * sizeof(char*));
    char *current = malloc(len + 1);
    int count = 0;
    
    // バックトラッキング開始
    solve(s, 0, open_rem, close_rem, current, 0, results, &count);
    
    // 結果を出力してメモリ解放
    for (int i = 0; i < count; i++)
    {
        puts(results[i]);
        free(results[i]);
    }
    
    free(results);
    free(current);
    return 0;
}/
*
 * 実行例:
 * ./rip '(()'
 * 出力:
 * ()
 * ( )
 * 
 * 削除数計算の流れ（'(()'の場合）:
 * 1. '(': open_rem = 1
 * 2. '(': open_rem = 2  
 * 3. ')': open_rem > 0なので open_rem = 1（相殺）
 * 結果: open_rem = 1, close_rem = 0
 * → 開き括弧を1個削除する必要がある
 * 
 * バックトラッキングの流れ:
 * solve("(()", idx=0, open_rem=1, close_rem=0, current="", pos=0)
 * ├─ '('を削除: current=" ", solve(..., idx=1, open_rem=0, close_rem=0, pos=1)
 * │  ├─ '('を保持: current=" (", solve(..., idx=2, open_rem=0, close_rem=0, pos=2)
 * │  │  └─ ')'を保持: current=" ()", solve(..., idx=3, open_rem=0, close_rem=0, pos=3)
 * │  │     └─ idx==len, open_rem==0, close_rem==0, is_valid(" ()")=true → " ()"出力
 * │  └─ '('を削除: 不可（open_rem=0）
 * └─ '('を保持: current="(", solve(..., idx=1, open_rem=1, close_rem=0, pos=1)
 *    ├─ '('を削除: current="( ", solve(..., idx=2, open_rem=0, close_rem=0, pos=2)
 *    │  └─ ')'を保持: current="( )", solve(..., idx=3, open_rem=0, close_rem=0, pos=3)
 *    │     └─ idx==len, open_rem==0, close_rem==0, is_valid("( )")=true → "( )"出力
 *    └─ '('を保持: current="((", solve(..., idx=2, open_rem=1, close_rem=0, pos=2)
 *       └─ ')'を保持: current="(()", solve(..., idx=3, open_rem=1, close_rem=0, pos=3)
 *          └─ idx==len, open_rem!=0 → 無効
 * 
 * 最終出力: " ()" と "( )" → puts()で " ()" と "( )" として表示
 */