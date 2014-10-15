#include "patt.h"
#include "pattern.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
  size_t i;
  pattern *pat = pattern_init(0);
  spots *spots;
  if (pat != NULL)
  {
    pattern_read(pat, stdin);
    pattern_printf(stdout, pat);
    spots = pattern_calculate_substrate_spots(pat);
    printf("# spots = %i\n", spots->n_spots);
    for (i = 0; i < spots->n_spots; i++)
      printf("spot %i: (%f, %f)\n", i, spots->spots[i].index.h, spots->spots[i].index.k);
  }
  return 0;
}