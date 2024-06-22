// #include <stdio.h>

#define BASIC_BMP_IMPLEMENTATION
#include "basic_bmp.h"

#define img_width 256
#define img_height 256
#define bpp 32

int power(int x)
{
    return x * x;
}

int main(void)
{
    unsigned char data[img_height][img_width][bpp/8];

    int i, j, k;
    // bg color
    for(i=0; i<img_height; ++i){
        for(j=0; j<img_width; ++j){
        data[i][j][0] = 255;
        data[i][j][1] = 127;
        data[i][j][2] = 0;
        data[i][j][3] = 255;
        }
    }

    // draw a circle
    int radius = 100;
    int center = img_height/2;
    for(i=0; i<img_height; ++i){
        for(j=0; j<img_width; ++j){
            if ((power(j-center) + power(i-center)) <= power(radius)){
                data[i][j][0] = 0;
                data[i][j][1] = 255;
                data[i][j][2] = 127;
                data[i][j][3] = 255;
            }
        }
    }

    bmp_write("basic_bmp_test.bmp", bpp, img_width, img_height, (void *)(&data));

/*
    void *p = &data;

    int a = 10;
    int arr[2][2] = {{10, 20}, {30, 40}};
    // int *b = arr;
    void *t = arr;

    int (*tarr)[2] = (int (*)[2])t;

    // printf("*b = %d\n", *b);
    // printf("b = %d\n", b);
    printf("arr is: %d\n", **arr);
    printf("tarr is: %d\n", **tarr);

    printf("data[0][0][0] is: %u\n\n", data[0][0][0]);
    printf("data[0][1][0] is: %u\n", *((unsigned char *) p + 3));
*/

    return 0;
}
