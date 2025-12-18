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

    // Test BMI when negative flag is set
    cpu.setFlag(CPU6502::N, true);
    cpu.PC = 0x8000;

    ram.write(0x8000, 0x30); // BMI
    ram.write(0x8001, 0x04);

    cpu.step();

    assert(cpu.PC == 0x8006);

    // Test BMI when negative flag is clear
    cpu.setFlag(CPU6502::N, false);
    cpu.PC = 0x8000;

    ram.write(0x8000, 0x30); // BMI
    ram.write(0x8001, 0x04);

    cpu.step();

    assert(cpu.PC == 0x8002);

    return 0;
}