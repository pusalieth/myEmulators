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

    //int channels = 1;
    int img_width = 64;
    int img_height = 32;
    int scale = 12;
    SDL_Surface* window = SDL_SetVideoMode(img_width*scale, img_height*scale+10, 8, SDL_HWSURFACE);

    freopen( "CON", "w", stdout );
    freopen( "CON", "w", stderr );

    clock_t start = clock ();
    clock_t timeElapsed;
    unsigned msElapsed;

    // Run Program
    while(1) {
        timeElapsed = clock() - start;
        msElapsed = timeElapsed / (CLOCKS_PER_SEC / 1000);

        if(msElapsed >= 2) {
            start = clock ();
            chip8.CPU_RUN();
        }

        keyboard.getKey(&chip8);
        if(keyboard.close()) {
            break;
        }

        if(chip8.needsReDraw()) {
            chip8.removeDrawFlag();

            SDL_Rect rect;
            rect.x = 0;
            rect.y = 0;
            rect.h = 64*scale;
            rect.w = 64*scale;

            for(int gy = 0; gy < 32; gy++) {
                for(int gx = 0; gx < 64; gx++) {
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
        }
    }

    return 0;
}
