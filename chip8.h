//
// Created by Alexey Golubev on 24.08.2024.
//

#ifndef CHIP_8_CHIP8_H
#define CHIP_8_CHIP8_H

#include <vector>
#include <map>

const int DISPLAY_WIDTH = 64;
const int DISPLAY_HEIGHT = 32;
const int DISPLAY_SCALING_FACTOR = 20;

class Chip8 {
public:
    Chip8();

    bool loadROM(std::string romPath);
    void decodeOpcode();

    void printDisplay();
    bool redrawDisplay = false;
    bool display[DISPLAY_WIDTH][DISPLAY_HEIGHT];

    uint8_t keymap[16] = {
            SDLK_x,
            SDLK_1,
            SDLK_2,
            SDLK_3,
            SDLK_q,
            SDLK_w,
            SDLK_e,
            SDLK_a,
            SDLK_s,
            SDLK_d,
            SDLK_z,
            SDLK_c,
            SDLK_4,
            SDLK_r,
            SDLK_f,
            SDLK_v,
    };

    bool keyPadValues[16];

protected:
    uint8_t ram[4 * 1024];

    uint16_t pc, I;
    std::vector<uint16_t> stack;
    uint8_t V[16];
    uint8_t delayTimer, soundTimer;

protected:
    void loadFont();
};

#endif //CHIP_8_CHIP8_H
