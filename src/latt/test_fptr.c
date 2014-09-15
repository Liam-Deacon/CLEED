/*
 * test_fptr.c
 *
 *  Created on: 3 Aug 2014
 *      Author: Liam
 */

#include <stdio.h>

typedef void (*pftr)();

void print_1()
{
  fprint("printf\n")
}

void print_2(FILE *f)
{
  fprintf(stdout, "fprintf\n")
}

void print(FILE *f)
{

}

int main()
{
  void *print

}
