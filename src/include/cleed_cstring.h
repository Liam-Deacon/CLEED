#ifndef CLEED_CSTRING_H
#define CLEED_CSTRING_H

/*
 * Minimal C string declarations for translation units that historically relied
 * on implicit function declarations. This avoids newer Clang treating implicit
 * declarations as hard errors, without requiring system headers that may be
 * unavailable to static analysis tooling.
 */

#if defined(_WIN64)
typedef unsigned long long cleed_size_t;
#elif defined(_WIN32)
typedef unsigned int cleed_size_t;
#else
typedef unsigned long cleed_size_t;
#endif

int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, cleed_size_t n);
char *strcpy(char *dst, const char *src);
char *strcat(char *dst, const char *src);
char *strrchr(const char *s, int c);

#endif /* CLEED_CSTRING_H */
