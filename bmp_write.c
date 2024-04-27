#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
// #include <assert.h>

#define image_width 512
#define image_height 512

#define reverse16(arg, n) (arg >> n)&(0x00FF)
#define reverse32(arg, n) (arg >> n)&(0x000000FF)


typedef struct Bmp_header{
    unsigned char file_type[2];
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
 * Create a pixel array, called buf, or convert a size_t array, called data, to pixel array
 * */
void bmp_pixel_array(Bmp_pixel **buf, const size_t *data, size_t img_width, size_t img_height, 
                     uint8_t init_red, uint8_t init_green, uint8_t init_blue, uint8_t init_alaph)
{
    size_t i;
    size_t size = img_width * img_height;
    (*buf) = malloc(sizeof(Bmp_pixel) * size);
    if(data == NULL){
        for(i=0; i<size; ++i){
            (*buf)[i].red = init_red;
            (*buf)[i].green = init_green;
            (*buf)[i].blue = init_blue;
            (*buf)[i].alpha = init_alaph;
        }
    }
    // else{
    //     (*buf)[i].red = data[i];
    //     (*buf)[i].green = data[i+1];
    //     (*buf)[i].blue = data[i+2];
    //     (*buf)[i].alpha = data[i+3];
    // }
}


Bmp_header *bmp_create_header(const char *file_type, size_t width, size_t height)
{
    Bmp_header *header = malloc(sizeof(Bmp_header));
    strcpy(header->file_type, file_type);
    header->file_size = 0x00000000; //14 + 40 + width * height;
    header->reserved_1 = 0x0000;
    header->reserved_2 = 0x0000;
    header->offset = 14 + 40;

    return header;
}

Bmp_info *bmp_create_info(size_t bits_per_pixel, size_t width, size_t height)
{
    Bmp_info *info = malloc(sizeof(Bmp_info));
    info->header_size = 40;
    info->width = width;
    info->height = height;
    info->num_of_color_planes = 0x0001;
    info->bits_per_pixel = bits_per_pixel;
    info->compression_method = 6; //0;  // try with alpha
    info->image_data_size = 0;
    info->horizontal_resolution = 0;
    info->vertical_resolution = 0;
    info->num_of_color_in_palette = 0;
    info->num_of_important_color = 0;

    return info;
}

FILE *bmp_write(const char *file_name, Bmp bmp, Bmp_pixel *data)
{
    printf("file name: %s\n", file_name);
    FILE *fptr = fopen(file_name, "wb");
    if (fptr == NULL){
        fprintf(stderr, "Can't open file [%s]", file_name);
        exit(1);
    }
    // Write header
    Bmp_header *header = bmp.header;
    unsigned char the_header[] = {
        header->file_type[0], header->file_type[1],
        header->file_size & 0x000000FF, (header->file_size >> 8) & 0x000000FF,(header->file_size >> 16) & 0x000000FF,(header->file_size >> 24) & 0x000000FF,
        header->reserved_1 & 0x00FF, (header->reserved_1 >> 8) & 0x00FF,
        header->reserved_2 & 0x00FF, (header->reserved_2 >> 8) & 0x00FF,
        header->offset & 0x000000FF, (header->offset >> 8) & 0x000000FF,(header->offset >> 16) & 0x000000FF,(header->offset >> 24) & 0x000000FF,
    };
    fwrite(the_header, sizeof(the_header), 1, fptr);

    // Write info
    Bmp_info *info = bmp.info;
    unsigned char the_info[] = {
        reverse32(info->header_size, 0), reverse32(info->header_size, 8), reverse32(info->header_size, 16), reverse32(info->header_size, 24),
        reverse32(info->width, 0), reverse32(info->width, 8), reverse32(info->width, 16), reverse32(info->width, 24),
        reverse32(info->height, 0), reverse32(info->height, 8), reverse32(info->height, 16), reverse32(info->height, 24),
        reverse16(info->num_of_color_planes, 0), reverse16(info->num_of_color_planes, 8),
        reverse16(info->bits_per_pixel, 0), reverse16(info->bits_per_pixel, 8),
        reverse32(info->compression_method, 0), reverse32(info->compression_method, 8), reverse32(info->compression_method, 16), reverse32(info->compression_method, 24),
        reverse32(info->image_data_size, 0), reverse32(info->image_data_size, 8), reverse32(info->image_data_size, 16), reverse32(info->image_data_size, 24),
        reverse32(info->horizontal_resolution, 0), reverse32(info->horizontal_resolution, 8), reverse32(info->horizontal_resolution, 16), reverse32(info->horizontal_resolution, 24),
        reverse32(info->vertical_resolution, 0), reverse32(info->vertical_resolution, 8), reverse32(info->vertical_resolution, 16), reverse32(info->vertical_resolution, 24),
        reverse32(info->num_of_color_in_palette, 0), reverse32(info->num_of_color_in_palette, 8), reverse32(info->num_of_color_in_palette, 16), reverse32(info->num_of_color_in_palette, 24),
        reverse32(info->num_of_important_color, 0), reverse32(info->num_of_important_color, 8), reverse32(info->num_of_important_color, 16), reverse32(info->num_of_important_color, 24),
        // try to write alpha mask
        // IMPORTANT: needs below when bpp is 32bit and compression_method set to 6
        0x00, 0x00, 0xFF, 0x00,
        0x00, 0xFF, 0x00, 0x00,
        0xFF, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xFF,
    };
    fwrite(the_info, sizeof(the_info), 1, fptr);

    int i = info->height - 1;
    int j, padding, start, cur;
    uint8_t x;
    unsigned char pad[1] = {0x00}, image_data[info->height * info->width * (info->bits_per_pixel / 8)];
    for(cur=0; i>=0; --i){
        start = i * (info->width);
        for(j=0; j<info->width; ++j){
            image_data[cur++] = data[start + j ].blue;
            image_data[cur++] = data[start + j ].green;
            image_data[cur++] = data[start + j ].red;
            if(info->bits_per_pixel == 32) image_data[cur++] = data[start + j ].alpha;
            //printf("position: %d ",   (start + j));
            //printf("cur: %d \n",   (cur));
            //printf("blue: %x ",   (data[start + j].blue));
            //printf("green: %x ",  (data[start + j].green));
            //printf("red: %x ",    (data[start + j].red));
            //printf("alpha: %x\n", (data[start + j].alpha));
        }
        padding = 4 - ((info->bits_per_pixel / 8) * info->width) % 4;
        //printf("padding: %d\n", padding);
        if(padding > 0 && padding != 4){
            int c;
            printf("in\n");
            for(c=0; c<padding; ++c){
                // fwrite(pad, padding, 1, fptr);
                image_data[cur++] = 0x00;
            }
        }
    }
    fwrite(image_data, info->height * info->width * (info->bits_per_pixel / 8), 1, fptr);

    return fptr;
}

void bmp_free(Bmp *bmp, Bmp_pixel *data)
{
    size_t i;
    // size_t size = width * height;
    free((bmp)->header);
    free((bmp)->info);
    free(data);
}

int main()
{
    Bmp bmp;

    // Create the bmp file header
    bmp.header = bmp_create_header("BM", image_width, image_height);
#if(0)
    printf("header:\n");
    printf("file_type : %s\n", bmp.header->file_type);
    printf("file_size : %d\n", bmp.header->file_size);
    printf("reserved_1: %d\n", bmp.header->reserved_1);
    printf("reserved_2: %d\n", bmp.header->reserved_2);
    printf("offset    : %d\n", bmp.header->offset);
#endif

    bmp.info = bmp_create_info(32, image_width, image_height);
#if(0)
    printf("info:\n");
    printf("header_size            : %d\n", bmp.info->header_size);
    printf("width                  : %d\n", bmp.info->width);
    printf("height                 : %d\n", bmp.info->height);
    printf("num_of_color_planes    : %d\n", bmp.info->num_of_color_planes);
    printf("bits_per_pixel         : %d\n", bmp.info->bits_per_pixel);
    printf("compression_method     : %d\n", bmp.info->compression_method);
    printf("image_data_size        : %d\n", bmp.info->image_data_size);
    printf("horizontal_resolution  : %d\n", bmp.info->horizontal_resolution);
    printf("vertical_resolution    : %d\n", bmp.info->vertical_resolution);
    printf("num_of_color_in_palette: %d\n", bmp.info->num_of_color_in_palette);
    printf("num_of_important_color : %d\n", bmp.info->num_of_important_color);
#endif

    Bmp_pixel *data_buf = NULL;
    // Create a pixel array, with each black represents black
    bmp_pixel_array(&data_buf, NULL, image_width, image_height, 255, 0, 0, 255);
    size_t i, j;
    for(i=256; i<512; ++i){
        for(j=0; j<512; ++j){
            data_buf[i*512+j].red = 255;
            data_buf[i*512+j].green = 0;
            data_buf[i*512+j].blue = 0;
            data_buf[i*512+j].alpha = (255-i)%256;
        }
    }
#if (0)
    for(i=0; i<image_width*image_height; ++i){
        printf("red: %d ", data_buf[i].red);
        printf("green: %d ", data_buf[i].green);
        printf("blue: %d ", data_buf[i].blue);
        printf("alpha: %d", data_buf[i].alpha);
        printf("\n");
    }
#endif


    FILE *fptr;
    char file_name[] = "my_bmp.bmp";
    fptr = bmp_write(file_name, bmp, data_buf);
    fclose(fptr);


    // free(data_buf);
    bmp_free(&bmp, data_buf);
    return 0;
}

/*
 * image data is type bmp_pixel
 *
 * TODO: add functions:
 *      1. create_header_and_DIB()
 *      2. bmp_custom_write(const char *filename, Bmp_header, Bmp_info, Bmp_pixel **pixels)
 *      3. pixel2uchar_array
 *
 * */
