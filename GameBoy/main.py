class HARDWARE:
    def __init__(self):
        init_cpu()
        init_RAM()
        init_GPU()
        init_input()

    def init_cpu(self):
        pass

    def init_RAM(self):
        pass

    def init_GPU(self):
        pass
    
    def init_input(self):
        pass

class CPU:
    def __init__(self):
        sp = 0
        ap = 0

    def decode(self, opcode):
        if(opcode and 0xff00 == 0x03):
            pass
        