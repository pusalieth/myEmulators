#include "Keyboard.h"
#include <SDL/SDL.h>

void Keyboard::getKey(Hardware* chip8) {
    SDL_Event event;

    while ( SDL_PollEvent(&event) ) {
        if (event.type == SDL_KEYDOWN) {
            switch(event.key.keysym.sym) {
            case SDLK_1:
                chip8->peripheral[0x1] = 1;
                break;
            case SDLK_2:
                chip8->peripheral[0x2] = 1;
                break;
            case SDLK_3:
                chip8->peripheral[0x3] = 1;
                break;
            case SDLK_4:
                chip8->peripheral[0xC] = 1;
                break;
            case SDLK_q:
                chip8->peripheral[0x4] = 1;
                break;
            case SDLK_w:
                chip8->peripheral[0x5] = 1;
                break;
            case SDLK_e:
                chip8->peripheral[0x6] = 1;
                break;
            case SDLK_r:
                chip8->peripheral[0xD] = 1;
                break;
            case SDLK_a:
                chip8->peripheral[0x7] = 1;
                break;
            case SDLK_s:
                chip8->peripheral[0x8] = 1;
                break;
            case SDLK_d:
                chip8->peripheral[0x9] = 1;
                break;
            case SDLK_f:
                chip8->peripheral[0xE] = 1;
                break;
            case SDLK_z:
                chip8->peripheral[0xA] = 1;
                break;
            case SDLK_x:
                chip8->peripheral[0x0] = 1;
                break;
            case SDLK_c:
                chip8->peripheral[0xB] = 1;
                break;
            case SDLK_v:
                chip8->peripheral[0xF] = 1;
                break;
            case SDLK_ESCAPE:
                exit(1);
                break;
            }
        } else if (event.type == SDL_KEYUP) {
            switch(event.key.keysym.sym) {
            case SDLK_1:
                chip8->peripheral[0x1] = 0;
                break;
            case SDLK_2:
                chip8->peripheral[0x2] = 0;
                break;
            case SDLK_3:
                chip8->peripheral[0x3] = 0;
                break;
            case SDLK_4:
                chip8->peripheral[0xC] = 0;
                break;
            case SDLK_q:
                chip8->peripheral[0x4] = 0;
                break;
            case SDLK_w:
                chip8->peripheral[0x5] = 0;
                break;
            case SDLK_e:
                chip8->peripheral[0x6] = 0;
                break;
            case SDLK_r:
                chip8->peripheral[0xD] = 0;
                break;
            case SDLK_a:
                chip8->peripheral[0x7] = 0;
                break;
            case SDLK_s:
                chip8->peripheral[0x8] = 0;
                break;
            case SDLK_d:
                chip8->peripheral[0x9] = 0;
                break;
            case SDLK_f:
                chip8->peripheral[0xE] = 0;
                break;
            case SDLK_z:
                chip8->peripheral[0xA] = 0;
                break;
            case SDLK_x:
                chip8->peripheral[0x0] = 0;
                break;
            case SDLK_c:
                chip8->peripheral[0xB] = 0;
                break;
            case SDLK_v:
                chip8->peripheral[0xF] = 0;
                break;
            }
        } else if (event.type == SDL_QUIT) {
            quit = true;
        }
    }
}

bool Keyboard::close() {
    return quit;
}
