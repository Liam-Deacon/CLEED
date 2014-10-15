#include "patt.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
  pattern *pat = (pattern*) &pattern_default;
  pattern_printf(stdout, pat);
  return 0;
}