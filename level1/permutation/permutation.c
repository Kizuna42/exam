#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void swap(char *a, char *b)
{
    char temp = *a;
    *a = *b;
    *b = temp;
}

void permute(char *str, int l, int r, char **result, int *count)
{
    if (l == r)
    {
        result[*count] = malloc(strlen(str) + 1);
        strcpy(result[*count], str);
        (*count)++;
    }
    else
    {
        for (int i = l; i <= r; i++)
        {
            swap(&str[l], &str[i]);
            permute(str, l + 1, r, result, count);
            swap(&str[l], &str[i]);
        }
    }
}

int compare(const void *a, const void *b)
{
    return strcmp(*(char**)a, *(char**)b);
}

int main(int ac, char **av)
{
    if (ac != 2) return 1;
    char *str = av[1];
    int len = strlen(str);
    int fact = 1;
    for (int i = 2; i <= len; i++) fact *= i;
    char **result = malloc(fact * sizeof(char*));
    int count = 0;
    permute(str, 0, len - 1, result, &count);
    qsort(result, count, sizeof(char*), compare);
    for (int i = 0; i < count; i++)
    {
        puts(result[i]);
        free(result[i]);
    }
    free(result);
    return 0;
}