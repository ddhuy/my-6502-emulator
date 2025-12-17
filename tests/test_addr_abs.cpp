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
    ram.write(0x8000, 0x34);
    ram.write(0x8001, 0x12);
    ram.write(0x1234, 0x77);

    cpu.ABS();
    uint8_t value = cpu.fetch();

    assert(value == 0x77);
    assert(cpu.PC == 0x8002);

    return 0;
}
