#include <iostream>
#include <SDL2/SDL.h>
#include <zconf.h>

#include "chip8.h"

const std::string romPath = "/Users/alexeygolubev/Desktop/Pong (alt).ch8";

int main(int argc, char *argv[]) {
    Chip8 chip8;

    if(chip8.loadROM(romPath))
    {
        std::cout << "ROM loaded successfully" << std::endl;
    } else
    {
        std::cout << "There is an error during ROM loading" << std::endl;
        exit(1);
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::cerr << "Error in initialising SDL " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    SDL_Window* window = SDL_CreateWindow("CHIP-8",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          DISPLAY_WIDTH * DISPLAY_SCALING_FACTOR, DISPLAY_HEIGHT * DISPLAY_SCALING_FACTOR, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    while(true)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                exit(0);
            }

            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    exit(0);
                }

                for (int i = 0; i < 16; ++i)
                {
                    if (event.key.keysym.sym == chip8.keymap[i])
                    {
                        chip8.keyPadValues[i] = 1;
                    }
                }
            }

            if (event.type == SDL_KEYUP)
            {
                for (int i = 0; i < 16; ++i)
                {
                    if (event.key.keysym.sym == chip8.keymap[i])
                    {
                        chip8.keyPadValues[i] = 0;
                    }
                }
            }
        }


        chip8.decodeOpcode();

        if(chip8.redrawDisplay)
        {
            SDL_RenderClear(renderer);

            for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
                for (int x = 0; x < DISPLAY_WIDTH; ++x) {
                    if (chip8.display[x][y]) {
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    } else {
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    }

                    SDL_Rect rect = { x * DISPLAY_SCALING_FACTOR, y * DISPLAY_SCALING_FACTOR, DISPLAY_SCALING_FACTOR, DISPLAY_SCALING_FACTOR };
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
            SDL_RenderPresent(renderer);
            chip8.redrawDisplay = false;
        }
        usleep(1500);
    }
}
