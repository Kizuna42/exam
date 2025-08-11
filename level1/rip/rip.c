// Allowed functions: puts, write
// 仕様: 最小削除でバランス化。削除は空白 ' ' に置換し、全解を出力。重複は同一連続括弧で同位置を複数削除しないことで抑制。

#include <stdio.h>

static int str_len(const char *s){ int i=0; while(s[i]) i++; return i; }

static int min_removals(const char *s, int *open_rem, int *close_rem)
{
    int open = 0, close = 0;
    for (int i = 0; s[i]; i++)
    {
        if (s[i] == '(') open++;
        else if (s[i] == ')') { if (open>0) open--; else close++; }
    }
    *open_rem = open; *close_rem = close; return open+close;
}

static void dfs(const char *s, int i, int open, int close, int balance, char *buf, int k)
{
    if (!s[i])
    {
        if (open==0 && close==0 && balance==0) { buf[k]='\0'; puts(buf); }
        return;
    }
    char c = s[i];
    if (c != '(' && c != ')')
    {
        buf[k]=c; dfs(s, i+1, open, close, balance, buf, k+1); return;
    }

    if (c=='(')
    {
        // 削除分岐
        if (open>0)
        {   buf[k]=' '; dfs(s, i+1, open-1, close, balance, buf, k+1); }
        // 削除しない分岐
        buf[k]='(';
        dfs(s, i+1, open, close, balance+1, buf, k+1);
    }
    else // ')'
    {
        // 削除しない分岐（バランスが取れている場合のみ）
        if (balance>0)
        {
            buf[k]=')';
            dfs(s, i+1, open, close, balance-1, buf, k+1);
        }
        // 削除分岐
        if (close>0)
        {   buf[k]=' '; dfs(s, i+1, open, close-1, balance, buf, k+1); }
    }
}

int main(int ac, char **av)
{
    if (ac != 2) return 1;
    int open_rem=0, close_rem=0; (void)min_removals(av[1], &open_rem, &close_rem);
    int n = str_len(av[1]);
    char buf[n + 1];
    dfs(av[1], 0, open_rem, close_rem, 0, buf, 0);
    return 0;
}