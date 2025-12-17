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

    // Test loading a value from absolute address
    cpu.PC = 0x8000;
    ram.write(0x8000, 0xAD); // LDA $1234
    ram.write(0x8001, 0x34);
    ram.write(0x8002, 0x12);
    ram.write(0x1234, 0x42);

    cpu.step();

    assert(cpu.A == 0x42);
    assert(cpu.getFlag(CPU6502::Z) == false);
    assert(cpu.getFlag(CPU6502::N) == false);
    assert(cpu.PC == 0x8003);

    // Test loading zero
    cpu.PC = 0x8003;
    ram.write(0x8003, 0xAD); // LDA $5678
    ram.write(0x8004, 0x78);
    ram.write(0x8005, 0x56);
    ram.write(0x5678, 0x00);

    cpu.step();

    assert(cpu.A == 0x00);
    assert(cpu.getFlag(CPU6502::Z) == true);
    assert(cpu.getFlag(CPU6502::N) == false);
    assert(cpu.PC == 0x8006);

    // Test loading a negative value
    cpu.PC = 0x8006;
    ram.write(0x8006, 0xAD); // LDA $9ABC
    ram.write(0x8007, 0xBC);
    ram.write(0x8008, 0x9A);
    ram.write(0x9ABC, 0xFF);

    cpu.step();

    assert(cpu.A == 0xFF);
    assert(cpu.getFlag(CPU6502::Z) == false);
    assert(cpu.getFlag(CPU6502::N) == true);
    assert(cpu.PC == 0x8009);

    return 0;
}
