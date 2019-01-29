
/*
 * GPLv3
 */
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.Random;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.Clip;

/**
 *
 * @author Jake Pring
 */
public class Hardware {

    private char[] register;
    private char opcode;
    private char PC;
    private char[] memory;
    private char addressPointer;

    private char[] stack;
    private int stackPointer;

    private int delay_timer;
    private int sound_timer;

    private byte[] peripheral;

    private byte[][] display;

    private boolean needReDraw;

    private int scale = 0;

    public void setScale(int Scale) {
        scale = Scale;
    }

    public int getScale() {
        return scale;
    }

    public void init() {
        memory = new char[4096];
        register = new char[16];
        addressPointer = 0;
        PC = 0x200;

        stack = new char[16];
        stackPointer = 0;

        delay_timer = 0;
        sound_timer = 0;

        peripheral = new byte[16];

        display = new byte[64][32];

        needReDraw = false;

        loadFontSet();
    }

    public void loadFontSet() {
        // Font Set starts at Memory position 0x50
        int[] fontset = { 0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
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
                0xF0, 0x80, 0xF0, 0x80, 0x80 // F
        };

        for (int i = 0; i < fontset.length; i++) {
            memory[0x50 + i] = (char) (fontset[i] & 0xFF);
        }
    }

    public void CPU_run() {
        // fetch opcode
        opcode = (char) ((memory[PC] << 8) | memory[PC + 1]);
        // System.out.println("PC: " + Integer.toHexString(PC) + " Opcode: 0x" +
        // Integer.toHexString(opcode));
        // decode opcode
        switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x00FF) {
            case 0x00E0: // clear screen
                for (int x = 0; x < 64; x++) {
                    for (int y = 0; y < 32; y++) {
                        display[x][y] = 0;
                    }
                }
                needReDraw = true;
                PC += 2;
                break;
            case 0x00EE: // returns from subroutine
                stackPointer--;
                PC = (char) (stack[stackPointer]);
                PC += 2;
                break;
            default: // Calls RCA 1802 program at address NNN. Not necessary for most ROMs.
                break;
            }
            break;
        case 0x1000: // (1NNN) Jumps to address NNN
            PC = (char) (opcode & 0x0FFF);
            break;
        case 0x2000: // (2NNN) Calls subroutine at NNN
            stack[stackPointer] = PC;
            stackPointer++;
            PC = (char) (opcode & 0x0FFF);
            break;
        case 0x3000: // (3XNN) Skips the next instruction if VX equals NN
            if (register[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
                PC += 4;
            } else {
                PC += 2;
            }
            break;
        case 0x4000: // (4XNN) Skips the next instruction if VX doesn't equal NN
            if (register[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
                PC += 4;
            } else {
                PC += 2;
            }
            break;
        case 0x5000: {// (5XY0) Skips the next instruction if VX equals VY
            int x = (opcode & 0x0F00) >> 8;
            int y = (opcode & 0x00F0) >> 4;
            if (register[x] == register[y]) {
                PC += 4;
            } else {
                PC += 2;
            }
            break;
        }
        case 0x6000: // (6XNN) Sets VX to NN
            register[(opcode & 0x0F00) >> 8] = (char) (opcode & 0x00FF);
            PC += 2;
            break;
        case 0x7000: // (7XNN) Adds NN to VX
            register[(opcode & 0x0F00) >> 8] += (char) (opcode & 0x00FF);
            PC += 2;
            break;
        case 0x8000:
            switch (opcode & 0x000F) {
            case 0x0000: // (8XY0) Sets VX to the value of VY
                register[(opcode & 0x0F00) >> 8] = register[(opcode & 0x00F0) >> 4];
                PC += 2;
                break;
            case 0x0001: // (8XY1) Sets VX to VX or VY
                register[(opcode & 0x0F00) >> 8] |= register[(opcode & 0x00F0) >> 4];
                PC += 2;
                break;
            case 0x0002: // (8XY2) Sets VX to VX and VY
                register[(opcode & 0x0F00) >> 8] &= register[(opcode & 0x00F0) >> 4];
                PC += 2;
                break;
            case 0x0003: // (8XY3) Sets VX to VX xor VY
                register[(opcode & 0x0F00) >> 8] ^= register[(opcode & 0x00F0) >> 4];
                PC += 2;
                break;
            case 0x0004: {// (8XY4) Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when
                          // there isn't
                int x = (opcode & 0x0F00) >> 8;
                int y = (opcode & 0x00F0) >> 4;
                int temp = (register[x] + register[y]);
                register[x] += register[y];
                register[x] &= 0xFF;
                if (temp > 0xFF) {
                    register[0xF] = 1;
                } else {
                    register[0xF] = 0;
                }
                PC += 2;
                break;
            }
            case 0x0005: {// (8XY5) VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1
                          // when there isn't
                int x = (opcode & 0x0F00) >> 8;
                int y = (opcode & 0x00F0) >> 4;
                int temp = register[x] - register[y];
                register[x] -= register[y];
                if (temp < 0) {
                    register[0xF] = 0;
                } else {
                    register[0xF] = 1;
                }
                PC += 2;
                break;
            }
            case 0x0006: {// (8XY6) Shifts VX right by one. VF is set to the value of the least
                          // significant bit of VX before the shift
                int x = (opcode & 0x0F00) >> 8;
                register[0xF] = (char) (register[x] & 0x1);
                register[x] >>= 1;
                PC += 2;
                break;
            }
            case 0x0007: {// (8XY7) Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1
                          // when there isn't
                int x = (opcode & 0x0F00) >> 8;
                int y = (opcode & 0x00F0) >> 4;
                int temp = register[y] - register[x];
                register[x] = (char) (register[y] - register[x]);
                if (temp < 0) {
                    register[0xF] = 0;
                } else {
                    register[0xF] = 1;
                }
                PC += 2;
                break;
            }
            case 0x000E: {// (8XYE) Shifts VX left by one. VF is set to the value of the most significant
                          // bit of VX before the shift
                int x = (opcode & 0x0F00) >> 8;
                register[0xF] = (char) (register[x] & 0xF0);
                register[x] <<= 1;
                PC += 2;
                break;
            }
            }
            break;
        case 0x9000: {// (9XY0) Skips the next instruction if VX doesn't equal VY
            int x = (opcode & 0x0F00) >> 8;
            int y = (opcode & 0x00F0) >> 4;
            if (register[x] != register[y]) {
                PC += 4;
            } else {
                PC += 2;
            }
            break;
        }
        case 0xA000: // (ANNN) Sets I to the address NNN
            addressPointer = (char) (opcode & 0x0FFF);
            PC += 2;
            break;
        case 0xB000: // (BNNN) Jumps to the address NNN plus V0
            PC = (char) ((opcode & 0x0FFF) + register[0]);
            break;
        case 0xC000: // (CXNN) Sets VX to the result of a bitwise and operation on a random number
                     // and NN
            register[(opcode & 0x0F00) >> 8] = (char) (new Random().nextInt(256) & (opcode & 0x00FF));
            PC += 2;
            break;
        case 0xD000: {// (DXYN) Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and
                      // a height of N pixels. Each row of 8 pixels is read as bit-coded starting from
                      // memory location I; VF is set to 1 if any screen pixels are flipped from set
                      // to unset when the sprite is drawn, and to 0 if that doesn’t happen
            // Draw XOR to Screen
            // Collision detection, set value to reg16
            // Read image from I
            int VX = register[(opcode & 0x0F00) >> 8];
            int VY = register[(opcode & 0x00F0) >> 4];
            int N_height = opcode & 0x000F;

            // Clear Collision flag
            register[0xF] = 0;

            // loop through vertical lines
            for (int gy = 0; gy < N_height; gy++) {
                int scanline = memory[addressPointer + gy];
                int selectBit = 0x80;

                // loop through horizontal lines
                for (int gx = 0; gx < 8; gx++) {
                    if (display[(VX + gx) % 64][(VY + gy) % 32] == 0x1) {
                        register[0xF] = 1;
                    }
                    if ((scanline & selectBit) != 0) {
                        display[(VX + gx) % 64][(VY + gy) % 32] ^= 1;
                    }

                    selectBit >>= 1;
                }
            }
            needReDraw = true;
            PC += 2;
            break;
        }
        case 0xE000:
            switch (opcode & 0x00FF) {
            case 0x009E: {// (EX9E) Skips the next instruction if the key stored in VX is pressed
                int x = (opcode & 0x0F00) >> 8;
                int key = register[x];
                if (peripheral[key] == 1) {
                    PC += 4;
                } else {
                    PC += 2;
                }
                break;
            }
            case 0x00A1: // (EXA1) Skips the next instruction if the key stored in VX isn't pressed
                int x = (opcode & 0x0F00) >> 8;
                int key = register[x];
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
                for (int i = 0; i < peripheral.length; i++) {
                    if (peripheral[i] == 1) {
                        register[(opcode & 0x0F00) >> 8] = (char) i;
                        PC += 2;
                    }
                }
                break;
            case 0x0007: // (FX07) Sets VX to the value of the delay timer.
                register[(opcode & 0x0F00) >> 8] = (char) delay_timer;
                PC += 2;
                break;
            case 0x0015: // (FX15) Sets the delay timer to VX.
                delay_timer = register[(opcode & 0x0F00) >> 8];
                PC += 2;
                break;
            case 0x0018: // (FX18) Sets the sound timer to VX.
                sound_timer = register[(opcode & 0x0F00) >> 8];
                PC += 2;
                break;
            case 0x001E: // (FX1E) Adds VX to I
                addressPointer += register[(opcode & 0x0F00) >> 8];
                PC += 2;
                break;
            case 0x0029: {// (FX29) Sets I to the location of the sprite for the character in VX.
                          // Characters 0-F (in hexadecimal) are represented by a 4x5 font
                addressPointer = (char) (0x50 + (register[(opcode & 0x0F00) >> 8] * 5));
                PC += 2;
                break;
            }
            case 0x0033: {// (FX33) Stores the binary-coded decimal representation of VX, with the most
                          // significant of three digits at the address in I, the middle digit at I plus
                          // 1, and the least significant digit at I plus 2. (In other words, take the
                          // decimal representation of VX, place the hundreds digit in memory at location
                          // in I, the tens digit at location I+1, and the ones digit at location I+2)
                char temp = register[(opcode & 0x0F00) >> 8];
                memory[addressPointer] = (char) (temp / 100);
                temp %= 100;
                memory[addressPointer + 1] = (char) (temp / 10);
                temp %= 10;
                memory[addressPointer + 2] = (char) temp;
                PC += 2;
                break;
            }
            case 0x0055: {// (FX55) Stores V0 to VX (including VX) in memory starting at address I
                int x = (opcode & 0x0F00) >> 8;
                for (int i = 0; i <= x; i++) {
                    memory[addressPointer + i] = register[i];
                }
                PC += 2;
                break;
            }
            case 0x0065: {// (FX65) Fills V0 to VX (including VX) with values from memory starting at
                          // address I
                int x = (opcode & 0x0F00) >> 8;
                for (int i = 0; i <= x; i++) {
                    register[i] = memory[addressPointer + i];
                }
                PC += 2;
                break;
            }
            }
            break;
        default:
            System.err.println("This opcode is unsupported: 0x" + Integer.toHexString(opcode));
            System.exit(0);
        }
        if (sound_timer > 0) {
            sound_timer--;
            playSound("../Beep.wav");
        }
        if (delay_timer > 0) {
            delay_timer--;
        }
    }

    public void playSound(String filename) {
        try {
            Clip clip = AudioSystem.getClip();
            AudioInputStream audioInput = AudioSystem.getAudioInputStream(new File(filename));
            clip.open(audioInput);
            clip.start();
        } catch (Exception e) {
            System.err.println("Failed to play audio file: " + e.getMessage());
        }
    }

    public byte[][] getDisplay() {
        return display;
    }

    public boolean needsReDraw() {
        return needReDraw;
    }

    public void removeDrawFlag() {
        needReDraw = false;
    }

    public void loadProgram(String filename) {
        DataInputStream input = null;
        try {
            input = new DataInputStream(new FileInputStream(new File(filename)));

            // Game data starts at memeory location 0x200
            for (int offset = 0; input.available() > 0; offset++) {
                memory[0x200 + offset] = (char) (input.readByte() & 0xFF);
            }
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(0);
        } finally {
            if (input != null) {
                try {
                    input.close();
                } catch (IOException ex) {
                    //
                }
            }
        }
    }

    public void setKeyBuffer(int[] keyBuffer) {
        for (int i = 0; i < 16; i++) {
            peripheral[i] = (byte) keyBuffer[i];
        }
    }
}
