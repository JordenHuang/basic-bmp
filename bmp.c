#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "bmp.h"


Bmp_pixel *new_pixel_array(int width, int height, size_t bpp,
                           unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
    int size = width * height;
    int i, j, start;
    Bmp_pixel *pixel_arr = malloc(sizeof(Bmp_pixel) * size);

    // if it's RGB
    if(bpp == 24){
        for(i=0; i<height; ++i){
            start = i * width;
            for(j=0; j<width; ++j){
                pixel_arr[start + j].r = red;
                pixel_arr[start + j].g = green;
                pixel_arr[start + j].b = blue;
                pixel_arr[start + j].a = 255;
            }
        }
    }
    // else if it's RGBA
    else if(bpp == 32){
        for(i=0; i<height; ++i){
            start = i * width;
            for(j=0; j<width; ++j){
                pixel_arr[start + j].r = red;
                pixel_arr[start + j].g = green;
                pixel_arr[start + j].b = blue;
                pixel_arr[start + j].a = alpha;
            }
        }
    }

    return pixel_arr;
}

Bmp_pixel *to_pixel_array(unsigned char *data, int width, int height, size_t bpp)
{
    int size = width * height;
    int i, j, start, data_pos;
    Bmp_pixel *pixel_arr = malloc(sizeof(Bmp_pixel) * size);

    // if it's RGB
    if(bpp == 24){
        for(i=0, data_pos=0; i<height; ++i){
            start = i * width;
            for(j=0; j<width; ++j){
                pixel_arr[start + j].r = data[data_pos++];
                pixel_arr[start + j].g = data[data_pos++];
                pixel_arr[start + j].b = data[data_pos++];
                pixel_arr[start + j].a = 255;
            }
        }
    }
    // else if it's RGBA
    else if(bpp == 32){
        for(i=0, data_pos=0; i<height; ++i){
            start = i * width;
            for(j=0; j<width; ++j){
                pixel_arr[start + j].r = data[data_pos++];
                pixel_arr[start + j].g = data[data_pos++];
                pixel_arr[start + j].b = data[data_pos++];
                pixel_arr[start + j].a = data[data_pos++];
                //printf("a is: %d\n", pixel_arr[start + j].a);
            }
        }
    }

    return pixel_arr;
}

/*
unsigned char ***to_unsigned_char_array(Bmp_pixel *pixels, int width, int height, size_t bpp)
{
    unsigned char ***result = malloc(sizeof(unsigned char**) * height);
    int i, j, k, bytes_per_pixel = (bpp/8);
    for(i=0; i<width; ++i){
        result[i] = malloc(sizeof(unsigned char*) * width);
        for(j=0; j<height; ++j){
        result[i][j] = malloc(sizeof(unsigned char) * bytes_per_pixel);
            // for(k=0; k<width; ++k){
            // }
            result[i][j][0] = pixels[i][j].r;
            result[i][j][1] = pixels[i][j].g;
            result[i][j][2] = pixels[i][j].b;
            if(bytes_per_pixel == 4){
                result[i][j][3] = pixels[i][j].a;
            }
        }
    }
    return result;
}
*/

void bmp_write(const char *file_name, uint8_t bpp, int width, int height, Bmp_pixel *pixels)
{
    FILE *fptr = fopen(file_name, "wb");
    if (fptr == NULL){
        fprintf(stderr, "Can't open file to write [%s]", file_name);
        exit(1);
    }

    int neg_height = 0;
    if(height < 0){
        height = -(height);
        neg_height = 1;
    }

    int image_size = width * height;

    uint32_t offset;
    uint32_t compression_method;
    size_t dib_size;
    if(bpp == 24){
        offset = 14 + 40;
        compression_method = 0;
        dib_size = 40;
    }
    else if(bpp == 32){
        offset = 14 + 56;
        compression_method = 6;
        dib_size = 56;
    }

    // Write header
    unsigned char the_header[] = {
        'B', 'M',
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00,
        0x00, 0x00,
        reverse32(offset, 0), reverse32(offset, 8), reverse32(offset, 16), reverse32(offset, 24)
    };
    fwrite(the_header, sizeof(the_header), 1, fptr);


    // Write DIB
    int temp_height = height;
    if(neg_height) temp_height = -(height);

    unsigned char the_dib[56] = {
        0x28, 0x00, 0x00, 0x00, // 0x28 = 40 in decimal
        reverse32(width, 0), reverse32(width, 8), reverse32(width, 16), reverse32(width, 24),
        reverse32(temp_height, 0), reverse32(temp_height, 8), reverse32(temp_height, 16), reverse32(temp_height, 24),
        0x01, 0x00,
        reverse16(bpp, 0), reverse16(bpp, 8),
        reverse32(compression_method, 0), reverse32(compression_method, 8), reverse32(compression_method, 16), reverse32(compression_method, 24),
        reverse32(image_size, 0), reverse32(image_size, 8), reverse32(image_size, 16), reverse32(image_size, 24),
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };
    if(bpp == 32){
        int i;
        for(i=40; i<56; ++i){
            the_dib[i] = 0x00;
        }
        the_dib[42] = 0xFF;
        the_dib[45] = 0xFF;
        the_dib[48] = 0xFF;
        the_dib[55] = 0xFF;
    }
    fwrite(the_dib, dib_size, 1, fptr);

    // Write pixel data
    int pixel_data_size = (width + (width % 4)) * height * (bpp/8);
    unsigned char the_pixel_data[pixel_data_size];
    int i, j, k, start, cur=0;
    int padding;
    // Positive height stores values from bottom to top, left to right
    if(!neg_height){
        for(i=height-1; i>=0; --i){
            start = i * width;
            for(j=0; j<width; ++j){
                the_pixel_data[cur++] = pixels[start + j].b;
                the_pixel_data[cur++] = pixels[start + j].g;
                the_pixel_data[cur++] = pixels[start + j].r;
                //printf("b: %d\n", pixels[start + j].b);
                //printf("g: %d\n", pixels[start + j].g);
                //printf("r: %d\n", pixels[start + j].r);
                if(bpp == 32){
                    the_pixel_data[cur++] = pixels[start + j].a;
                    //printf("a: %d\n", pixels[start + j].a);
                }
            }
            //padding = 4 - ((bpp / 8) * width) % 4;
            padding = width % 4;
            if(padding > 0 && padding != 4){
                printf("pad\n");
                for(k=0; k<padding; ++k){
                    the_pixel_data[cur++] = 0x00;
                }
            }
        }
    }
    // Negative height stores values from top to bottom, left to right
    else{
        printf("is neg\n");
        for(i=0; i<height; ++i){
            start = i * width;
            for(j=0; j<width; ++j){
                the_pixel_data[cur++] = pixels[start + j].b;
                the_pixel_data[cur++] = pixels[start + j].g;
                the_pixel_data[cur++] = pixels[start + j].r;
                if(bpp == 32){
                    the_pixel_data[cur++] = pixels[start + j].a;
                }
            }
            padding = width % 4;
            if(padding > 0 && padding != 4){
                printf("pad\n");
                for(k=0; k<padding; ++k){
                    the_pixel_data[cur++] = 0x00;
                }
            }
        }
    }
    fwrite(the_pixel_data, pixel_data_size, 1, fptr);

    fclose(fptr);
}

void bmp_free(Bmp_pixel **pixels)
{
    free((*pixels));
    (*pixels) = NULL;
}

