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
    cpu.A = 0x01;
    cpu.PC = 0x8000;
    
    ram.write(0x8000, 0x0A); // ASL A

    cpu.step();

    assert(cpu.A == 0x02);
    assert(cpu.getFlag(CPU6502::C) == false);
    assert(cpu.getFlag(CPU6502::Z) == false);
    assert(cpu.getFlag(CPU6502::N) == false);

    //
    cpu.A = 0x80;
    cpu.PC = 0x8000;

    ram.write(0x8000, 0x0A); // ASL A

    cpu.step();

    assert(cpu.A == 0x00);
    assert(cpu.getFlag(CPU6502::C) == true);
    assert(cpu.getFlag(CPU6502::Z) == true);
    assert(cpu.getFlag(CPU6502::N) == false);

    return 0;
}