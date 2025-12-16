#include "test_support.h"

#include <stdio.h>
#include <stdlib.h>

int cleed_test_write_text_file(const char *path, const char *contents)
{
    FILE *file = fopen(path, "w");
    if (!file) {
        return -1;
    }
    if (contents) {
        fputs(contents, file);
    }
    fclose(file);
    return 0;
}

int cleed_test_remove_file(const char *path)
{
    if (!path) {
        return 0;
    }
    return remove(path);
}

real **cleed_test_alloc_matrix_1based(int rows, int cols)
{
    if (rows <= 0 || cols <= 0) {
        return NULL;
    }

    real **matrix = (real **)calloc((size_t)rows + 1, sizeof(real *));
    if (!matrix) {
        return NULL;
    }

    real *data = (real *)calloc(((size_t)rows + 1) * ((size_t)cols + 1), sizeof(real));
    if (!data) {
        free(matrix);
        return NULL;
    }

    matrix[0] = data;
    for (int row = 1; row <= rows; row++) {
        matrix[row] = data + ((size_t)row * ((size_t)cols + 1));
    }

    return matrix;
}

void cleed_test_free_matrix_1based(real **matrix)
{
    if (!matrix) {
        return;
    }
    free(matrix[0]);
    free(matrix);
}

real *cleed_test_alloc_vector_1based(int n)
{
    if (n <= 0) {
        return NULL;
    }
    return (real *)calloc((size_t)n + 1, sizeof(real));
}

void cleed_test_free_vector_1based(real *vector)
{
    free(vector);
}
