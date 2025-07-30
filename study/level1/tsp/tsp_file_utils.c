/*
 * TSP - ファイル処理部分の解説
 * 
 * この部分は与えられたコードを使用
 * 標準入力から都市の座標を読み込む処理
 */

/*
 * file_size: ファイルの行数を数える
 * 
 * @param file: 読み込むファイル
 * @return: 行数、エラーの場合-1
 * 
 * 動作:
 * 1. getline()で1行ずつ読み込んで行数をカウント
 * 2. ファイルポインタを先頭に戻す
 */
ssize_t file_size(FILE *file)
{
    char *buffer = NULL;
    size_t n = 0;
    ssize_t ret;
    
    errno = 0;
    
    // 1行ずつ読み込んで行数をカウント
    for (ret = 0; getline(&buffer, &n, file) != -1; ret++);
    
    free(buffer);
    
    // エラーチェックとファイルポインタのリセット
    if (errno || fseek(file, 0, SEEK_SET)) 
        return -1;
    
    return ret;
}

/*
 * retrieve_file: ファイルから座標データを読み込み
 * 
 * @param array: 座標を格納する配列
 * @param file: 読み込むファイル
 * @return: 成功時0、エラー時-1
 * 
 * 期待する形式: "x座標, y座標\n"
 */
int retrieve_file(float (*array)[2], FILE *file)
{
    int tmp;
    
    // 各行から座標を読み込み
    for (size_t i = 0; (tmp = fscanf(file, "%f, %f\n", array[i] + 0, array[i] + 1)) != EOF; i++)
    {
        // fscanfが2つの値を正常に読み込めなかった場合
        if (tmp != 2) 
        { 
            errno = EINVAL; 
            return -1; 
        }
    }
    
    // ファイル読み込みエラーチェック
    if (ferror(file)) 
        return -1;
    
    return 0;
}

int main(int ac, char **av)
{
    // デフォルトは標準入力
    char *filename = "stdin";
    FILE *file = stdin;
    
    // コマンドライン引数でファイル名が指定された場合
    if (ac > 1) 
    { 
        filename = av[1]; 
        file = fopen(filename, "r"); 
    }
    
    // ファイルオープンエラーチェック
    if (!file) 
    { 
        fprintf(stderr, "Error opening %s: %m\n", filename); 
        return 1; 
    }
    
    // ファイルサイズ（都市数）を取得
    ssize_t size = file_size(file);
    if (size == -1) 
    { 
        fprintf(stderr, "Error reading %s: %m\n", filename); 
        fclose(file); 
        return 1; 
    }
    
    // 座標配列を確保
    float (*array)[2] = calloc(size, sizeof(float[2]));
    if (!array) 
    { 
        fprintf(stderr, "Error: %m\n"); 
        fclose(file); 
        return 1; 
    }
    
    // 座標データを読み込み
    if (retrieve_file(array, file) == -1) 
    { 
        fprintf(stderr, "Error reading %s: %m\n", av[1]); 
        fclose(file); 
        free(array); 
        return 1; 
    }
    
    // ファイルを閉じる（標準入力以外の場合）
    if (ac > 1) 
        fclose(file);
    
    // TSP問題を解いて結果を出力
    printf("%.2f\n", tsp(array, size));
    
    free(array);
    return 0;
}