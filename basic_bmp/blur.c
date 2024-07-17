#include <stdio.h>

#define BASIC_BMP_IMPLEMENTATION
#include "basic_bmp.h"

int main(void)
{
    int w_read, h_read, channel_read;
    unsigned char *img;
    // img = bmp_load("snail.bmp", &w_read, &h_read, &channel_read);
    img = bmp_load("basic_bmp_test.bmp", &w_read, &h_read, &channel_read);
    printf("w: %d, h: %d, bpp: %d\n", w_read, h_read, channel_read*8);

    int w = w_read;
    int h = h_read;
    int channel = channel_read;
    unsigned char extend_img[w * h * channel];

    int i, j, k, idx;
    int fi, fj, sum;
    int flen = 2;
    for(i=0; i<h; ++i){
        for(j=0; j<w*channel; j+=channel){
            idx = j + i * w * channel;
            for(k=0; k<channel; ++k){
            //     if (k == 4) break;
            //     sum = 0;
            //     for (fi=i-flen; fi<=i+flen; ++fi) {
            //         for (fj=j-flen*channel; fj<=j+flen*channel; fj+=channel) {
            //             if (fi>-1 && fi<h && fj>-1 && fj<w*channel) {
            //                 sum += img[(fj + fi * w * channel) + k];
            //             }
            //         }
            //     }
            //
            //     extend_img[idx + k] = sum / 9;
                extend_img[idx + k] = img[idx + k];
            }
        }
    }

    // Write to a file
    int r;
    const char *filename = "snail_blur.bmp";
    r = bmp_write(filename, w, h, channel, (extend_img));
    if (r != 0) {
        printf("[ERROR] Some error happened\n");
    } else {
        printf("Image [%s] created successfully\n", filename);
    }

    free(img);

    return 0;
}
