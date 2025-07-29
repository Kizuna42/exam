#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int is_valid(char *s)
{
    int count = 0;
    for (int i = 0; s[i]; i++)
    {
        if (s[i] == '(') count++;
        else if (s[i] == ')') count--;
        if (count < 0) return 0;
    }
    return count == 0;
}

void solve(char *s, int idx, int open_rem, int close_rem, char *current, int pos, char **results, int *count)
{
    if (idx == strlen(s))
    {
        if (open_rem == 0 && close_rem == 0 && is_valid(current))
        {
            current[pos] = '\0';
            for (int i = 0; i < *count; i++)
                if (strcmp(results[i], current) == 0) return;
            results[*count] = malloc(strlen(current) + 1);
            strcpy(results[*count], current);
            (*count)++;
        }
        return;
    }
    char ch = s[idx];
    if ((ch == '(' && open_rem > 0) || (ch == ')' && close_rem > 0))
    {
        current[pos] = ' ';
        solve(s, idx + 1, (ch == '(') ? open_rem - 1 : open_rem, 
        (ch == ')') ? close_rem - 1 : close_rem, current, pos + 1, results, count);
    }
    current[pos] = ch;
    solve(s, idx + 1, open_rem, close_rem, current, pos + 1, results, count);
}

int main(int ac, char **av)
{
    if (ac != 2) return 1;
    char *s = av[1];
    int len = strlen(s);
    int open_rem = 0, close_rem = 0;
    for (int i = 0; i < len; i++)
    {
        if (s[i] == '(') open_rem++;
        else if (s[i] == ')')
        {
            if (open_rem > 0) open_rem--;
            else close_rem++;
        }
    }
    char **results = malloc(1000 * sizeof(char*));
    char *current = malloc(len + 1);
    int count = 0;
    solve(s, 0, open_rem, close_rem, current, 0, results, &count);
    for (int i = 0; i < count; i++)
    {
        puts(results[i]);
        free(results[i]);
    }
    free(results);
    free(current);
    return 0;
} 