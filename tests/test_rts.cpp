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

    cpu.SP = 0xFB;

    ram.write(0x01FC, 0x01); // low
    ram.write(0x01FD, 0x80); // high

    ram.write(0x8000, 0x60); // RTS
    cpu.PC = 0x8000;

    cpu.step();

    assert(cpu.PC == 0x8002);
    assert(cpu.SP == 0xFD);

    return 0;
}
