# written by jake pring

import random
import sys


class CONTROL_UNIT:
    def __init__(self, game_file):
        self.RAM = [None] * 0xFFF
        self.PC = 0x200
        self.I = 0
        self.SP = 0xEA0
        self.REGISTERS = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        # 0xF00-0xFFF display
        # 0xEA0-0xEFF call stack,
        self.load(game_file)

        while True:
            self.cpu_decode(self.RAM[self.PC], self.RAM[self.PC + 1])

    def load(self, game_file):
        file_bytes = open(game_file, "rb").read()
        file_bytes = bytearray(file_bytes)
        for i in range(0, len(file_bytes)):
            self.RAM[self.PC + self.I] = file_bytes[self.I]
            self.I += 1

    def cpu_decode(self, MSB_Opcode, LSB_Opcode):
        opcode = (MSB_Opcode << 8) | LSB_Opcode
        opcode_prefix = opcode & 0xF000
        print(hex(opcode))
        if(opcode_prefix & 0x00FF):
            if(opcode == 0x00EE):
                print("Unsupported opcode 0x00EE %s" % hex(opcode))
            elif(opcode == 0x00E0):
                # disp_clear()
                print("Unsupported opcode 0x00E0 %s" % hex(opcode))
            elif(opcode == 0x0FFF):
                print("Unsupported opcode 0x00EE %s" % hex(opcode))
            else:
                print("Unsupported opcode 0x00EE %s" % hex(opcode))
        elif(opcode_prefix & 0x1000):
            # goto opcode & 0x0FFF
            print("Unsupported opcode 0x1 %s" % hex(opcode))
        elif(opcode_prefix & 0x2000):
            # call subroutine at opcode & 0x0FFF
            self.RAM[self.SP] = self.PC
            self.PC = (opcode & 0x0FFF)
            return
        elif(opcode_prefix & 0x3000):
            vx = (opcode & 0x0F00)
            if(self.REGISTERS[vx] == (opcode & 0x00FF)):
                self.PC += 2
        elif(opcode_prefix & 0x4000):
            vx = (opcode & 0x0F00)
            if(self.REGISTERS[vx] != (opcode & 0x00FF)):
                self.PC += 2
        elif(opcode_prefix & 0x5000):
            vx = (opcode & 0x0F00)
            vy = (opcode & 0x00F0)
            if(self.REGISTERS[vx] == self.REGISTERS[vy]):
                self.PC += 2
        elif(opcode_prefix & 0x6000):
            opcode & 0x0FFF
            vx = (opcode & 0x0F00)
            self.REGISTERS[vx] = (opcode & 0x00FF)
        elif(opcode_prefix & 0x7000):
            vx = (opcode & 0x0F00)
            self.REGISTERS[vx] += (opcode & 0x00FF)
        elif(opcode_prefix & 0x8000):
            opcode_suffix = opcode & 0x000F
            if(opcode_suffix & 0x0000):
                vx = (opcode & 0x0F00)
                vy = (opcode & 0x00F0)
                self.REGISTERS[vx] = self.REGISTERS[vy]
            elif(opcode_suffix & 0x0001):
                vx = (opcode & 0x0F00)
                vy = (opcode & 0x00F0)
                self.REGISTERS[vx] |= self.REGISTERS[vy]
            elif(opcode_suffix & 0x0002):
                vx = (opcode & 0x0F00)
                vy = (opcode & 0x00F0)
                self.REGISTERS[vx] &= self.REGISTERS[vy]
            elif(opcode_suffix & 0x0003):
                vx = (opcode & 0x0F00)
                vy = (opcode & 0x00F0)
                self.REGISTERS[vx] ^= self.REGISTERS[vy]
            elif(opcode_suffix & 0x0004):
                vx = (opcode & 0x0F00)
                vy = (opcode & 0x00F0)
                self.REGISTERS[vx] += self.REGISTERS[vy]
                if(self.REGISTERS[vx] > 0xFF):
                    self.REGISTERS[vx] = 0xFF
                    self.REGISTERS[0xF] = 1
                else:
                    self.REGISTERS[0xF] = 0
            elif(opcode_suffix & 0x0005):
                vx = (opcode & 0x0F00)
                vy = (opcode & 0x00F0)
                self.REGISTERS[vx] -= self.REGISTERS[vy]
                if(self.REGISTERS[vx] < 0):
                    self.REGISTERS[vx] = 0
                    self.REGISTERS[0xF] = 1
                else:
                    self.REGISTERS[0xF] = 0
            elif(opcode_suffix & 0x0006):
                vx = (opcode & 0x0F00)
                vy = (opcode & 0x00F0)
                self.REGISTERS[0xF] = self.REGISTERS[vx] & 0x01
                self.REGISTERS[vx] >>= 1
            elif(opcode_suffix & 0x0007):
                vx = (opcode & 0x0F00)
                vy = (opcode & 0x00F0)
                self.REGISTERS[vx] = self.REGISTERS[vy] - self.REGISTERS[vx]
                if(self.REGISTERS[vx] < 0):
                    self.REGISTERS[vx] = 0
                    self.REGISTERS[0xF] = 1
                else:
                    self.REGISTERS[0xF] = 0
            elif(opcode_suffix & 0x000E):
                vx = (opcode & 0x0F00)
                vy = (opcode & 0x00F0)
                self.REGISTERS[0xF] = self.REGISTERS[vx] & 0xF0
                self.REGISTERS[vx] <<= 1
            else:
                print("Unsupported opcode %s" % hex(opcode))
        elif(opcode_prefix & 0x9000):
            vx = (opcode & 0x0F00)
            vy = (opcode & 0x00F0)
            if(self.REGISTERS[vx] != self.REGISTERS[vy]):
                self.PC += 2
        elif(opcode_prefix & 0xA000):
            self.I = (opcode & 0x0FFF)
        elif(opcode_prefix & 0xB000):
            self.PC = self.REGISTERS[0] + (opcode & 0x0FFF)
            return
        elif(opcode_prefix & 0xC000):
            random.seed()
            vx = (opcode & 0x0F00)
            self.REGISTERS[vx] + (random.randint(0, 255)
                                  & (opcode & 0x0FFF))
        elif(opcode_prefix & 0xD000):
            print()
        else:
            print("Unsupported opcode %s" % hex(opcode))

        self.PC += 2
        print(self.PC)


class IO:
    def __init__(self):
        print()


class SOUND:
    def __init__(self):
        print()

    def delay(self, set_time):
        while(set_time > 0):
            set_time -= 1

    def sound(self, set_time):
        while(set_time > 0):
            set_time -= 1


class GRAPHICS:
    def __init__(self):
        print()


if __name__ == "__main__":
    self = CONTROL_UNIT(sys.argv[1])
