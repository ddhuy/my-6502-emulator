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

    //
    cpu.setFlag(CPU6502::C, false);
    cpu.A = 0xFF;
    cpu.PC = 0x8000;

    ram.write(0x8000, 0x6A); // ROR A
    
    cpu.step();
    
    assert(cpu.A == 0x7F);
    assert(cpu.getFlag(CPU6502::C) == true);
    assert(cpu.getFlag(CPU6502::Z) == false);
    assert(cpu.getFlag(CPU6502::N) == false);

    return 0;
}