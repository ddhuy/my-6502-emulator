#include <iostream>
#include "cpu/CPU.h"

int main() {
    std::cout << "NES Emulator - Phase 1: CPU Infrastructure" << std::endl;
    std::cout << "===========================================" << std::endl;
    
    CPU cpu;
    
    std::cout << "CPU initialized successfully!" << std::endl;
    std::cout << "Initial state:" << std::endl;
    std::cout << "  A:  0x" << std::hex << (int)cpu.A << std::endl;
    std::cout << "  X:  0x" << std::hex << (int)cpu.X << std::endl;
    std::cout << "  Y:  0x" << std::hex << (int)cpu.Y << std::endl;
    std::cout << "  SP: 0x" << std::hex << (int)cpu.SP << std::endl;
    std::cout << "  PC: 0x" << std::hex << (int)cpu.PC << std::endl;
    std::cout << "  P:  0x" << std::hex << (int)cpu.P << std::endl;
    
    std::cout << "\nRun 'make test' to execute all unit tests." << std::endl;
    
    return 0;
}