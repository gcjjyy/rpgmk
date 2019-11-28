#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>

int ARG_WINDOW_WIDTH = 128;
int ARG_WINDOW_HEIGHT = 128;
int ARG_SCALE = 3;

const int IMAGE_WIDTH = 80;
const int IMAGE_HEIGHT = 96;
const int IMAGE_HALF_WIDTH = (IMAGE_WIDTH >> 1);
const int IMAGE_SIZE = IMAGE_HALF_WIDTH * IMAGE_HEIGHT;
const int FRAME_WIDTH = (IMAGE_WIDTH >> 2);
const int FRAME_HEIGHT = (IMAGE_HEIGHT >> 2);

SDL_Window *window = NULL;
SDL_Surface *screenSurface = NULL;
SDL_Event event;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} __attribute__((packed)) pal_t;

pal_t g_pal[16];

void put_pixel(int x, int y, int index) {
    SDL_Rect rect = {(x)*ARG_SCALE, (y)*ARG_SCALE, ARG_SCALE,
                     ARG_SCALE};
    SDL_FillRect(screenSurface, &rect,
                 SDL_MapRGB(screenSurface->format, g_pal[(index)].r,
                            g_pal[(index)].g, g_pal[(index)].b));
}

void redraw(const char *filename) {
    uint8_t enemy[IMAGE_SIZE];
    uint8_t index_map[IMAGE_HEIGHT][IMAGE_WIDTH];

    screenSurface = SDL_GetWindowSurface(window);
    SDL_FillRect(screenSurface, NULL,
                 SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));

    FILE *fp = fopen(filename, "r");
    fread(enemy, 1, IMAGE_SIZE, fp);
    fclose(fp);

    for (int i = 0; i < IMAGE_HEIGHT; i++) {
        for (int j = 0; j < IMAGE_HALF_WIDTH; j++) {
            uint8_t dp = enemy[IMAGE_HALF_WIDTH * i + j];
            index_map[i][j * 2 + 0] = (dp & 0xf0) >> 4;
            index_map[i][j * 2 + 1] = (dp & 0x0f) >> 0;
        }
    }

    for (int i = 0; i < FRAME_HEIGHT; i++) {
        for (int j = 0; j < FRAME_WIDTH; j++) {
            uint8_t data[4][4];
            for (int r = 0; r < 4; r++) {
                for (int c = 0; c < 4; c++) {
                    data[r][c] = index_map[FRAME_HEIGHT * r + i][FRAME_WIDTH * c + j];
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
                    put_pixel(4 * j + c, 4 * i + r, data_tp[r][c]);
                }
            }
        }
    }

    SDL_UpdateWindowSurface(window);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        return 0;
    }

    FILE *fp = fopen("PALET2.DAT", "r");
    for (int i = 0; i < 16; i++) {
        fread(&g_pal[i], 1, 3, fp);
        g_pal[i].r = g_pal[i].r << 4;
        g_pal[i].g = g_pal[i].g << 4;
        g_pal[i].b = g_pal[i].b << 4;
        printf("Palette[%02d]: %02x%02x%02x\n", i, g_pal[i].r, g_pal[i].g, g_pal[i].b);
    }
    fclose(fp);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL_Init error: %s\n", SDL_GetError());
        return 0;
    }

    window =
        SDL_CreateWindow(argv[0], SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, ARG_WINDOW_WIDTH * ARG_SCALE,
                         ARG_WINDOW_HEIGHT * ARG_SCALE, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("SDL_CreateWindow error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(0);
    }

    redraw(argv[1]);

    while (SDL_WaitEvent(&event) >= 0) {
        switch (event.type) {
            case SDL_KEYDOWN: {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        SDL_DestroyWindow(window);
                        SDL_Quit();
                        return 0;
                        break;
                }
            } break;

            case SDL_QUIT: {
                SDL_DestroyWindow(window);
                SDL_Quit();
                return 0;
            } break;
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
