#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "rpgmk.h"

static rgb_t g_palette[16];

int read_palette(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        return (-1);
    }

    for (int i = 0; i < 16; i++) {
        fread(&g_palette[i], 1, 3, fp);
        g_palette[i].r = g_palette[i].r << 4;
        g_palette[i].g = g_palette[i].g << 4;
        g_palette[i].b = g_palette[i].b << 4;
    }
    fclose(fp);

    return (0);
}

rgb_t index_to_rgb(int index) {
    if (index >= 0 && index < 16) {
        return g_palette[index];
    }

    return (rgb_t){ 0, 0, 0 };
}

int calc_image_count(const char *filename, int width, int height) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        return (0);
    }

    fseek(fp, 0L, SEEK_END);
    long file_size = ftell(fp);
    fclose(fp);

    int mod = file_size % (width * height / 2);
    if (mod != 0) {
        return (0);
    }

    return file_size / (width * height / 2);
}

int read_image(FILE *fp, image_t *image, int width, int height) {
    if (!image) {
        return (-1);
    }

    int half_width = width / 2;
    int raw_data_size = half_width * height;
    uint8_t *raw = (uint8_t *)malloc(raw_data_size);
    uint8_t *converted = (uint8_t *)malloc(width * height);

    image->width = width;
    image->height = height;
    image->buf = (uint8_t *)malloc(width * height);

    fread(raw, raw_data_size, 1, fp);

    // convert 4-bpp raw data to 8-bpp index image
    // the image has 4x4 grid with separated images that need to merged as one
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < half_width; j++) {
            uint8_t data = raw[half_width * i + j];
            converted[(i * width) + (j * 2 + 0)] = (data & 0xf0) >> 4;
            converted[(i * width) + (j * 2 + 1)] = (data & 0x0f) >> 0;
        }
    }

    int grid_width = width / 4;
    int grid_height = height / 4;

    for (int i = 0; i < grid_height; i++) {
        for (int j = 0; j < grid_width; j++) {
            uint8_t data[4][4];
            for (int r = 0; r < 4; r++) {
                for (int c = 0; c < 4; c++) {
                    data[r][c] = converted[(width * (grid_height * r + i)) + (grid_width * c + j)];
                }
            }

            uint8_t data_tp[4][4];
            for (int m = 0; m < 4; m++) {
                data_tp[m][0] =
                    ((data[0][m] & 0b1000) >> 3) |
                    ((data[1][m] & 0b1000) >> 2) |
                    ((data[2][m] & 0b1000) >> 1) |
                    ((data[3][m] & 0b1000) >> 0);

                data_tp[m][1] =
                    ((data[0][m] & 0b0100) >> 2) |
                    ((data[1][m] & 0b0100) >> 1) |
                    ((data[2][m] & 0b0100) >> 0) |
                    ((data[3][m] & 0b0100) << 1);

                data_tp[m][2] =
                    ((data[0][m] & 0b0010) >> 1) |
                    ((data[1][m] & 0b0010) >> 0) |
                    ((data[2][m] & 0b0010) << 1) |
                    ((data[3][m] & 0b0010) << 2);

                data_tp[m][3] =
                    ((data[0][m] & 0b0001) >> 0) |
                    ((data[1][m] & 0b0001) << 1) |
                    ((data[2][m] & 0b0001) << 2) |
                    ((data[3][m] & 0b0001) << 3);
            }

            for (int r = 0; r < 4; r++) {
                for (int c = 0; c < 4; c++) {
                    image->buf[(4 * i + r) * width + (4 * j + c)] = data_tp[r][c];
                }
            }
        }
    }

    free(converted);
    free(raw);

    return (0);
}

void free_image(image_t *image) {
    if (image && image->buf) {
        free(image->buf);
        image->buf = NULL;
    }
}

int get_index_image(image_t *image, int row, int col) {
    if (!image) {
        return (0);
    }

    return image->buf[image->width * row + col];
}
