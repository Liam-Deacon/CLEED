#ifndef CLEED_TEST_SUPPORT_H
#define CLEED_TEST_SUPPORT_H

// cppcheck-suppress missingIncludeSystem
#include <stddef.h>
// cppcheck-suppress missingIncludeSystem
#include <math.h>
// cppcheck-suppress missingIncludeSystem
#include <stdio.h>

#include "real.h"

#define CLEED_TEST_ASSERT(condition)                                            \
    do {                                                                        \
        if (!(condition)) {                                                     \
            fprintf(stderr, "assert failed: %s (%s:%d)\n", #condition,          \
                    __FILE__, __LINE__);                                        \
            return 1;                                                           \
        }                                                                       \
    } while (0)

#define CLEED_TEST_ASSERT_NEAR(actual, expected, tol)                           \
    do {                                                                        \
        const double diff = fabs((double)(actual) - (double)(expected));        \
        if (diff > (tol)) {                                                     \
            fprintf(stderr,                                                     \
                    "assert failed: %s ~= %s (got=%g expected=%g tol=%g) "      \
                    "(%s:%d)\n",                                                \
                    #actual, #expected, (double)(actual), (double)(expected),   \
                    (double)(tol), __FILE__, __LINE__);                         \
            return 1;                                                           \
        }                                                                       \
    } while (0)

int cleed_test_write_text_file(const char *path, const char *contents);
int cleed_test_remove_file(const char *path);

real **cleed_test_alloc_matrix_1based(int rows, int cols);
void cleed_test_free_matrix_1based(real **matrix);

real *cleed_test_alloc_vector_1based(int n);
void cleed_test_free_vector_1based(real *vector);

#endif
