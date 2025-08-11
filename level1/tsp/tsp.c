#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>

float distance(float a[2], float b[2])
{
    return sqrtf((b[0] - a[0]) * (b[0] - a[0]) + (b[1] - a[1]) * (b[1] - a[1]));
}

void swap(int *a, int *b)
{
    int temp = *a; *a = *b; *b = temp;
}

float path_dist(float (*array)[2], int *path, ssize_t size)
{
    float dist = 0.0;
    for (ssize_t i = 0; i < size; i++)
        dist += distance(array[path[i]], array[path[(i + 1) % size]]);
    return dist;
}

void permute(float (*array)[2], int *path, ssize_t size, int start, float *best)
{
    if (start == size)
    {
        float curr = path_dist(array, path, size);
        if (curr < *best) *best = curr;
        return;
    }
    for (int i = start; i < size; i++)
    {
        swap(&path[start], &path[i]);
        permute(array, path, size, start + 1, best);
        swap(&path[start], &path[i]);
    }
}

float tsp(float (*array)[2], ssize_t size)
{
    if (size <= 1) return 0.0;
    int *path = malloc(size * sizeof(int));
    for (int i = 0; i < size; i++) path[i] = i;
    float best = __FLT_MAX__;
    permute(array, path, size, 1, &best);
    free(path);
    return best;
}

ssize_t file_size(FILE *file)
{
    char *buffer = NULL;
    size_t n = 0;
    ssize_t ret;
    errno = 0;
    for (ret = 0; getline(&buffer, &n, file) != -1; ret++);
    free(buffer);
    if (errno) return -1;
    if (file != stdin && fseek(file, 0, SEEK_SET)) return -1;
    return ret;
}

int retrieve_file(float (*array)[2], FILE *file)
{
    int tmp;
    for (size_t i = 0; (tmp = fscanf(file, "%f, %f\n", array[i] + 0, array[i] + 1)) != EOF; i++)
        if (tmp != 2) { errno = EINVAL; return -1; }
    if (ferror(file)) return -1;
    return 0;
}

int main(int ac, char **av)
{
    char *filename = "stdin";
    FILE *file = stdin;
    if (ac > 1) { filename = av[1]; file = fopen(filename, "r"); }
    if (!file) { fprintf(stderr, "Error opening %s\n", filename); return 1; }
    
    if (file == stdin)
    {
        // For stdin, read dynamically
        float (*array)[2] = NULL;
        size_t capacity = 0, size = 0;
        float x, y;
        
        while (fscanf(file, "%f, %f", &x, &y) == 2)
        {
            if (size >= capacity)
            {
                capacity = capacity ? capacity * 2 : 8;
                float (*new_array)[2] = realloc(array, capacity * sizeof(float[2]));
                if (!new_array) { free(array); fprintf(stderr, "Error allocating memory\n"); return 1; }
                array = new_array;
            }
            array[size][0] = x;
            array[size][1] = y;
            size++;
        }
        
        if (size == 0) { free(array); fprintf(stderr, "Error reading %s\n", filename); return 1; }
        fprintf(stdout, "%.2f\n", tsp(array, size));
        free(array);
    }
    else
    {
        ssize_t size = file_size(file);
        if (size <= 0) { fprintf(stderr, "Error reading %s\n", filename); fclose(file); return 1; }
        float (*array)[2] = calloc(size, sizeof(float[2]));
        if (!array) { fprintf(stderr, "Error allocating memory\n"); fclose(file); return 1; }
        if (retrieve_file(array, file) == -1) { fprintf(stderr, "Error reading %s\n", filename); fclose(file); free(array); return 1; }
        fclose(file);
        fprintf(stdout, "%.2f\n", tsp(array, size));
        free(array);
    }
    return 0;
} 