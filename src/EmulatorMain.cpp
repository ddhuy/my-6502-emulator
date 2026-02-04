#include <iostream>
#include "CPU.h"
#include "Bus.h"
#include "Memory.h"


int main()
{
    Memory memory;
    Bus bus;
    CPU cpu;

    bus.ConnectMemory(&memory);
    bus.ConnectCPU(&cpu);

    bus.Reset();
    
    std::cout << "+===========================+" << std::endl;
    std::cout << "|      My NES Emulator      |" << std::endl;
    std::cout << "+===========================+" << std::endl;
    std::cout << "CPU initialized successfully!" << std::endl;
    std::cout << "Initial state:" << std::endl;
    std::cout << "  A:  0x" << std::hex << (int)cpu.A << std::endl;
    std::cout << "  X:  0x" << std::hex << (int)cpu.X << std::endl;
    std::cout << "  Y:  0x" << std::hex << (int)cpu.Y << std::endl;
    std::cout << "  SP: 0x" << std::hex << (int)cpu.SP << std::endl;
    std::cout << "  PC: 0x" << std::hex << (int)cpu.PC << std::endl;
    std::cout << "  P:  0x" << std::hex << (int)cpu.P << std::endl;
    
    return 0;
}