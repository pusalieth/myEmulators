#include "Hardware.h"
#include <array>
#include <fstream>
#include <random>
#include <iostream>
#include <time.h>

using namespace std;

// Font Set
unsigned char font_set[80] = {
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

void Hardware::init() {

    PC = 0x200;
    SP = 0;
    AP = 0;
    delay_timer = 0;
    sound_timer = 0;

    // Load Font Set into memory starting at location 0x50
    for (int i = 0; i < 80; i++) {
        M[0x50 + i] = font_set[i];
    }
}

void Hardware::CPU_RUN() {
    // fetch cycle opcode
    opcode = ((M[PC] << 8) | M[PC+1]);
    //cout << "Opcode is: 0x" << hex << opcode << endl;

    // decode opcode
    switch (opcode & 0xF000) {
    case 0x0000:
        switch (opcode & 0x00FF) {
        case 0x00E0: // clear screen
            for (int gx = 0; gx < 64; gx++) {
                for(int gy = 0; gy < 32; gy++) {
                    display[gx][gy] = 0;
                }
            }
            PC += 2;
            break;
        case 0x00EE: // returns from subroutine
            SP--;
            PC = stack[SP];
            PC += 2;
            break;
        default: // Calls RCA 1802 program at address NNN. Not necessary for most ROMs.
            cout << "Opcode 0NNN has been called NOP" << endl;
            exit (EXIT_FAILURE);
        }
        break;
    case 0x1000: // (1NNN) Jumps to address NNN
        PC = opcode & 0x0FFF;
        break;
    case 0x2000: // (2NNN) Calls subroutine at NNN
        stack[SP] = PC;
        SP++;
        PC = opcode & 0x0FFF;
        break;
    case 0x3000: // (3XNN) Skips the next instruction if VX equals NN
        if (R[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
            PC += 4;
        } else {
            PC += 2;
        }
        break;
    case 0x4000: // (4XNN) Skips the next instruction if VX doesn't equal NN
        if (R[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
            PC += 4;
        } else {
            PC += 2;
        }
        break;
    case 0x5000: { // (5XY0) Skips the next instruction if VX equals VY
        int x = (opcode & 0x0F00) >> 8;
        int y = (opcode & 0x00F0) >> 4;
        if (R[x] == R[y]) {
            PC += 4;
        } else {
            PC += 2;
        }
        break;
    }
    case 0x6000: // (6XNN) Sets VX to NN
        R[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
        PC += 2;
        break;
    case 0x7000: // (7XNN) Adds NN to VX
        R[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
        PC += 2;
        break;
    case 0x8000:
        switch (opcode & 0x000F) {
        case 0x0000: // (8XY0) Sets VX to the value of VY
            R[(opcode & 0x0F00) >> 8] = R[(opcode & 0x00F0) >> 4];
            PC += 2;
            break;
        case 0x0001: // (8XY1) Sets VX to VX or VY
            R[(opcode & 0x0F00) >> 8] |= R[(opcode & 0x00F0) >> 4];
            PC += 2;
            break;
        case 0x0002: // (8XY2) Sets VX to VX and VY
            R[(opcode & 0x0F00) >> 8] &= R[(opcode & 0x00F0) >> 4];
            PC += 2;
            break;
        case 0x0003: // (8XY3) Sets VX to VX xor VY
            R[(opcode & 0x0F00) >> 8] ^= R[(opcode & 0x00F0) >> 4];
            PC += 2;
            break;
        case 0x0004: { // (8XY4) Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't
            int x = (opcode & 0x0F00) >> 8;
            int y = (opcode & 0x00F0) >> 4;
            int temp = (R[x] + R[y]);
            R[x] += R[y];
            R[x] &= 0xFF;
            if (temp > 255) {
                R[0xF] = 1;
            } else {
                R[0xF] = 0;
            }
            PC += 2;
            break;
        }
        case 0x0005: { // (8XY5) VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't
            int x = (opcode & 0x0F00) >> 8;
            int y = (opcode & 0x00F0) >> 4;
            int temp = (R[x] - R[y]);
            R[x] -= R[y];
            R[x] &= 0xFF;
            if (temp < 0) {
                R[0xF] = 0;
            } else {
                R[0xF] = 1;
            }
            PC += 2;
            break;
        }
        case 0x0006: { // (8XY6) Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift
            int x = (opcode & 0x0F00) >> 8;
            R[0xF] = R[x] & 0x1;
            R[x] >>= 1;
            PC += 2;
            break;
        }
        case 0x0007: { // (8XY7) Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't
            int x = (opcode & 0x0F00) >> 8;
            int y = (opcode & 0x00F0) >> 4;
            int temp = (R[y] - R[x]);
            R[x] = R[y] - R[x];
            R[x] &= 0xFF;
            if (temp < 0) {
                R[0xF] = 0;
            } else {
                R[0xF] = 1;
            }
            PC += 2;
            break;
        }
        case 0x000E: { // (8XYE) Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift
            int x = (opcode & 0x0F00) >> 8;
            R[0xF] = R[x] & 0xF0;
            R[x] <<= 1;
            PC += 2;
            break;
        }
        }
        break;
    case 0x9000: { // (9XY0) Skips the next instruction if VX doesn't equal VY
        int x = (opcode & 0x0F00) >> 8;
        int y = (opcode & 0x00F0) >> 4;
        if (R[x] != R[y]) {
            PC += 4;
        } else {
            PC += 2;
        }
        break;
    }
    case 0xA000: // (ANNN) Sets I to the address NNN
        AP = opcode & 0x0FFF;
        PC += 2;
        break;
    case 0xB000: // (BNNN) Jumps to the address NNN plus V0
        PC = (opcode & 0x0FFF) + R[0];
        break;
    case 0xC000: // (CXNN) Sets VX to the result of a bitwise and operation on a random number and NN
        srand (time(NULL));
        R[(opcode & 0x0F00) >> 8] = (rand() % 256 + 1) & (opcode & 0x00FF);
        PC += 2;
        break;
    case 0xD000: { // (DXYN) Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value doesn’t change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn’t happen
        // Draw XOR to Screen
        // Collision detection, set value to reg16
        // Read image from I
        int VX = R[(opcode & 0x0F00) >> 8];
        int VY = R[(opcode & 0x00F0) >> 4];
        int N = opcode & 0x000F;

        // Clear Collision Flag
        R[0xF] = 0;

        for (int gy = 0; gy < N; gy++) {
            int scanline = M[AP + gy];
            int selectBit = 0x80;

            for (int gx = 0; gx < 8; gx++) {
                int pixel = scanline & selectBit;

                // draw pixel
                if(pixel != 0) {
                    // Collision
                    if (display[VX + gx][VY + gy] == 1) {
                        R[0xF] = 1;
                    }
                    display[VX + gx][VY + gy] ^= 1;
                }
                selectBit >>= 1;
            }
        }
        PC += 2;
        drawFlag = true;
        break;
    }
    case 0xE000:
        switch (opcode & 0x00FF) {
        case 0x009E: { // (EX9E) Skips the next instruction if the key stored in VX is pressed
            int x = (opcode & 0x0F00) >> 8;
            int key = R[x];
            if (peripheral[key] == 1) {
                PC += 4;
            } else {
                PC += 2;
            }
            break;
        }
        case 0x00A1: // (EXA1) Skips the next instruction if the key stored in VX isn't pressed
            int x = (opcode & 0x0F00) >> 8;
            int key = R[x];
            if (peripheral[key] == 0) {
                PC += 4;
            } else {
                PC += 2;
            }
            break;
        }
        break;
    case 0xF000:
        switch (opcode & 0x00FF) {
        case 0x000A: // (FX0A) A key press is awaited, and then stored in VX.
            for (unsigned char i = 0; i < 16; i++) {
                if (peripheral[i] == 1) {
                    R[(opcode & 0x0F00) >> 8] = i;
                    PC += 2;
                }
            }
            break;
        case 0x0007: // (FX07) Sets VX to the value of the delay timer.
            R[(opcode & 0x0F00) >> 8] = delay_timer;
            PC += 2;
            break;
        case 0x0015: // (FX15) Sets the delay timer to VX.
            delay_timer = R[(opcode & 0x0F00) >> 8];
            PC += 2;
            break;
        case 0x0018: // (FX18) Sets the sound timer to VX.
            sound_timer = R[(opcode & 0x0F00) >> 8];
            PC += 2;
            break;
        case 0x001E: // (FX1E) Adds VX to I
            AP += R[(opcode & 0x0F00) >> 8];
            PC += 2;
            break;
        case 0x0029: { // (FX29) Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font
            AP = (0x50 + (R[(opcode & 0x0F00) >> 8] * 5));
            PC += 2;
            break;
        }
        case 0x0033: { // (FX33) Stores the binary-coded decimal representation of VX, with the most significant of three digits at the address in I, the middle digit at I plus 1, and the least significant digit at I plus 2. (In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2)
            unsigned char temp = R[(opcode & 0x0F00) >> 8];
            M[AP] = temp / 100;
            temp %= 100;
            M[AP + 1] = temp / 10;
            temp %= 10;
            M[AP + 2] = temp;
            PC += 2;
            break;
        }
        case 0x0055: { // (FX55) Stores V0 to VX (including VX) in memory starting at address I
            int x = (opcode & 0x0F00) >> 8;
            for (int i = 0; i <= x; i++) {
                M[AP + i] = R[i];
            }
            //AP = AP + limit + 1;
            PC += 2;
            break;
        }
        case 0x0065: { // (FX65) Fills V0 to VX (including VX) with values from memory starting at address I
            int x = (opcode & 0x0F00) >> 8;
            for (int i = 0; i <= x; i++) {
                R[i] = M[AP + i];
            }
            //AP = AP + limit + 1;
            PC += 2;
            break;
        }
        }
        break;
    default:
        cout << "This opcode is unsupported: 0x" << hex << opcode << endl;
        exit (EXIT_FAILURE);
    }
    if (sound_timer > 0) {
        --sound_timer;
    }
    if (delay_timer > 0) {
        --delay_timer;
    }
}

bool Hardware::needsReDraw() {
    return drawFlag;
}

void Hardware::removeDrawFlag() {
    drawFlag = false;
}

void Hardware::loadProgram(string filename) {
    ifstream inFile (filename, ifstream::binary);

    if (!inFile) {
        cout << "File " << filename << " cannot be read. Please try again" << endl;
        return;
    }

    inFile.seekg(0, inFile.end);
    int length = inFile.tellg();
    inFile.seekg(0, inFile.beg);

    char * buffer = new char [length];
    inFile.read(buffer, length);

    for(int i = 0; i < length; i++) {
        M[0x200 + i] = buffer[i];
    }

    if (inFile) {
        cout << filename << " was read correctly" << endl;
    } else {
        cout << "test\n";
        cout << "error: only " << inFile.gcount() << " was read" << endl;
    }

    inFile.close();
}
/*
void Hardware::setKeyBuffer(int[] keyBuffer) {
    for (int i = 0; i < P.size(); i++) {
        P[i] = keyBuffer[i];
    }
}*/
