/*
 * get_allocated_size.c
 *
 *  Created on: 3 Aug 2014
 *      Author: Liam
 */
#include <stdio.h>
#include <stdlib.h>

#define allocated_size(ptr, type) sizeof(*ptr)/sizeof(type)

int main()
{
	double *t = (double *) malloc(sizeof(double) * 20);
	double a[20];
	if (t != NULL)
		printf("allocated %u\n", sizeof(*a)/sizeof(double));
	return 0;
}
