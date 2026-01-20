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

    // Test loading a positive value
    cpu.PC = 0x8000;
    ram.write(0x8000, 0xA9); // LDA #$42
    ram.write(0x8001, 0x42);

    cpu.step();

    assert(cpu.A == 0x42);
    assert(cpu.getFlag(CPU6502::Z) == false);
    assert(cpu.getFlag(CPU6502::N) == false);
    assert(cpu.PC == 0x8002);

    // Test loading zero
    cpu.PC = 0x8002;
    ram.write(0x8002, 0xA9); // LDA #$00
    ram.write(0x8003, 0x00);
    
    cpu.step();

    assert(cpu.A == 0x00);
    assert(cpu.getFlag(CPU6502::Z) == true);
    assert(cpu.getFlag(CPU6502::N) == false);
    assert(cpu.PC == 0x8004);

    // Test loading a negative value
    cpu.PC = 0x8004;
    ram.write(0x8004, 0xA9); // LDA #$FF
    ram.write(0x8005, 0xFF);

    cpu.step();

    assert(cpu.A == 0xFF);
    assert(cpu.getFlag(CPU6502::Z) == false);
    assert(cpu.getFlag(CPU6502::N) == true);
    assert(cpu.PC == 0x8006);

    return 0;
}
