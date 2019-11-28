#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>

int ARG_WINDOW_WIDTH = 128;
int ARG_WINDOW_HEIGHT = 128;
int ARG_SCALE = 4;
int PIXEL_WIDTH = 4;

SDL_Window *window = NULL;
SDL_Surface *screenSurface = NULL;
SDL_Event event;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} __attribute__((packed)) pal_t;

pal_t g_pal[16];

#define PUT_PIXEL(x, y, index)                                       \
    do {                                                             \
        SDL_Rect rect = {(x)*ARG_SCALE, (y)*ARG_SCALE, PIXEL_WIDTH,  \
                         PIXEL_WIDTH};                               \
        SDL_FillRect(screenSurface, &rect,                           \
                     SDL_MapRGB(screenSurface->format, g_pal[(index)].r, \
                                g_pal[(index)].g, g_pal[(index)].b));        \
    } while (0)

void redraw(const char *filename) {
    uint8_t chara[8][512] = {
        0,
    };

    uint8_t enemy[3840] = {0, };

    screenSurface = SDL_GetWindowSurface(window);
    SDL_FillRect(screenSurface, NULL,
                 SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));

    FILE *fp = fopen(filename, "r");

    fread(enemy, 1, 3840, fp);

    for (int i = 0; i < 96; i++) {
        for (int j = 0; j < 80; j++) {
            uint8_t dp = enemy[(80 / 2) * i + j];
            PUT_PIXEL(j * 2, i, (dp & 0xf0) >> 4);
            PUT_PIXEL(j * 2 + 1, i, dp & 0x0f);
        }
    }

    fclose(fp);

    SDL_UpdateWindowSurface(window);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        return 0;
    }

    FILE *fp = fopen("PALET.DAT", "r");
    for (int i = 0; i < 16; i++) {
        fread(&g_pal[i], 1, 3, fp);
        g_pal[i].r = (g_pal[i].r << 4) | g_pal[i].r;
        g_pal[i].g = (g_pal[i].g << 4) | g_pal[i].g;
        g_pal[i].b = (g_pal[i].b << 4) | g_pal[i].b;
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
