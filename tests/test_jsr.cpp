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
    cpu.SP = 0xFD;

    ram.write(0x8000, 0x20); // JSR
    ram.write(0x8001, 0x34);
    ram.write(0x8002, 0x12);

    cpu.step();

    assert(cpu.PC == 0x1234);
    assert(cpu.SP == 0xFB);

    assert(ram.read(0x01FD) == 0x80);
    assert(ram.read(0x01FC) == 0x02);

    return 0;
}
