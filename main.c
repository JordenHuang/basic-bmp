#include <stdio.h>

#include "bmp.h"

#define img_width 64
#define img_height 64
#define bbp 24

int main()
{
    unsigned char data[img_height][img_width][3];

    int i, j, k;
    for(i=0; i<img_height; ++i){
        for(j=0; j<img_width; ++j){
        data[i][j][0] = i*j;
        data[i][j][1] = i*j;
        data[i][j][2] = i*j;
        //data[i][j][3] = 127;
        }
    }
    data[0][0][0] = 0;
    data[0][0][1] = 0;

    Bmp_pixel *pixels = to_pixel_array(data[0][0], img_width, img_height, bbp);

    //Bmp_pixel *pixels = new_pixel_array(img_width, img_height, 24, 255, 127, 0, 255);

    bmp_write("test_a.bmp", bbp, img_width, img_height, pixels);

    bmp_free(&pixels);

    return 0;
}
