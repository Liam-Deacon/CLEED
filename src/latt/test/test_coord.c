#include "coord.h"
#include <stdio.h>
#include <malloc.h>

int main()
{
	coord_t *pos = coord_init();
	coord_t *a = malloc(sizeof(coord_t));

	coord_set_x(pos, 1.);
	coord_set_y(pos, 2.);
	coord_set_z(pos, 3.);

	printf("%f %f %f\n",
			coord_get_x(pos),
			coord_get_y(pos),
			coord_get_z(pos)
		  );

	coord_printf(stdout, pos);
	coord_copy(a, pos);

	printf("magnitude: %f\n", coord_get_magnitude(pos));

	printf("allocated: %u\n", coord_get_allocated_size(a));

	coord_free(pos);
	coord_free(a);

	return(0);
}
