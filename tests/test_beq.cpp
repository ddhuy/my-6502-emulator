#include <cassert>
#include "cpu/CPU6502.hpp"
#include "bus/Bus.hpp"
#include "mem/RAM.hpp"


int main()
{
    RAM ram;
    Bus bus;
    CPU6502 cpu;

    bus.attachMemory(&ram);
    cpu.connectBus(&bus);

    // Test BEQ taken
    cpu.setFlag(CPU6502::Z, true);
    cpu.PC = 0x8000;

    ram.write(0x8000, 0xF0); // BEQ
    ram.write(0x8001, 0x06); // +6

    cpu.step();

    assert(cpu.PC == 0x8008);

    // Test BEQ not taken
    cpu.setFlag(CPU6502::Z, false);
    
    ram.write(0x8008, 0xF0); // BEQ
    ram.write(0x8009, 0x05); // +5
    
    cpu.step();
    
    assert(cpu.PC == 0x800A);

    // Test backward branch with BEQ
    cpu.setFlag(CPU6502::Z, true);

    ram.write(0x800A, 0xF0); // BEQ
    ram.write(0x800B, 0xF4); // -12

    cpu.step();

    assert(cpu.PC == 0x8000);

    return 0;
}
