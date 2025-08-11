// Allowed functions: puts, malloc/calloc/realloc/free, write
// 仕様: 引数の文字列の順列を辞書順で出力

#include <stdio.h>
#include <stdlib.h>

static size_t my_strlen(const char *s)
{
    size_t n = 0;
    while (s[n]) n++;
    return n;
}

static void swap_char(char *a, char *b)
{
    char t = *a; *a = *b; *b = t;
}

static void sort_string(char *s)
{
    // 簡易挿入ソート（辞書順の初期化）
    for (size_t i = 1; s[i]; i++)
    {
        char key = s[i];
        size_t j = i;
        while (j > 0 && s[j - 1] > key)
        {
            s[j] = s[j - 1];
            j--;
        }
        s[j] = key;
    }
}

static int next_permutation(char *s)
{
    int n = (int)my_strlen(s);
    if (n < 2) return 0;
    int i = n - 2;
    while (i >= 0 && s[i] >= s[i + 1]) i--;
    if (i < 0) return 0; // これが最後の順列
    int j = n - 1;
    while (s[j] <= s[i]) j--;
    swap_char(&s[i], &s[j]);
    // 末尾を反転
    int l = i + 1, r = n - 1;
    while (l < r) swap_char(&s[l++], &s[r--]);
    return 1;
}

int main(int ac, char **av)
{
    if (ac != 2) return 1;
    size_t len = my_strlen(av[1]);
    char *str = (char *)malloc(len + 1);
    if (!str) return 1;
    for (size_t i = 0; i <= len; i++) str[i] = av[1][i];

    sort_string(str);
    do { puts(str); } while (next_permutation(str));

    free(str);
    return 0;
}