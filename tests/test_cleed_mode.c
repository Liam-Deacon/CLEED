#include "cleed_mode.h"
#include "test_support.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define CLEED_TEST_PATH_SIZE 512

static int write_temp_file(const char *contents, char *path, size_t path_size)
{
#ifdef _WIN32
    char temp_path[MAX_PATH];
    char temp_file[MAX_PATH];
    DWORD path_len = GetTempPathA(sizeof(temp_path), temp_path);
    if (path_len == 0 || path_len >= sizeof(temp_path)) {
        return -1;
    }
    if (GetTempFileNameA(temp_path, "cld", 0, temp_file) == 0) {
        return -1;
    }
    if (strlen(temp_file) + 1 > path_size) {
        DeleteFileA(temp_file);
        return -1;
    }
    strcpy(path, temp_file);
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

static int cleed_assert_symmetry_from_contents(const char *contents,
                                               int expected_has_symmetry)
{
    char path[CLEED_TEST_PATH_SIZE];
    int has_symmetry = 1;

    if (write_temp_file(contents, path, sizeof(path)) != 0) {
        return 1;
    }
    if (cleed_detect_symmetry_file(path, &has_symmetry) != 0) {
        cleed_test_remove_file(path);
        return 1;
    }
    if (has_symmetry != expected_has_symmetry) {
        cleed_test_remove_file(path);
        return 1;
    }
    cleed_test_remove_file(path);
    return 0;
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
        int has_symmetry = 1;
        CLEED_TEST_ASSERT(cleed_detect_symmetry_file(NULL, &has_symmetry) == -1);
        CLEED_TEST_ASSERT(has_symmetry == 0);
    }

    {
        int has_symmetry = 1;
        CLEED_TEST_ASSERT(cleed_detect_symmetry_file("cleed_mode_missing_file.tmp",
                                                     &has_symmetry) == -1);
        CLEED_TEST_ASSERT(has_symmetry == 0);
    }

    CLEED_TEST_ASSERT(cleed_assert_symmetry_from_contents("sr: 3 0.0 0.0\n", 1) == 0);
    CLEED_TEST_ASSERT(cleed_assert_symmetry_from_contents("sm: 2 0.0 0.0\n", 1) == 0);
    CLEED_TEST_ASSERT(cleed_assert_symmetry_from_contents("pb: Ni 0.0 0.0 0.0\n", 0) == 0);
    CLEED_TEST_ASSERT(cleed_assert_symmetry_from_contents("  \tSR: 4 0.0 0.0\n", 1) == 0);
    CLEED_TEST_ASSERT(cleed_assert_symmetry_from_contents("# sr: 4 2/m\n! sm: p4mm\n; SR: 2/m\n",
                                                          0) == 0);
    CLEED_TEST_ASSERT(cleed_assert_symmetry_from_contents("# comment line\nsr: 3 0.0 0.0\n", 1) == 0);
    CLEED_TEST_ASSERT(cleed_assert_symmetry_from_contents("sx: 1 2 3\n", 0) == 0);
    CLEED_TEST_ASSERT(cleed_assert_symmetry_from_contents("", 0) == 0);

    {
        char path[CLEED_TEST_PATH_SIZE];
        CLEED_TEST_ASSERT(write_temp_file("Sm: 2 0.0 0.0\n", path, sizeof(path)) == 0);
        CLEED_TEST_ASSERT(cleed_detect_symmetry_file(path, NULL) == 0);
        cleed_test_remove_file(path);
    }

    return 0;
}
