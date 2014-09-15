#include <stdio.h>
#include "copy_file.h"

int copy_file(char *old_filename, char *new_filename)
{
    FILE  *ptr_old, *ptr_new;
    int  a;
    
    ptr_old = fopen(old_filename, "rb");
    ptr_new = fopen(new_filename, "wb");

    if(ptr_old == NULL) /* failed to open old file */
        return  -1;

    if(ptr_new == NULL) /* failed to open new file */
    {
        fclose(ptr_old);
        return  -1;
    }

    while(1)
    {
        a = fgetc(ptr_old);

        if(!feof(ptr_old))
            fputc(a, ptr_new);
        else
            break;
    }

    fclose(ptr_new);
    fclose(ptr_old);
    return  0;
}