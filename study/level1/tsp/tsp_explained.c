/*
 * TSP - 巡回セールスマン問題 解説版
 * 
 * 問題: N個の都市を全て訪問して出発点に戻る最短経路の距離を求める
 * 
 * アルゴリズム:
 * 1. 標準入力から都市の座標を読み込み
 * 2. 全ての順列（経路）を試して最短距離を計算
 * 3. 最初の都市を固定して残りの都市の順列を生成（対称性利用）
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>

/*
 * distance: 2点間のユークリッド距離を計算
 * 
 * @param a: 点Aの座標 [x, y]
 * @param b: 点Bの座標 [x, y]  
 * @return: 2点間の距離
 * 
 * 公式: √((x2-x1)² + (y2-y1)²)
 */
float distance(float a[2], float b[2])
{
    return sqrtf((b[0] - a[0]) * (b[0] - a[0]) + (b[1] - a[1]) * (b[1] - a[1]));
}

/*
 * swap: 配列の2つの要素を交換
 */
void swap(int *a, int *b)
{
    int temp = *a; 
    *a = *b; 
    *b = temp;
}

/*
 * path_dist: 指定された経路の総距離を計算
 * 
 * @param array: 都市の座標配列
 * @param path: 訪問順序を表す配列
 * @param size: 都市数
 * @return: 経路の総距離（最後の都市から最初の都市への距離も含む）
 */
float path_dist(float (*array)[2], int *path, ssize_t size)
{
    float dist = 0.0;
    
    // 各都市間の距離を累積
    for (ssize_t i = 0; i < size; i++)
    {
        // 最後の都市から最初の都市への距離も含める（巡回）
        dist += distance(array[path[i]], array[path[(i + 1) % size]]);
    }
    
    return dist;
}

/*
 * permute: 順列を生成して最短距離を探索
 * 
 * @param array: 都市の座標配列
 * @param path: 現在の経路
 * @param size: 都市数
 * @param start: 固定する都市数（最初のstart個の都市は固定）
 * @param best: 現在の最短距離（参照渡し）
 * 
 * 最適化: 最初の都市を固定することで計算量を1/n削減
 */
void permute(float (*array)[2], int *path, ssize_t size, int start, float *best)
{
    // ベースケース: 全ての都市の順序が決定
    if (start == size)
    {
        float curr = path_dist(array, path, size);
        if (curr < *best) 
            *best = curr;
        return;
    }
    
    // start位置以降の全ての都市を試す
    for (int i = start; i < size; i++)
    {
        // start位置とi位置の都市を交換
        swap(&path[start], &path[i]);
        
        // 残りの都市を再帰的に処理
        permute(array, path, size, start + 1, best);
        
        // バックトラック: 元の状態に戻す
        swap(&path[start], &path[i]);
    }
}

/*
 * tsp: TSP問題のメイン関数
 * 
 * @param array: 都市の座標配列
 * @param size: 都市数
 * @return: 最短経路の距離
 */
float tsp(float (*array)[2], ssize_t size)
{
    if (size <= 1) return 0.0;
    
    // 初期経路: 0, 1, 2, ..., size-1
    int *path = malloc(size * sizeof(int));
    for (int i = 0; i < size; i++) 
        path[i] = i;
    
    float best = __FLT_MAX__;  // 最大値で初期化
    
    // 最初の都市（インデックス0）を固定して残りを順列生成
    permute(array, path, size, 1, &best);
    
    free(path);
    return best;
}