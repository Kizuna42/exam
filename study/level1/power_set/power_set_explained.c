/*
 * Power Set - 部分集合の和 解説版
 * 
 * 問題: 整数の集合から、指定された和になる部分集合を全て見つける
 * 
 * アルゴリズム: バックトラッキング
 * 1. 各要素について「含める」「含めない」の2択を再帰的に試す
 * 2. 現在の和が目標値と一致したら出力
 * 3. 和が目標値を超えたら枝刈り
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * solve: バックトラッキングで部分集合を探索
 * 
 * @param arr: 元の整数配列
 * @param n: 配列のサイズ
 * @param target: 目標の和
 * @param idx: 現在処理している要素のインデックス
 * @param subset: 現在の部分集合
 * @param size: 現在の部分集合のサイズ
 * @param sum: 現在の部分集合の和
 */
void solve(int *arr, int n, int target, int idx, int *subset, int size, int sum)
{
    // 目標の和に達した場合、部分集合を出力
    if (sum == target)
    {
        for (int i = 0; i < size; i++)
        {
            printf("%d", subset[i]);
            if (i < size - 1) printf(" ");
        }
        printf("\n");
    }
    
    // 枝刈り: インデックスが範囲外 または 和が目標を超えた場合
    if (idx >= n || sum > target) return;
    
    // 選択1: 現在の要素を部分集合に含める
    subset[size] = arr[idx];
    solve(arr, n, target, idx + 1, subset, size + 1, sum + arr[idx]);
    
    // 選択2: 現在の要素を部分集合に含めない
    solve(arr, n, target, idx + 1, subset, size, sum);
}

int main(int ac, char **av)
{
    if (ac < 2) return 1;
    
    int target = atoi(av[1]);  // 目標の和
    int n = ac - 2;            // 要素数
    
    // 要素が0個の場合の特別処理
    if (n == 0)
    {
        if (target == 0) printf("\n");  // 空集合の和は0
        return 0;
    }
    
    // 配列とワーク用配列を確保
    int *arr = malloc(n * sizeof(int));
    int *subset = malloc(n * sizeof(int));
    
    // コマンドライン引数から整数配列を構築
    for (int i = 0; i < n; i++)
        arr[i] = atoi(av[i + 2]);
    
    // バックトラッキング開始
    solve(arr, n, target, 0, subset, 0, 0);
    
    free(arr);
    free(subset);
    return 0;
}
/
*
 * 実行例:
 * ./powerset 5 1 2 3 4 5
 * 出力:
 * 1 4
 * 2 3  
 * 5
 * 
 * 動作の流れ:
 * solve(arr=[1,2,3,4,5], target=5, idx=0, subset=[], size=0, sum=0)
 * ├─ 1を含める: solve(..., idx=1, subset=[1], size=1, sum=1)
 * │  ├─ 2を含める: solve(..., idx=2, subset=[1,2], size=2, sum=3)
 * │  │  ├─ 3を含める: sum=6 > target=5 → 枝刈り
 * │  │  └─ 3を含めない: solve(..., idx=3, subset=[1,2], size=2, sum=3)
 * │  │     ├─ 4を含める: sum=7 > target=5 → 枝刈り  
 * │  │     └─ 4を含めない: solve(..., idx=4, subset=[1,2], size=2, sum=3)
 * │  │        ├─ 5を含める: sum=8 > target=5 → 枝刈り
 * │  │        └─ 5を含めない: idx=5 → 終了
 * │  └─ 2を含めない: solve(..., idx=2, subset=[1], size=1, sum=1)
 * │     ├─ 3を含める: solve(..., idx=3, subset=[1,3], size=2, sum=4)
 * │     │  ├─ 4を含める: sum=8 > target=5 → 枝刈り
 * │     │  └─ 4を含めない: solve(..., idx=4, subset=[1,3], size=2, sum=4)
 * │     │     ├─ 5を含める: sum=9 > target=5 → 枝刈り
 * │     │     └─ 5を含めない: idx=5 → 終了
 * │     └─ 3を含めない: solve(..., idx=3, subset=[1], size=1, sum=1)
 * │        ├─ 4を含める: solve(..., idx=4, subset=[1,4], size=2, sum=5)
 * │        │  └─ sum=5 == target → "1 4"出力
 * │        └─ 4を含めない: solve(..., idx=4, subset=[1], size=1, sum=1)
 * │           ├─ 5を含める: sum=6 > target=5 → 枝刈り
 * │           └─ 5を含めない: idx=5 → 終了
 * └─ 1を含めない: solve(..., idx=1, subset=[], size=0, sum=0)
 *    ├─ 2を含める: solve(..., idx=2, subset=[2], size=1, sum=2)
 *    │  ├─ 3を含める: solve(..., idx=3, subset=[2,3], size=2, sum=5)
 *    │  │  └─ sum=5 == target → "2 3"出力
 *    │  └─ 3を含めない: solve(..., idx=3, subset=[2], size=1, sum=2)
 *    │     ├─ 4を含める: sum=6 > target=5 → 枝刈り
 *    │     └─ 4を含めない: solve(..., idx=4, subset=[2], size=1, sum=2)
 *    │        ├─ 5を含める: sum=7 > target=5 → 枝刈り
 *    │        └─ 5を含めない: idx=5 → 終了
 *    └─ 2を含めない: solve(..., idx=2, subset=[], size=0, sum=0)
 *       ├─ 3を含める: solve(..., idx=3, subset=[3], size=1, sum=3)
 *       │  ├─ 4を含める: sum=7 > target=5 → 枝刈り
 *       │  └─ 4を含めない: solve(..., idx=4, subset=[3], size=1, sum=3)
 *       │     ├─ 5を含める: sum=8 > target=5 → 枝刈り
 *       │     └─ 5を含めない: idx=5 → 終了
 *       └─ 3を含めない: solve(..., idx=3, subset=[], size=0, sum=0)
 *          ├─ 4を含める: solve(..., idx=4, subset=[4], size=1, sum=4)
 *          │  ├─ 5を含める: sum=9 > target=5 → 枝刈り
 *          │  └─ 5を含めない: idx=5 → 終了
 *          └─ 4を含めない: solve(..., idx=4, subset=[], size=0, sum=0)
 *             ├─ 5を含める: solve(..., idx=5, subset=[5], size=1, sum=5)
 *             │  └─ sum=5 == target → "5"出力
 *             └─ 5を含めない: idx=5 → 終了
 */