#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include "rpgmk.h"

const int ARG_WINDOW_WIDTH = 1024;
const int ARG_WINDOW_HEIGHT = 512;
const int ARG_SCALE = 1;

SDL_Window *window = NULL;
SDL_Surface *screenSurface = NULL;
SDL_Event event;

void put_pixel(int x, int y, rgb_t rgb) {
    SDL_Rect rect = {(x)*ARG_SCALE, (y)*ARG_SCALE, ARG_SCALE, ARG_SCALE};
    SDL_FillRect(screenSurface, &rect,
                 SDL_MapRGB(screenSurface->format, rgb.r, rgb.g, rgb.b));
}

void redraw() {
    image_t *chips;

    screenSurface = SDL_GetWindowSurface(window);
    SDL_FillRect(screenSurface, NULL,
                 SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));

    int tile_count = calc_image_count("MAPCHIP.DAT", 16, 16);
    chips = (image_t *)malloc(sizeof(image_t) * tile_count);

    FILE *fp_tile = fopen("MAPCHIP.DAT", "r");
    for (int i = 0; i < tile_count; i++) {
        read_image(fp_tile, &chips[i], 16, 16);
    }
    fclose(fp_tile);

    FILE *fp_patn = fopen("MAPPATN.DAT", "r");
    fseek(fp_patn, 0L, SEEK_END);
    long file_size = ftell(fp_patn);
    fseek(fp_patn, 0L, SEEK_SET);

    int count = file_size / sizeof(int16_t);
    int16_t *patns = (int16_t *)malloc(count * sizeof(int16_t));

    for (int i = 0; i < count; i++) {
        fread(&patns[i], sizeof(int16_t), 1, fp_patn);
    }

    fclose(fp_patn);

    int patn_count = count / 4;

    int x = 0;
    int y = 0;

    int off_x[4] = {0, 16, 0, 16};
    int off_y[4] = {0, 0, 16, 16};

    // patn_count is calculated to 296,
    // but the DANTE PC supports only 237 patterns
    // the other pattern value has gabage values
    patn_count = 237;
    for (int patn = 0; patn < patn_count; patn++) {
        int16_t p[4] = {patns[4 * patn + 0], patns[4 * patn + 1],
                        patns[4 * patn + 2], patns[4 * patn + 3]};

        for (int tile = 0; tile < 4; tile++) {
            for (int i = 0; i < 16; i++) {
                for (int j = 0; j < 16; j++) {
                    put_pixel(
                        x + off_x[tile] + j, y + off_y[tile] + i,
                        index_to_rgb(get_index_image(&chips[p[tile]], i, j)));
                }
            }
        }

        x += 32;
        if (x > ARG_WINDOW_WIDTH) {
            x = 0;
            y += 32;
        }
    }

    for (int i = 0; i < tile_count; i++) {
        free_image(&chips[i]);
    }

    free(chips);
    free(patns);

    SDL_UpdateWindowSurface(window);
}

int main(int argc, char *argv[]) {
    if (read_palette("PALET.DAT") < 0) {
        printf("Error: PALET.DAT not found!\n");
        return 0;
    }

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

    redraw();

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
