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
    ram.write(0x8000, 0xA9); // LDA #$42
    ram.write(0x8001, 0x42);

    cpu.step();

    assert(cpu.A == 0x42);
    assert(cpu.getFlag(CPU6502::Z) == false);
    assert(cpu.getFlag(CPU6502::N) == false);
    assert(cpu.PC == 0x8002);

    return 0;
}
