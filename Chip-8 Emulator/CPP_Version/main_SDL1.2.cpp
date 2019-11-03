#ifdef __cplusplus
#include <cstdlib>
#else
#include <stdlib.h>
#endif

#include <iostream>
#include <SDL/SDL.h>
#include <Hardware.h>
#include <Keyboard.h>
#include <string>
#include <thread>

using namespace std;

// initialize Hardware Object
Hardware chip8;
Keyboard keyboard;

int main(int argc, char **argv) {

    // Initialize the Chip8 system
    chip8.init();

    // Load the game into the memory
    chip8.loadProgram(argv[1]);

    // make sure SDL cleans up before exit
    atexit(SDL_Quit);

    // create a new window
    SDL_Init( SDL_INIT_EVERYTHING );

    int img_width = 64;
    int img_height = 32;
    int scale = 12;
    SDL_Surface* window = SDL_SetVideoMode(img_width*scale, img_height*scale, 8, SDL_HWSURFACE);
    SDL_Rect rect;

    freopen( "CON", "w", stdout );
    freopen( "CON", "w", stderr );

    // Run Program
    while(1) {
            chip8.CPU_RUN();

        keyboard.getKey(&chip8);
        if(keyboard.close()) {
            break;
        }

        if(chip8.needsReDraw()) {
            chip8.removeDrawFlag();
            for(int gy = 0; gy < img_height; gy++) {
                for(int gx = 0; gx < img_width; gx++) {
                    rect.x = gx*scale;
                    rect.y = gy*scale;
                    rect.w = scale;
                    rect.h = scale;
                    if(chip8.display[gx][gy] == 1)
                        SDL_FillRect(window, &rect, SDL_MapRGB(window->format, 255, 255, 255));
                    else
                        SDL_FillRect(window, &rect, SDL_MapRGB(window->format, 0, 0, 0));

                    // clear window
                    //SDL_FillRect(window, 0, SDL_MapRGB(window->format, 0, 0, 0));
                }
            }
            SDL_Flip(window);
            SDL_Delay(8);
        }
    }

    return 0;
}
