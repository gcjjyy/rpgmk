#ifndef __RPGMK_H__
#define __RPGMK_H__

#include <stdint.h>

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} __attribute__((packed)) rgb_t;

typedef struct {
    int width;
    int height;
    uint8_t *buf;
} __attribute__((packed)) image_t;

int read_palette(const char *filename);
rgb_t index_to_rgb(int index);
int calc_image_count(const char *filename, int width, int height);
int read_image(FILE *fp, image_t *image, int width, int height);
void free_image(image_t *image);
int get_index_image(image_t *image, int row, int col);

#endif
