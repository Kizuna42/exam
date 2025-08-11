#include <stdio.h>
#include <stdlib.h>

static void solve(int *arr, int n, int target, int idx, int *subset, int size, int sum)
{
    if (idx == n)
    {
        if (sum == target)
        {
            for (int i = 0; i < size; i++)
            {
                printf("%d", subset[i]);
                if (i < size - 1) printf(" ");
            }
            printf("\n");
        }
        return;
    }

    subset[size] = arr[idx];
    solve(arr, n, target, idx + 1, subset, size + 1, sum + arr[idx]);
    solve(arr, n, target, idx + 1, subset, size, sum);
}

int main(int ac, char **av)
{
    if (ac < 2) return 1;
    int target = atoi(av[1]);
    int n = ac - 2;

    if (n == 0)
    {
        if (target == 0) printf("\n");
        return 0;
    }

    int *arr = (int *)malloc(n * sizeof(int));
    int *subset = (int *)malloc(n * sizeof(int));
    if (!arr || !subset) { free(arr); free(subset); return 1; }

    for (int i = 0; i < n; i++)
        arr[i] = atoi(av[i + 2]);

    solve(arr, n, target, 0, subset, 0, 0);

    free(arr);
    free(subset);
    return 0;
}