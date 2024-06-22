#include <stdio.h>
#include <stdlib.h>


Bmp_pixel *bmp_read(const char *filename)
{
    FILE *fptr = fopen(file_name, "rb");
    if (fptr == NULL){
        fprintf(stderr, "Can't open file to read [%s]", file_name);
        exit(1);
    }
}
