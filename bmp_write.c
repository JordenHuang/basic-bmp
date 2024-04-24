#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#define image_width 64
#define image_height 64
// file type: BM
#define FILE_TYPE 0x4d42

typedef struct Bmp_header{
    uint16_t file_type;
    uint32_t file_size;
    uint16_t reserved_1;
    uint16_t reserved_2;
    uint32_t offset;
}Bmp_header;

typedef struct Bmp_info{
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
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
}Bmp_pixel;

typedef struct Bmp{
    Bmp_header *header;
    Bmp_info *info;
    Bmp_pixel *data;
}Bmp;

/*
 * Create a pixel array, called buf, or conver a size_t array, called data, to pixel array
 * */
void bmp_pixel_array(Bmp_pixel **buf, const size_t *data, size_t img_width, size_t img_height, 
                     uint8_t init_red, uint8_t init_blue, uint8_t init_green, uint8_t init_alaph)
{
    size_t i;
    size_t size = img_width * img_height;
    (*buf) = malloc(sizeof(Bmp_pixel *) * size);
    if(data == NULL){
        for(i=0; i<size; ++i){
            (*buf)[i].red = init_red;
            (*buf)[i].blue = init_blue;
            (*buf)[i].green = init_green;
            (*buf)[i].alpha = init_alaph;
        }
    }
    else{
        (*buf)[i].red = data[i];
        (*buf)[i].blue = data[i+1];
        (*buf)[i].green = data[i+2];
        (*buf)[i].alpha = data[i+3];
    }
}


void bmp_create_header(Bmp **bmp );

void bmp_write(Bmp **bmp, Bmp_pixel *data );

void bmp_free(Bmp **bmp, size_t width, size_t height)
{
    size_t i;
    size_t size = width * height;
    free((*bmp)->data);
}

int main()
{
    Bmp *bmp = NULL;
    Bmp_pixel *data_buf = NULL;
    // Create a pixel array, with each black represents black
    bmp_pixel_array(&data_buf, NULL, image_width, image_height, 0, 0, 0, 1);
    size_t i;
#if (0)
    for(i=0; i<image_width*image_height; ++i){
        printf("red: %d ", data_buf[i].red);
        printf("green: %d ", data_buf[i].green);
        printf("blue: %d ", data_buf[i].blue);
        printf("alpha: %d", data_buf[i].alpha);
        printf("\n");
    }
#endif


    // bmp_write(&bmp, data);


    free(data_buf);
    return 0;
}

/*
 * image data is type bmp_pixel
 *
 *
 * */
