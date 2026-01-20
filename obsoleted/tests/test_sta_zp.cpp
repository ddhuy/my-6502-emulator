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
    cpu.A  = 0x55;

    ram.write(0x8000, 0x85); // STA $10
    ram.write(0x8001, 0x10);

    cpu.step();

    assert(ram.read(0x0010) == 0x55);
    assert(cpu.PC == 0x8002);

    return 0;
}
