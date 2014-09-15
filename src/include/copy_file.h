
#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef COPY_FILE_H
#define COPY_FILE_H

int copy_file(char *old_filename, char  *new_filename);

#endif /* COPY_FILE_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif