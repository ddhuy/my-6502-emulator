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
    ram.write(0x8000, 0x10);
    ram.write(0x0010, 0x99);

    cpu.ZP0();
    uint8_t value = cpu.fetch();

    assert(value == 0x99);
    assert(cpu.PC == 0x8001);

    return 0;
}
