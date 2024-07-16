#ifndef _BASIC_BMP_H_
#define _BASIC_BMP_H_

#include <stdint.h>
#include <stddef.h>

#define reverse16(arg, n) (unsigned char)(((arg) >> (n))& 0x00FF)
#define reverse32(arg, n) (unsigned char)(((arg) >> (n))& 0x000000FF)
#define reverse16_as_int(num, arr, idx_begin) ({    \
    (num) = (arr)[(idx_begin)+1];                   \
    (num) = ((num) << 8) + (arr)[(idx_begin)];      \
    (num);                                          \
})
#define reverse32_as_int(num, arr, idx_begin) ({    \
    (num) = (arr)[(idx_begin)+3];                   \
    (num) = ((num) << 8) + (arr)[(idx_begin)+2];    \
    (num) = ((num) << 8) + (arr)[(idx_begin)+1];    \
    (num) = ((num) << 8) + (arr)[(idx_begin)];      \
    (num);                                          \
})

#define Red 0
#define Green 1
#define Blue 2
#define Alpha 3


// bpp: bits per pixel
int bmp_write(const char *filename, int w, int h, int bpp, void *data);
unsigned char *bmp_load(const char *filename, int *w, int *h, int *bpp);
void bmp_free(void *data);

#endif  // _BASIC_BMP_H_


// #define BASIC_BMP_IMPLEMENTATION
#ifdef BASIC_BMP_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>

int bmp_write(const char *filename, int w, int h, int bpp, void *data){
    FILE *fptr = fopen(filename, "wb");
    if (fptr == NULL){
        fprintf(stderr, "Can't open file to write [%s]", filename);
        return 1;
    }

    int pixel_data_size = (w + (w % 4)) * h * (bpp/8);
    int image_size = pixel_data_size;

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
    int file_size = offset + image_size;

    // Write header
    unsigned char the_header[] = {
        'B', 'M',
        reverse32(file_size, 0), reverse32(file_size, 8), reverse32(file_size, 16), reverse32(file_size, 24),
        0x00, 0x00,
        0x00, 0x00,
        reverse32(offset, 0), reverse32(offset, 8), reverse32(offset, 16), reverse32(offset, 24)
    };
    fwrite(the_header, sizeof(the_header), 1, fptr);


    // Write DIB
    unsigned char the_dib[56] = {
        0x28, 0x00, 0x00, 0x00, // 0x28 = 40 in decimal
        reverse32(w, 0), reverse32(w, 8), reverse32(w, 16), reverse32(w, 24),
        reverse32(h, 0), reverse32(h, 8), reverse32(h, 16), reverse32(h, 24),
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
    unsigned char the_pixel_data[pixel_data_size];

    int i, j, k, start, cur=0;
    int padding;

    int bytes_per_pixel = bpp / 8;
    int j_bound;
    unsigned char *start_of_pixel;
    // Positive height stores values from bottom to top, left to right
    for(i=h-1; i>=0; --i){
        j_bound = w * bytes_per_pixel;
        start = i * j_bound;  // start = i * w * bytes_per_pixel
        for(j=0; j<j_bound; j+=bytes_per_pixel){
            start_of_pixel = (unsigned char *)data + (start+j);
            the_pixel_data[cur++] = *(start_of_pixel + Blue );
            the_pixel_data[cur++] = *(start_of_pixel + Green);
            the_pixel_data[cur++] = *(start_of_pixel + Red  );
            // printf("B: %d\n", *((unsigned char *)data + (start+j)+Blue));
            // printf("G: %d\n", *((unsigned char *)data + (start+j)+Green));
            // printf("R: %d\n", *((unsigned char *)data + (start+j)+Red));
            if(bpp == 32){
                the_pixel_data[cur++] = *(start_of_pixel + Alpha);
            }
        }
        //padding = 4 - ((bpp / 8) * w) % 4;
        padding = w % 4;
        if(padding > 0 && padding != 4){
            // printf("pad\n");
            for(k=0; k<padding; ++k){
                the_pixel_data[cur++] = 0x00;
            }
        }
    }
    fwrite(the_pixel_data, pixel_data_size, 1, fptr);

    fclose(fptr);
    return 0;
}

unsigned char *bmp_load(const char *filename, int *w, int *h, int *bpp)
{
    FILE *fptr = fopen(filename, "rb");
    if (fptr == NULL){
        fprintf(stderr, "Can't open file to read [%s]", filename);
        return NULL;
    }

    // Check file type, and get info from header
    unsigned char header[14];
    fread(header, 14, 1, fptr);
    if (header[0] != 'B' || header[1] != 'M') {
        fprintf(stderr, "File format is not bmp (bitmap file)");
        return NULL;
    }
    uint32_t file_size;
    reverse32_as_int(file_size, header, 2);


    // uint32_t offset = *(int *)(&header[10]);
    uint32_t offset;
    reverse32_as_int(offset, header, 10);

    // Get info from dib header
    unsigned char dib[offset-14];
    fread(dib, (offset-14), 1, fptr);

    reverse32_as_int(*w, dib, 4);
    reverse32_as_int(*h, dib, 8);
    reverse16_as_int(*bpp, dib, 14);

    // int raw;
    // reverse32_as_int(raw, dib, 20);
    // printf("%u, %u, %u, %u, %u\n", file_size, offset, *w, *h, *bpp);

    int pixel_data_size = ((*w) + ((*w) % 4)) * (*h) * ((*bpp)/8);
    // printf("size: %d, raw: %d\n", pixel_data_size, raw);

    // Proccessing the pixel data
    unsigned char the_pixel_data[pixel_data_size];
    unsigned char *final_data = malloc(sizeof(unsigned char) * ((*w)*(*h)*(*bpp)));
    fread(the_pixel_data, pixel_data_size, 1, fptr);

    int i, j, k, start, cur=0;
    int padding;

    int bytes_per_pixel = (*bpp) / 8;
    int j_bound;
    unsigned char *start_of_pixel;
    // Positive height stores values from bottom to top, left to right
    for(i=(*h)-1; i>=0; --i){
        j_bound = (*w) * bytes_per_pixel;
        start = i * j_bound;  // start = i * w * bytes_per_pixel
        for(j=0; j<j_bound; j+=bytes_per_pixel){
            start_of_pixel = final_data + (start+j);
            *(start_of_pixel + Blue ) = the_pixel_data[cur++];
            *(start_of_pixel + Green) = the_pixel_data[cur++];
            *(start_of_pixel + Red  ) = the_pixel_data[cur++];
            if((*bpp) == 32){
                *(start_of_pixel + Alpha) = the_pixel_data[cur++];
            }
        }
    }


    return final_data;
}

void bmp_free(void *data)
{
    free(data);
}

#endif  // BASIC_BMP_IMPLEMENTATION
