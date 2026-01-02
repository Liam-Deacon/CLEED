#include "cleed_mode.h"
#include "test_support.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#endif

static int write_temp_file(const char *contents, char *path, size_t path_size)
{
#ifdef _WIN32
    char *name = tmpnam(NULL);
    if (!name) {
        return -1;
    }
    if (strlen(name) + 1 > path_size) {
        return -1;
    }
    strcpy(path, name);
#else
    char template_path[] = "/tmp/cleed_mode_XXXXXX";
    int fd = mkstemp(template_path);
    if (fd < 0) {
        return -1;
    }
    close(fd);
    if (strlen(template_path) + 1 > path_size) {
        return -1;
    }
    strcpy(path, template_path);
#endif
    return cleed_test_write_text_file(path, contents);
}

int main(void)
{
    CLEED_TEST_ASSERT(cleed_parse_mode_value("auto") == CLEED_MODE_AUTO);
    CLEED_TEST_ASSERT(cleed_parse_mode_value("YES") == CLEED_MODE_SYM);
    CLEED_TEST_ASSERT(cleed_parse_mode_value("true") == CLEED_MODE_SYM);
    CLEED_TEST_ASSERT(cleed_parse_mode_value("1") == CLEED_MODE_SYM);
    CLEED_TEST_ASSERT(cleed_parse_mode_value("no") == CLEED_MODE_NSYM);
    CLEED_TEST_ASSERT(cleed_parse_mode_value("FALSE") == CLEED_MODE_NSYM);
    CLEED_TEST_ASSERT(cleed_parse_mode_value("0") == CLEED_MODE_NSYM);
    CLEED_TEST_ASSERT(cleed_parse_mode_value("maybe") == CLEED_MODE_INVALID);

    {
        char path[256];
        int has_symmetry = 0;
        CLEED_TEST_ASSERT(write_temp_file("sr: 3 0.0 0.0\n", path, sizeof(path)) == 0);
        CLEED_TEST_ASSERT(cleed_detect_symmetry_file(path, &has_symmetry) == 0);
        CLEED_TEST_ASSERT(has_symmetry == 1);
        cleed_test_remove_file(path);
    }

    {
        char path[256];
        int has_symmetry = 0;
        CLEED_TEST_ASSERT(write_temp_file("sm: 2 0.0 0.0\n", path, sizeof(path)) == 0);
        CLEED_TEST_ASSERT(cleed_detect_symmetry_file(path, &has_symmetry) == 0);
        CLEED_TEST_ASSERT(has_symmetry == 1);
        cleed_test_remove_file(path);
    }

    {
        char path[256];
        int has_symmetry = 1;
        CLEED_TEST_ASSERT(write_temp_file("pb: Ni 0.0 0.0 0.0\n", path, sizeof(path)) == 0);
        CLEED_TEST_ASSERT(cleed_detect_symmetry_file(path, &has_symmetry) == 0);
        CLEED_TEST_ASSERT(has_symmetry == 0);
        cleed_test_remove_file(path);
    }

    return 0;
}
