#include "patt.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
  pattern_t *pat = (pattern_t*) &pattern_default;
  pattern_printf(stdout, pat);
  return 0;
}