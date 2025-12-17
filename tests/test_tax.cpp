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

    cpu.PC = 0x8000;
    cpu.A  = 0x80;

    ram.write(0x8000, 0xAA); // TAX

    cpu.step();

    assert(cpu.X == 0x80);
    assert(cpu.getFlag(CPU6502::N) == true);
    assert(cpu.getFlag(CPU6502::Z) == false);

    return 0;
}
