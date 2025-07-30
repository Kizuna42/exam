/*
 * VBC - 数式計算機 解説版
 * 
 * 問題: 数式を解析して計算結果を出力
 * 対応演算: +（加算）、*（乗算）、()（括弧）
 * 
 * 文法（BNF記法）:
 * expression := term (('+') term)*
 * term       := factor (('*') factor)*  
 * factor     := digit | '(' expression ')'
 * digit      := '0' | '1' | ... | '9'
 * 
 * アルゴリズム: 再帰下降パーサー + AST構築 + 評価
 * 演算子優先度: * > +
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// AST（抽象構文木）のノード定義
typedef struct node {
    enum {
        ADD,    // 加算ノード
        MULTI,  // 乗算ノード
        VAL     // 値ノード
    } type;
    int val;            // 値ノードの場合の数値
    struct node *l;     // 左の子ノード
    struct node *r;     // 右の子ノード
} node;

// 関数プロトタイプ
node *parse_expr(char **s);
node *parse_term(char **s);
node *parse_factor(char **s);

/*
 * new_node: 新しいノードを作成
 * 
 * @param n: ノードの初期値
 * @return: 作成されたノードのポインタ
 */
node *new_node(node n)
{
    node *ret = calloc(1, sizeof(n));
    if (!ret) return NULL;
    *ret = n;
    return ret;
}

/*
 * destroy_tree: ASTを再帰的に解放
 * 
 * @param n: 解放するノード
 */
void destroy_tree(node *n)
{
    if (!n) return;
    if (n->type != VAL)
    {
        destroy_tree(n->l);  // 左の子を解放
        destroy_tree(n->r);  // 右の子を解放
    }
    free(n);
}

/*
 * unexpected: エラーメッセージ出力
 * 
 * @param c: 予期しない文字
 */
void unexpected(char c)
{
    if (c) 
        printf("Unexpected token '%c'\n", c);
    else 
        printf("Unexpected end of input\n");
}

/*
 * accept: 指定文字があれば消費
 * 
 * @param s: 文字列ポインタの参照
 * @param c: 期待する文字
 * @return: 文字があれば1、なければ0
 */
int accept(char **s, char c)
{
    if (**s == c)
    {
        (*s)++;  // 文字列ポインタを進める
        return 1;
    }
    return 0;
}

/*
 * expect: 指定文字を期待し、なければエラー
 * 
 * @param s: 文字列ポインタの参照
 * @param c: 期待する文字
 * @return: 成功時1、失敗時0
 */
int expect(char **s, char c)
{
    if (accept(s, c)) 
        return 1;
    unexpected(**s);
    return 0;
}

/*
 * parse_factor: 因子を解析
 * 
 * @param s: 文字列ポインタの参照
 * @return: 解析されたASTノード
 * 
 * 文法: factor := digit | '(' expression ')'
 */
node *parse_factor(char **s)
{
    if (isdigit(**s))
    {
        // 数字の場合
        int val = **s - '0';  // 文字を数値に変換
        (*s)++;
        return new_node((node){.type = VAL, .val = val});
    }
    else if (accept(s, '('))
    {
        // 括弧の場合
        node *expr = parse_expr(s);  // 括弧内の式を解析
        if (!expr || !expect(s, ')'))
        {
            destroy_tree(expr);
            return NULL;
        }
        return expr;
    }
    
    unexpected(**s);
    return NULL;
}

/*
 * parse_term: 項を解析
 * 
 * @param s: 文字列ポインタの参照
 * @return: 解析されたASTノード
 * 
 * 文法: term := factor (('*') factor)*
 * 左結合で乗算を処理
 */
node *parse_term(char **s)
{
    node *left = parse_factor(s);
    if (!left) return NULL;
    
    // '*' がある限り続行
    while (**s == '*')
    {
        (*s)++;  // '*' を消費
        node *right = parse_factor(s);
        if (!right)
        {
            destroy_tree(left);
            return NULL;
        }
        // 乗算ノードを作成
        left = new_node((node){.type = MULTI, .l = left, .r = right});
    }
    
    return left;
}/*
 * parse
_expr: 式を解析
 * 
 * @param s: 文字列ポインタの参照
 * @return: 解析されたASTノード
 * 
 * 文法: expression := term (('+') term)*
 * 左結合で加算を処理
 */
node *parse_expr(char **s)
{
    node *left = parse_term(s);
    if (!left) return NULL;
    
    // '+' がある限り続行
    while (**s == '+')
    {
        (*s)++;  // '+' を消費
        node *right = parse_term(s);
        if (!right)
        {
            destroy_tree(left);
            return NULL;
        }
        // 加算ノードを作成
        left = new_node((node){.type = ADD, .l = left, .r = right});
    }
    
    return left;
}

/*
 * parse_expr_main: メイン解析関数
 * 
 * @param s: 解析する文字列
 * @return: 解析されたASTノード
 * 
 * 文字列全体を解析し、余分な文字がないかチェック
 */
node *parse_expr_main(char *s)
{
    node *ret = parse_expr(&s);
    if (!ret) return NULL;
    
    // 文字列の終端でない場合はエラー
    if (*s) 
    {
        unexpected(*s);
        destroy_tree(ret);
        return NULL;
    }
    return ret;
}

/*
 * eval_tree: ASTを評価して計算結果を求める
 * 
 * @param tree: 評価するASTノード
 * @return: 計算結果
 * 
 * 後順走査で再帰的に計算
 */
int eval_tree(node *tree)
{
    switch (tree->type)
    {
        case ADD:
            // 左の子 + 右の子
            return eval_tree(tree->l) + eval_tree(tree->r);
        case MULTI:
            // 左の子 * 右の子
            return eval_tree(tree->l) * eval_tree(tree->r);
        case VAL:
            // 値をそのまま返す
            return tree->val;
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 1;
    
    // 数式を解析してASTを構築
    node *tree = parse_expr_main(argv[1]);
    if (!tree) return 1;
    
    // ASTを評価して結果を出力
    printf("%d\n", eval_tree(tree));
    
    // メモリ解放
    destroy_tree(tree);
    return 0;
}

/*
 * 解析例: "3+4*5"
 * 
 * 1. parse_expr() 呼び出し
 * 2. parse_term() で "3" を解析 → VALノード(3)
 * 3. '+' を検出
 * 4. parse_term() で "4*5" を解析:
 *    a. parse_factor() で "4" → VALノード(4)
 *    b. '*' を検出
 *    c. parse_factor() で "5" → VALノード(5)
 *    d. MULTIノード(4, 5) を作成
 * 5. ADDノード(3, MULTI(4,5)) を作成
 * 
 * AST構造:
 *     ADD
 *    /   \
 *   3    MULTI
 *        /   \
 *       4     5
 * 
 * 評価:
 * 1. eval_tree(ADD) 呼び出し
 * 2. eval_tree(3) → 3
 * 3. eval_tree(MULTI) → eval_tree(4) * eval_tree(5) → 4 * 5 → 20
 * 4. 3 + 20 → 23
 * 
 * 演算子優先度の実現:
 * - parse_expr() は parse_term() を呼び出す
 * - parse_term() は parse_factor() を呼び出す
 * - この階層構造により * が + より先に評価される
 */