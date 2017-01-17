#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "Hardware.h"

class Keyboard {
public:
    void getKey(Hardware* chip8);
    bool close();

protected:

private:
    bool quit = false;
};

#endif // KEYBOARD_H
