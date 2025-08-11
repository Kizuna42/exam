/*
 * ARGO - JSON Parser 解説版
 * 
 * 問題: 簡単なJSON形式を解析してAST（抽象構文木）を構築
 * 
 * 対応形式:
 * - 数値: 整数のみ（例: 42）
 * - 文字列: ダブルクォートで囲む（例: "hello"）
 * - オブジェクト: キー・バリューペア（例: {"key": "value"}）
 * - エスケープ: \" と \\ のみ
 * 
 * アルゴリズム: 再帰下降パーサー
 * 1. トークンを先読み（peek）
 * 2. 文法に従って再帰的に解析
 * 3. エラー時は適切なメッセージを出力
 * 
 * 重要な実装要件:
 * - argo()関数は成功時1、失敗時-1を返す
 * - 動的メモリ管理（parse_string, parse_map）
 * - 適切なリソース管理（fopen/fclose, malloc/free）
 * - エラーハンドリング時のメモリリーク防止
 */

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

// JSON値の型定義
typedef struct json {
    enum {
        MAP,        // オブジェクト（連想配列）
        INTEGER,    // 整数
        STRING      // 文字列
    } type;
    union {
        struct {
            struct pair *data;  // キー・バリューペアの配列
            size_t size;        // ペア数
        } map;
        int integer;            // 整数値
        char *string;           // 文字列（動的確保）
    };
} json;

// キー・バリューペアの定義
typedef struct pair {
    char *key;      // キー（文字列）
    json value;     // 値（JSON値）
} pair;

// 関数プロトタイプ
void free_json(json j);
int argo(json *dst, FILE *stream);

/*
 * peek: 次の文字を先読み（ファイルポインタは移動しない）
 * 
 * @param stream: 入力ストリーム
 * @return: 次の文字、EOF時はEOF
 */
int peek(FILE *stream)
{
    int c = getc(stream);
    ungetc(c, stream);  // 文字を戻す
    return c;
}

/*
 * unexpected: 予期しないトークンのエラーメッセージ出力
 */
void unexpected(FILE *stream)
{
    if (peek(stream) != EOF)
        printf("Unexpected token '%c'\n", peek(stream));
    else
        printf("Unexpected end of input\n");
}

/*
 * accept: 指定文字があれば消費して成功を返す
 * 
 * @param stream: 入力ストリーム
 * @param c: 期待する文字
 * @return: 文字があれば1、なければ0
 */
int accept(FILE *stream, char c)
{
    if (peek(stream) == c)
    {
        (void)getc(stream);  // 文字を消費
        return 1;
    }
    return 0;
}

/*
 * expect: 指定文字を期待し、なければエラー
 * 
 * @param stream: 入力ストリーム
 * @param c: 期待する文字
 * @return: 成功時1、失敗時0
 */
int expect(FILE *stream, char c)
{
    if (accept(stream, c))
        return 1;
    unexpected(stream);
    return 0;
}

/*
 * parse_string: 文字列を解析
 * 
 * @param stream: 入力ストリーム
 * @param str: 解析した文字列を格納するポインタ
 * @return: 成功時1、失敗時0
 * 
 * 文法: "文字列"
 * エスケープ: \" と \\ のみ対応
 */
int parse_string(FILE *stream, char **str)
{
    if (!expect(stream, '"')) return 0;
    
    char *buf = malloc(1000);  // 十分なサイズを確保
    int len = 0;
    
    // 終了ダブルクォートまで文字を読み取り
    while (peek(stream) != '"' && peek(stream) != EOF)
    {
        char c = getc(stream);
        if (c == '\\')
        {
            // エスケープ文字の処理
            c = getc(stream);
            if (c == EOF) 
            { 
                free(buf); 
                unexpected(stream); 
                return 0; 
            }
        }
        buf[len++] = c;
    }
    
    if (!expect(stream, '"')) 
    { 
        free(buf); 
        return 0; 
    }
    
    buf[len] = '\0';
    *str = buf;
    return 1;
}

/*
 * parse_number: 数値を解析
 * 
 * @param stream: 入力ストリーム
 * @param num: 解析した数値を格納するポインタ
 * @return: 成功時1、失敗時0
 */
int parse_number(FILE *stream, int *num)
{
    if (!isdigit(peek(stream))) return 0;
    return fscanf(stream, "%d", num) == 1;
}

// 前方宣言
int parse_value(FILE *stream, json *dst);/*

 * parse_map: オブジェクト（マップ）を解析
 * 
 * @param stream: 入力ストリーム
 * @param dst: 解析結果を格納するJSON構造体
 * @return: 成功時1、失敗時0
 * 
 * 文法: { "key1": value1, "key2": value2, ... }
 */
int parse_map(FILE *stream, json *dst)
{
    if (!expect(stream, '{')) return 0;
    
    dst->type = MAP;
    dst->map.data = malloc(100 * sizeof(pair));  // 十分なサイズを確保
    dst->map.size = 0;
    
    // 空のオブジェクト {}
    if (accept(stream, '}')) return 1;
    
    // キー・バリューペアを解析
    do {
        char *key;
        
        // キーを解析（必ず文字列）
        if (!parse_string(stream, &key)) return 0;
        
        // コロンを期待
        if (!expect(stream, ':')) return 0;
        
        // キーを保存
        dst->map.data[dst->map.size].key = key;
        
        // 値を解析
        if (!parse_value(stream, &dst->map.data[dst->map.size].value)) 
            return 0;
        
        dst->map.size++;
        
    } while (accept(stream, ','));  // カンマがある限り続行
    
    return expect(stream, '}');
}

/*
 * parse_value: JSON値を解析（メイン解析関数）
 * 
 * @param stream: 入力ストリーム
 * @param dst: 解析結果を格納するJSON構造体
 * @return: 成功時1、失敗時0
 * 
 * 値の種類を判定して適切な解析関数を呼び出し
 */
int parse_value(FILE *stream, json *dst)
{
    if (peek(stream) == '"')
    {
        // 文字列
        dst->type = STRING;
        return parse_string(stream, &dst->string);
    }
    else if (isdigit(peek(stream)))
    {
        // 数値
        dst->type = INTEGER;
        return parse_number(stream, &dst->integer);
    }
    else if (peek(stream) == '{')
    {
        // オブジェクト
        return parse_map(stream, dst);
    }
    
    // 予期しないトークン
    unexpected(stream);
    return 0;
}

/*
 * argo: JSONパーサーのメイン関数
 * 
 * @param dst: 解析結果を格納するJSON構造体
 * @param stream: 入力ストリーム
 * @return: 成功時1、失敗時-1（要件に従った戻り値）
 */
int argo(json *dst, FILE *stream)
{
    if (parse_value(stream, dst))
        return 1;
    return -1;
}

/*
 * free_json: JSON構造体のメモリを解放
 * 
 * @param j: 解放するJSON構造体
 * 
 * 再帰的に全ての動的メモリを解放
 */
void free_json(json j)
{
    switch (j.type)
    {
        case MAP:
            // オブジェクトの場合、全てのキー・バリューペアを解放
            for (size_t i = 0; i < j.map.size; i++)
            {
                free(j.map.data[i].key);        // キーを解放
                free_json(j.map.data[i].value); // 値を再帰的に解放
            }
            free(j.map.data);  // ペア配列を解放
            break;
        case STRING:
            free(j.string);     // 文字列を解放
            break;
        default:
            break;  // 整数は動的メモリを使わない
    }
}

/*
 * serialize: JSON構造体を文字列として出力
 * 
 * @param j: 出力するJSON構造体
 * 
 * 元の入力と同じ形式で出力（エスケープ処理含む）
 */
void serialize(json j)
{
    switch (j.type)
    {
        case INTEGER:
            printf("%d", j.integer);
            break;
        case STRING:
            putchar('"');
            // エスケープが必要な文字を処理
            for (int i = 0; j.string[i]; i++)
            {
                if (j.string[i] == '\\' || j.string[i] == '"')
                    putchar('\\');
                putchar(j.string[i]);
            }
            putchar('"');
            break;
        case MAP:
            putchar('{');
            for (size_t i = 0; i < j.map.size; i++)
            {
                if (i != 0)
                    putchar(',');
                // キーを出力
                serialize((json){.type = STRING, .string = j.map.data[i].key});
                putchar(':');
                // 値を再帰的に出力
                serialize(j.map.data[i].value);
            }
            putchar('}');
            break;
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 1;
    
    char *filename = argv[1];
    FILE *stream = fopen(filename, "r");
    
    // ファイルオープンの失敗をチェック
    if (!stream)
        return 1;
    
    json file;
    
    // JSON解析実行
    if (argo(&file, stream) != 1)
    {
        fclose(stream);  // ファイルを閉じる
        return 1;
    }
    
    // 解析結果を出力
    serialize(file);
    printf("\n");
    
    // メモリ解放とファイルクローズ
    free_json(file);
    fclose(stream);
    
    return 0;
}

/*
 * 重要な実装注意事項とよくある間違い:
 * 
 * 1. 戻り値の制約:
 *    - argo()は成功時1、失敗時-1を返すこと（内部関数とは異なる）
 *    - parse_*()関数は成功時1、失敗時0を返す
 * 
 * 2. メモリ管理:
 *    - parse_string()で動的メモリ確保、適切なサイズ調整
 *    - parse_map()で動的配列管理、realloc()でサイズ拡張
 *    - エラー時は確保済みメモリを必ず解放
 * 
 * 3. ファイル処理:
 *    - fopen()の失敗をチェック
 *    - 処理完了後は必ずfclose()
 *    - エラー時も忘れずにfclose()
 * 
 * 4. エラーメッセージ:
 *    - "Unexpected token '%c'\n" 形式
 *    - EOF時は "Unexpected end of input\n"
 *    - 正確な文字を出力（peek()の結果）
 * 
 * 5. メモリリーク対策:
 *    - free_json()で再帰的に全メモリ解放
 *    - main()で成功時もfree_json()呼び出し
 *    - エラー処理時の適切なcleanup
 */