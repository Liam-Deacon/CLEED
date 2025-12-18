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

static int ends_with(const char *s, const char *suffix)
{
    const size_t slen = strlen(s);
    const size_t tlen = strlen(suffix);
    if (slen < tlen) {
        return 0;
    }
    return memcmp(s + slen - tlen, suffix, tlen) == 0;
}

static void replace_suffix(char *path, size_t cap, const char *suffix, const char *replacement)
{
    size_t len = strlen(path);
    size_t slen = strlen(suffix);
    if (len < slen) {
        return;
    }
    if (!ends_with(path, suffix)) {
        return;
    }
    path[len - slen] = '\0';
    (void)snprintf(path + (len - slen), cap - (len - slen), "%s", replacement);
}

static int read_first_po_z(const char *par_path, double *out_z)
{
    FILE *fp = fopen(par_path, "rb");
    if (fp == NULL) {
        return -1;
    }

    char line[512];
    while (fgets(line, (int)sizeof(line), fp) != NULL) {
        if (strncmp(line, "po:", 3) != 0) {
            continue;
        }

        char name[128];
        double x = 0.0, y = 0.0, z = 0.0;
        if (sscanf(line, "po: %127s %lf %lf %lf", name, &x, &y, &z) == 4) {
            fclose(fp);
            *out_z = z;
            return 0;
        }
    }

    fclose(fp);
    return -1;
}

int main(int argc, char **argv)
{
    const char *res_path = arg_value(argc, argv, "-t");
    if (res_path == NULL) {
        fprintf(stderr, "fake_csearch_rfac: missing -t <res>\n");
        return 2;
    }

    char par_path[1024];
    (void)snprintf(par_path, sizeof(par_path), "%s", res_path);
    replace_suffix(par_path, sizeof(par_path), ".res", ".par");

    double z = 0.0;
    if (read_first_po_z(par_path, &z) != 0) {
        fprintf(stderr, "fake_csearch_rfac: failed to read z from %s\n", par_path);
        return 3;
    }

    /*
     * Create a small, smooth objective with a minimum near z=1.2.
     * csearch passes the R factor value to its optimisers (y), and expects
     * the R-factor program stdout to contain at least three numbers:
     *   rfac rr shift
     *
     * sr_evalrf parses: rfac (1st) and shift (3rd), skipping rr (2nd).
     * sr_er parses: rfac (1st) and rr (2nd).
     */
    const double target_z = 1.2;
    const double dz = z - target_z;
    const double rfac = 0.10 + (dz * dz);
    const double rr = 0.25;
    const double shift = 0.0;

    printf("%.6f %.6f %.6f\n", rfac, rr, shift);
    return 0;
}

