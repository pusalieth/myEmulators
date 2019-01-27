# written by jake pring

import random
import sys


class CONTROL_UNIT:
    def __init__(self, game_file):
        self.RAM = [None] * 0xFFF
        self.load(game_file)
        self.CPU = CPU(self.PC, self.I)
        # self.CPU.decode(self.RAM[self.PC])

    def load(self, game_file):
        self.PC = 0x200
        self.I = 0
        file_bytes = open(game_file, "rb").read()
        file_bytes = bytearray(file_bytes)
        for i in xrange(0, len(file_bytes)):
            self.RAM[self.PC + self.I] = file_bytes[self.I]
            self.I += 1
        print(self.RAM[0x200:0x200 + self.I])


class CPU:
    def __init__(self, ProgramCounter, MemoryPointer):
        self.REGISTERS = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        self.PC = ProgramCounter
        self.I = MemoryPointer

    def decode(self, opcode):
        if(opcode & 0x00E0):
            # disp_clear()
            print("Unsupported opcode %s" % opcode)
        elif(opcode & 0x00EE):
            print("Unsupported opcode %s" % opcode)
        elif(opcode & 0x1000):
            # goto opcode & 0x0FFF
            print("Unsupported opcode %s" % opcode)
        elif(opcode & 0x2000):
            # call subroutine at opcode & 0x0FFF
            print("Unsupported opcode %s" % opcode)
        elif(opcode & 0x3000):
            vx = (opcode & 0x0F00)
            if(self.REGISTERS[vx] == (opcode & 0x00FF)):
                self.PC += 2
        elif(opcode & 0x4000):
            vx = (opcode & 0x0F00)
            if(self.REGISTERS[vx] != (opcode & 0x00FF)):
                self.PC += 2
        elif(opcode & 0x5000):
            vx = (opcode & 0x0F00)
            vy = (opcode & 0x00F0)
            if(self.REGISTERS[vx] == self.REGISTERS[vy]):
                self.PC += 2
        elif(opcode & 0x6000):
            vx = (opcode & 0x0F00)
            self.REGISTERS[vx] = (opcode & 0x00FF)
        elif(opcode & 0x7000):
            vx = (opcode & 0x0F00)
            self.REGISTERS[vx] += (opcode & 0x00FF)
        elif(opcode & 0x8000):
            vx = (opcode & 0x0F00)
            vy = (opcode & 0x00F0)
            self.REGISTERS[vx] = self.REGISTERS[vy]
        elif(opcode & 0x8001):
            vx = (opcode & 0x0F00)
            vy = (opcode & 0x00F0)
            self.REGISTERS[vx] |= self.REGISTERS[vy]
        elif(opcode & 0x8002):
            vx = (opcode & 0x0F00)
            vy = (opcode & 0x00F0)
            self.REGISTERS[vx] &= self.REGISTERS[vy]
        elif(opcode & 0x8003):
            vx = (opcode & 0x0F00)
            vy = (opcode & 0x00F0)
            self.REGISTERS[vx] ^= self.REGISTERS[vy]
        elif(opcode & 0x8004):
            vx = (opcode & 0x0F00)
            vy = (opcode & 0x00F0)
            self.REGISTERS[vx] += self.REGISTERS[vy]
            if(self.REGISTERS[vx] > 0xFF):
                self.REGISTERS[vx] = 0xFF
                self.REGISTERS[0xF] = 1
            else:
                self.REGISTERS[0xF] = 0
        elif(opcode & 0x8005):
            vx = (opcode & 0x0F00)
            vy = (opcode & 0x00F0)
            self.REGISTERS[vx] -= self.REGISTERS[vy]
            if(self.REGISTERS[vx] < 0):
                self.REGISTERS[vx] = 0
                self.REGISTERS[0xF] = 1
            else:
                self.REGISTERS[0xF] = 0
        elif(opcode & 0x8006):
            vx = (opcode & 0x0F00)
            vy = (opcode & 0x00F0)
            self.REGISTERS[0xF] = self.REGISTERS[vx] & 0x01
            self.REGISTERS[vx] >>= 1
        elif(opcode & 0x8007):
            vx = (opcode & 0x0F00)
            vy = (opcode & 0x00F0)
            self.REGISTERS[vx] = self.REGISTERS[vy] - self.REGISTERS[vx]
            if(self.REGISTERS[vx] < 0):
                self.REGISTERS[vx] = 0
                self.REGISTERS[0xF] = 1
            else:
                self.REGISTERS[0xF] = 0
        elif(opcode & 0x800E):
            vx = (opcode & 0x0F00)
            vy = (opcode & 0x00F0)
            self.REGISTERS[0xF] = self.REGISTERS[vx] & 0xF0
            self.REGISTERS[vx] <<= 1
        elif(opcode & 0x9000):
            vx = (opcode & 0x0F00)
            vy = (opcode & 0x00F0)
            if(self.REGISTERS[vx] != self.REGISTERS[vy]):
                self.PC += 2
        elif(opcode & 0xA000):
            self.I = (opcode & 0x0FFF)
        elif(opcode & 0xB000):
            self.PC = self.REGISTERS[0] + (opcode & 0x0FFF)
        elif(opcode & 0xC000):
            random.seed()
            vx = (opcode & 0x0F00)
            self.REGISTERS[vx] + (random.randint(0, 255)
                                  & (opcode & 0x0FFF))
        elif(opcode & 0xD000):
            print()
        else:
            print("Unsupported opcode %s" % opcode)

        self.PC += 2


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
