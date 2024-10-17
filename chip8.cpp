//
// Created by Alexey Golubev on 24.08.2024.
//
#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>

#include "chip8.h"

const uint8_t fontSet[80] =
        {
                0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
                0x20, 0x60, 0x20, 0x20, 0x70, // 1
                0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
                0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
                0x90, 0x90, 0xF0, 0x10, 0x10, // 4
                0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
                0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
                0xF0, 0x10, 0x20, 0x40, 0x40, // 7
                0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
                0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
                0xF0, 0x90, 0xF0, 0x90, 0x90, // A
                0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
                0xF0, 0x80, 0x80, 0x80, 0xF0, // C
                0xE0, 0x90, 0x90, 0x90, 0xE0, // D
                0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
                0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };

Chip8::Chip8()
{
    for(int i = 0; i < DISPLAY_WIDTH; i++){
        for(int j = 0; j < DISPLAY_HEIGHT; j++)
        {
            display[i][j] = false;
        }
    }

    loadFont();

    pc = 0x200; // 512
}

void Chip8::loadFont()
{
    for(int i = 0; i < 80; i++) { ram[i] = fontSet[i]; }
}

bool Chip8::loadROM(std::string romPath)
{
    std::ifstream f(romPath, std::ios::binary | std::ios::in);
    if (!f.is_open())
    {
        return false;
    }

    char c;
    int j = 512;
    for (int i = j; f.get(c); i++)
    {
        if (j >= 4096) { return false; } //file size too big memory space over so exit
        ram[i] = (uint8_t) c;
        j++;
    }
    return true;
}

void Chip8::decodeOpcode()
{
    uint16_t opcode = (ram[pc] << 8) | ram[pc + 1];

    pc += 2;
    //std::cout << "Opcode: " << std::hex << opcode << std::endl;
    switch (opcode & 0xF000)
    {
        case 0x0000:
            if(opcode == 0x00E0)
            {
                for(int i = 0; i < DISPLAY_WIDTH; i++){
                    for(int j = 0; j < DISPLAY_HEIGHT; j++)
                    {
                        display[i][j] = false;
                    }
                }
                redrawDisplay = true;
            } else if (opcode == 0x00EE)
            {
                pc = stack.back();
                stack.pop_back();
                pc += 2;
            }
            break;
        case 0x1000:
            pc -= 2;
            pc = opcode & 0x0FFF;
            //std::cout << "Jump to address: 0x" << std::hex << pc << std::endl;
            break;
        case 0x2000:
            pc -= 2;
            stack.push_back(pc);
            pc = opcode & 0x0FFF;
            break;
        case 0x3000:
            if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
            {
                pc += 2;
            }
            break;
        case 0x4000:
            if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
            {
                pc += 2;
            }
            break;
        case 0x5000:
            if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
            {
                pc += 2;
            }
            break;
        case 0x6000:
            V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
            break;
        case 0x7000:
            V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
            break;
        case 0x8000:
            switch (opcode & 0x000F)
            {
                case 0x0000:
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0001:
                    V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
                    V[0xF] = 0;
                    break;
                case 0x0002:
                    V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
                    V[0xF] = 0;
                    break;
                case 0x0003:
                    V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
                    V[0xF] = 0;
                    break;
                case 0x0004:
                {
                    uint16_t resultOfAddition = V[(opcode & 0x0F00) >> 8] + V[(opcode & 0x00F0) >> 4];
                    V[(opcode & 0x0F00) >> 8] = resultOfAddition;
                    V[(opcode & 0x0F00) >> 8] = (int8_t) V[(opcode & 0x0F00) >> 8];
                    if (resultOfAddition > 0xFF) {
                        V[0xF] = 1;
                    } else {
                        V[0xF] = 0;
                    }
                    break;
                }
                case 0x0005:
                    if(V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])
                    {
                        V[0xF] = 1;
                    } else
                    {
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >> 8] = (uint8_t) V[(opcode & 0x0F00) >> 8] - (uint8_t) V[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0006:
                    V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
                    V[(opcode & 0x0F00) >> 8] = (uint8_t) V[(opcode & 0x0F00) >> 8];
                    break;
                case 0x0007:
                    if(V[(opcode & 0x0F00) >> 8] < V[(opcode & 0x00F0) >> 4])
                    {
                        V[0xF] = 1;
                    } else
                    {
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x00F0) >> 4] = (uint8_t) V[(opcode & 0x00F0) >> 4] - (uint8_t) V[(opcode & 0x0F00) >> 8];
                    break;
                case 0x000E:
                    V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
                    V[0xF] = (uint8_t) V[0xF];
                    V[(opcode & 0x0F00) >> 8] <<= 1;
                    break;
                default:
                    break;
            }
            break;
        case 0x9000:
            if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
            {
                pc += 2;
            }
            break;
        case 0xA000:
            I = opcode & 0x0FFF;
            break;
        case 0xB000:
            pc = (opcode & 0x0FFF);
            pc += V[0x0];
            break;
        case 0xC000:
            V[(opcode & 0x0F00) >> 8] = (rand() % (0xFF + 1)) & (opcode & 0x00FF);
            break;
        case 0xD000:
        {
            int h = (opcode & 0x000F);
            int w = 8;
            int x = (V[(opcode & 0x0F00) >> 8] & 63);
            int y = (V[(opcode & 0x00F0) >> 4] & 31);

            V[0x0F] = 0;

            for(int i = 0; i < h; i++)
            {
                int pixel = ram[I + i];
                for(int j = 0; j < w; j++)
                {
                    if ((pixel & (0x80 >> j)) != 0)
                    {
                        if (display[x + j][y + i] == 1)
                        {
                            V[0x0F] = 1;
                        }
                        display[x + j][y + i] ^= 1;
                    }
                }
            }

            redrawDisplay = true;
            break;
        }
        case 0xE000:
            switch (opcode & 0x00FF)
            {
                case 0x009E:
                    if(keyPadValues[V[(opcode & 0x0F00) >> 8]] != 0)
                    {
                        pc += 2;
                    }
                    break;
                case 0x00A1:
                    if(keyPadValues[V[(opcode & 0x0F00) >> 8]] == 0)
                    {
                        pc += 2;
                    }
                    break;
                default:
                    break;
            }
            break;
        case 0xF000:
            switch (opcode & 0x00FF)
            {
                case 0x001E:
                    if (I + V[(opcode & 0x0F00) >> 8] > 0xFFF)
                    {
                        V[0xF] = 1;
                    }
                    else
                    {
                        V[0xF] = 0;
                    }
                    I += V[(opcode & 0x0F00) >> 8];
                    I = (uint16_t) I;
                    break;
                case 0x0007:
                    V[(opcode & 0x0F00) >> 8] = delayTimer;
                    break;
                case 0x0015:
                    delayTimer = V[(opcode & 0x0F00) >> 8];
                    break;
                case 0x0018:
                    soundTimer = V[(opcode & 0x0F00) >> 8];
                    break;
                case 0x0029:
                    I = V[(opcode & 0x0F00) >> 8] * 0x5;
                    break;
                case 0x0033:
                    ram[I] = (uint8_t) ((uint8_t) V[(opcode & 0x0F00) >> 8] / 100);
                    ram[I + 1] = (uint8_t) ((uint8_t) (V[(opcode & 0x0F00) >> 8] / 10) % 10);
                    ram[I + 2] = (uint8_t) ((uint8_t) (V[(opcode & 0x0F00) >> 8] % 100) % 10);
                    break;
                case 0x0055:
                {
                    uint16_t reg = (opcode & 0x0F00) >> 8;
                    for (int i = 0; i <= reg; i++)
                    {
                        ram[I + i] = V[i];
                    }
                    I = I + reg + 1;
                    I = (uint16_t) I;
                    break;
                }
                case 0x0065:
                {
                    uint16_t reg = (opcode & 0x0F00) >> 8;
                    for (int i = 0; i <= reg; i++)
                    {
                        V[i] = ram[I + i];
                    }
                    I = I + reg + 1;
                    I = (uint16_t) I;
                    break;
                }
                case 0x000A:
                    pc -= 2;
                    bool key_pressed = false;
                    for (int i = 0; i < 16; i++)
                    {
                        if (keyPadValues[i] != 0)
                        {
                            key_pressed = true;
                            V[(opcode & 0x0F00) >> 8] = (uint8_t) i;
                        }
                    }
                    if (key_pressed)
                    {
                        pc += 2;
                    }
                    break;
            }
            if (delayTimer > 0)
            {
                delayTimer--;
            }
            if (soundTimer > 0)
            {
                printf("\a\n"); //terminal dependent, should be changed later
                soundTimer--;
            }
    }
}

void Chip8::printDisplay() {
        for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
            for (int x = 0; x < DISPLAY_WIDTH; ++x) {
                std::cout << (display[x][y] ? 'X' : '.');
            }
            std::cout << std::endl;
        }
}
