#ifndef HARDWARE_H
#define HARDWARE_H

#include <string>

using namespace std;

class Hardware {
public:
    void init();
    void CPU_RUN();
    void loadProgram(string filename);
    unsigned char** getDisplay();
    bool needsReDraw();
    void removeDrawFlag();
    unsigned char display[64][32];   // Display, default size 64x32 width*height
    unsigned char peripheral[16];   // Peripheral Device

protected:

private:
    unsigned short opcode;          // (8-bits)
    unsigned char R[16];            // Registers (8-bits)
    unsigned short PC;              // Program Counter (16-bits)
    unsigned short stack[16];       // Stack memory (16-bits)
    unsigned short SP;              // Stack Pointer (16-bits)
    unsigned char M[4096];          // memory locations up to 4096 (8-bits)
    unsigned short AP;              // Address Pointer (16-bits)

    unsigned char delay_timer;      // Delay Timer
    unsigned char sound_timer;      // Sound Timer

    bool drawFlag = false;
};

#endif // HARDWARE_H
