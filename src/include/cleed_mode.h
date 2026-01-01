/*********************************************************************
  Shared helpers for selecting CLEED symmetry mode.
*********************************************************************/

#ifndef CLEED_MODE_H
#define CLEED_MODE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  CLEED_MODE_AUTO = 0,
  CLEED_MODE_SYM,
  CLEED_MODE_NSYM,
  CLEED_MODE_INVALID
} cleed_mode_t;

cleed_mode_t cleed_parse_mode_value(const char *value);
int cleed_detect_symmetry_file(const char *path, int *has_symmetry);

#ifdef __cplusplus
}
#endif

#endif /* CLEED_MODE_H */
