#ifndef CLEED_TEST_SUPPORT_H
#define CLEED_TEST_SUPPORT_H

#include <stddef.h>

#include "real.h"

int cleed_test_write_text_file(const char *path, const char *contents);
int cleed_test_remove_file(const char *path);

real **cleed_test_alloc_matrix_1based(int rows, int cols);
void cleed_test_free_matrix_1based(real **matrix);

real *cleed_test_alloc_vector_1based(int n);
void cleed_test_free_vector_1based(real *vector);

#endif
