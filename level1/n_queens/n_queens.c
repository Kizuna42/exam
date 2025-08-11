// Allowed: atoi, fprintf, write, calloc, malloc, free, realloc, stdout, stderr
#include <stdio.h>
#include <stdlib.h>

static int my_abs(int x){ return x < 0 ? -x : x; }

static int is_safe(int *tab, int col, int row)
{
    for (int i = 0; i < col; i++)
        if (tab[i] == row || my_abs(i - col) == my_abs(tab[i] - row))
            return 0;
    return 1;
}

static void solve(int *tab, int n, int col)
{
    if (col == n)
    {
        for (int i = 0; i < n; i++)
        {
            fprintf(stdout, "%d", tab[i]);
            if (i < n - 1) fprintf(stdout, " ");
        }
        fprintf(stdout, "\n");
        return;
    }
    for (int i = 0; i < n; i++)
        if (is_safe(tab, col, i)) { tab[col] = i; solve(tab, n, col + 1); }
}

int main(int ac, char **argv)
{
    if (ac != 2) return 0;
    int n = atoi(argv[1]);
    if (n <= 0) return 0;
    int *tab = (int *)calloc(n, sizeof(int));
    if (!tab) return 1;
    solve(tab, n, 0);
    free(tab);
    return 0;
}