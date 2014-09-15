/*********************************************************************
LD/24.04.14
  header for functions:

  usage (24.04.14)
     Provides syntax help and other information before exiting

  info (24.04.14)
     Provides version information then exits
  
Changes:

*********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef PROG_HELP_H

#include <stdio.h>
#include <stdlib.h>

/* function prototypes */
void usage(FILE *output);
void usage_sym(FILE *output);
void info();
void info_sym();

#endif /* PROG_HELP_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif