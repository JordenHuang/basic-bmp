#include <stdio.h>

#define BASIC_BMP_IMPLEMENTATION
#include "basic_bmp.h"

#define img_width 256
#define img_height 256
#define channel 4
#define bpp (channel*8)

int power(int x)
{
    return x * x;
}

int main(void)
{
    int i, j, k;
    int idx;
#if 1
    unsigned char data[img_height][img_width][channel];

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

    int r = bmp_write("basic_bmp_test.bmp", img_width, img_height, channel, (void *)(data));
    if (r != 0) {
        printf("[ERROR] Some error happened\n");
    } else {
        printf("[INFO] Image created successfully\n");
    }
#endif

#if 0
    int w_read, h_read, channel_read;
    unsigned char *pixels;
    // printf("--------------------\n");
    // pixels = bmp_load("snail.bmp", &w_read, &h_read, &channel_read);
    // free(pixels);

    // printf("--------------------\n");
    pixels = bmp_load("basic_bmp_test.bmp", &w_read, &h_read, &channel_read);

    for(i=0; i<h_read; ++i){
        for(j=0; j<w_read*channel_read; j+=channel_read){
            idx = j + i*img_width*channel_read;
            k = pixels[idx+0];
            pixels[idx+0] = pixels[idx+1];
            pixels[idx+1] = pixels[idx+2];
            pixels[idx+2] = k;
        }
    }
    int rr = bmp_write("basic_bmp_test_rewrite.bmp", w_read, h_read, channel_read, (void *)(pixels));
    if (rr != 0) {
        printf("[ERROR] Some error happened\n");
    } else {
        printf("[INFO] Image created successfully\n");
    }

    free(pixels);
#endif

    return 0;
}
/* The below is my learning history
    void *p = &data;
    printf("Hello\n");
    printf("data[0][0][0] is: %u\n\n", ((unsigned char ***)p)[0][0][0]);

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
