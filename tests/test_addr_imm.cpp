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
    ram.write(0x8000, 0x42);

    cpu.IMM();
    uint8_t value = cpu.fetch();

    assert(value == 0x42);
    assert(cpu.PC == 0x8001);

    return 0;
}
