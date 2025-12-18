// cppcheck-suppress missingIncludeSystem
#include <stdio.h>
// cppcheck-suppress missingIncludeSystem
#include <stdlib.h>
// cppcheck-suppress missingIncludeSystem
#include <string.h>

static const char *arg_value(int argc, char **argv, const char *key)
{
    for (int i = 1; i + 1 < argc; i++) {
        if (strcmp(argv[i], key) == 0) {
            return argv[i + 1];
        }
    }
    return NULL;
}

int main(int argc, char **argv)
{
    const char *out_path = arg_value(argc, argv, "-o");
    if (out_path == NULL) {
        fprintf(stderr, "fake_csearch_leed: missing -o <output>\n");
        return 2;
    }

    FILE *fp = fopen(out_path, "wb");
    if (fp == NULL) {
        fprintf(stderr, "fake_csearch_leed: failed to open output: %s\n", out_path);
        return 3;
    }

    fputs("# fake cleed output for csearch e2e tests\n", fp);
    fclose(fp);
    return 0;
}

