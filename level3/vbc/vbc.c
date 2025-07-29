#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct node {
    enum {
        ADD,
        MULTI,
        VAL
    }   type;
    int val;
    struct node *l;
    struct node *r;
}   node;

node *parse_expr(char **s);
node *parse_term(char **s);
node *parse_factor(char **s);

node    *new_node(node n)
{
    node *ret = calloc(1, sizeof(n));
    if (!ret) return NULL;
    *ret = n;
    return ret;
}

void    destroy_tree(node *n)
{
    if (!n) return;
    if (n->type != VAL)
    {
        destroy_tree(n->l);
        destroy_tree(n->r);
    }
    free(n);
}

void    unexpected(char c)
{
    if (c) printf("Unexpected token '%c'\n", c);
    else printf("Unexpected end of input\n");
}

int accept(char **s, char c)
{
    if (**s == c)
    {
        (*s)++;
        return 1;
    }
    return 0;
}

int expect(char **s, char c)
{
    if (accept(s, c)) return 1;
    unexpected(**s);
    return 0;
}

node *parse_factor(char **s)
{
    if (isdigit(**s))
    {
        int val = **s - '0';
        (*s)++;
        return new_node((node){.type = VAL, .val = val});
    }
    else if (accept(s, '('))
    {
        node *expr = parse_expr(s);
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

node *parse_term(char **s)
{
    node *left = parse_factor(s);
    if (!left) return NULL;
    
    while (**s == '*')
    {
        (*s)++;
        node *right = parse_factor(s);
        if (!right)
        {
            destroy_tree(left);
            return NULL;
        }
        left = new_node((node){.type = MULTI, .l = left, .r = right});
    }
    
    return left;
}

node *parse_expr(char **s)
{
    node *left = parse_term(s);
    if (!left) return NULL;
    
    while (**s == '+')
    {
        (*s)++;
        node *right = parse_term(s);
        if (!right)
        {
            destroy_tree(left);
            return NULL;
        }
        left = new_node((node){.type = ADD, .l = left, .r = right});
    }
    
    return left;
}

node    *parse_expr_main(char *s)
{
    node *ret = parse_expr(&s);
    if (!ret) return NULL;
    
    if (*s) 
    {
        unexpected(*s);
        destroy_tree(ret);
        return NULL;
    }
    return ret;
}

int eval_tree(node *tree)
{
    switch (tree->type)
    {
        case ADD:
            return eval_tree(tree->l) + eval_tree(tree->r);
        case MULTI:
            return eval_tree(tree->l) * eval_tree(tree->r);
        case VAL:
            return tree->val;
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 1;
    node *tree = parse_expr_main(argv[1]);
    if (!tree) return 1;
    printf("%d\n", eval_tree(tree));
    destroy_tree(tree);
    return 0;
} 