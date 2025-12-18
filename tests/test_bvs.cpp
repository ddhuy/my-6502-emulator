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

    // Test BVS when overflow is set
    cpu.setFlag(CPU6502::V, true);
    cpu.PC = 0x8000;

    ram.write(0x8000, 0x70); // BVS
    ram.write(0x8001, 0x04);

    cpu.step();

    assert(cpu.PC == 0x8006);

    // Test BVS when overflow is clear
    cpu.setFlag(CPU6502::V, false);
    cpu.PC = 0x8000;

    ram.write(0x8000, 0x70); // BVS
    ram.write(0x8001, 0x04);

    cpu.step();

    assert(cpu.PC == 0x8002);

    return 0;
}