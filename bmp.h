#ifndef _BMP_H_
#define _BMP_H_

#include <stdint.h>
#include <stddef.h>

#define reverse16(arg, n) (arg >> n)&(0x00FF)
#define reverse32(arg, n) (arg >> n)&(0x000000FF)

typedef struct Bmp_header{
    unsigned char file_type[2];
    uint32_t file_size;
    uint16_t reserved_1;
    uint16_t reserved_2;
    uint32_t offset;
}Bmp_header;

typedef struct Bmp_dib{
    uint32_t header_size;
    int32_t width;
    int32_t height;
    uint16_t num_of_color_planes;
    uint16_t bits_per_pixel;
    uint32_t compression_method;
    uint32_t image_data_size;
    uint32_t horizontal_resolution;
    uint32_t vertical_resolution;
    uint32_t num_of_color_in_palette;
    uint32_t num_of_important_color;
}Bmp_info;

typedef struct Bmp_pixel{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
}Bmp_pixel;


Bmp_pixel *new_pixel_array(
    int width, int height, size_t bpp,
    unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha
);

Bmp_pixel *to_pixel_array(unsigned char *data, int width, int height, size_t bpp);

void bmp_write(const char *filename, uint8_t bpp, int width, int height, Bmp_pixel *pixels);

void bmp_free(Bmp_pixel **pixels);

#endif // _BMP_H_
