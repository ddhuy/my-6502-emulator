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

    // Minimal test for ADC instruction
    cpu.setFlag(CPU6502::C, false);
    cpu.A = 0xFF;

    ram.write(0x0, 0x01); // Value to add
    
    cpu.ADC();

    assert(cpu.A == 0x00);
    assert(cpu.getFlag(CPU6502::C) == true);

    // Zero flag test
    cpu.setFlag(CPU6502::C, false);
    cpu.A = 0x01;

    ram.write(0x0, 0xFF);

    cpu.ADC();

    assert(cpu.A == 0x00);
    assert(cpu.getFlag(CPU6502::Z) == true);

    // Negative flag test
    cpu.setFlag(CPU6502::C, false);
    cpu.A = 0x40;

    ram.write(0x0, 0x40);

    cpu.ADC();

    assert(cpu.A == 0x80);
    assert(cpu.getFlag(CPU6502::N) == true);

    // Overflow flag test: Positive + Positive = Negative (overflow)
    cpu.setFlag(CPU6502::C, false);
    cpu.A = 0x50;

    ram.write(0x0, 0x50);

    cpu.ADC();

    assert(cpu.A == 0xA0);
    assert(cpu.getFlag(CPU6502::V) == true);
    assert(cpu.getFlag(CPU6502::N) == true);

    // Negative + Negative = Positive (overflow)
    cpu.setFlag(CPU6502::C, false);
    cpu.A = 0x90;

    ram.write(0x0, 0x90);

    cpu.ADC();

    assert(cpu.A == 0x20);
    assert(cpu.getFlag(CPU6502::V) == true);
    assert(cpu.getFlag(CPU6502::N) == false);

    // Mixed sign addition (no overflow)
    cpu.setFlag(CPU6502::C, false);
    cpu.A = 0x50;

    ram.write(0x0, 0x90);

    cpu.ADC();

    assert(cpu.A == 0xE0);
    assert(cpu.getFlag(CPU6502::V) == false);
    assert(cpu.getFlag(CPU6502::N) == true);

    // Simple decimal mode test
    cpu.setFlag(CPU6502::D, true);
    cpu.setFlag(CPU6502::C, false);
    cpu.A = 0x15;

    ram.write(0x0, 0x27);

    cpu.ADC();

    assert(cpu.A == 0x42);
    assert(cpu.getFlag(CPU6502::C) == false);

    // Decimal mode with carry
    cpu.setFlag(CPU6502::D, true);
    cpu.setFlag(CPU6502::C, false);
    cpu.A = 0x45;

    ram.write(0x0, 0x55);

    cpu.ADC();

    assert(cpu.A == 0x00);
    assert(cpu.getFlag(CPU6502::C) == true);

    // Decimal mode with carry in
    cpu.setFlag(CPU6502::D, true);
    cpu.setFlag(CPU6502::C, true);
    cpu.A = 0x45;

    ram.write(0x0, 0x54);

    cpu.ADC();

    assert(cpu.A == 0x00);
    assert(cpu.getFlag(CPU6502::C) == true);

    // Overflow
    cpu.setFlag(CPU6502::D, true);
    cpu.setFlag(CPU6502::C, false);
    cpu.A = 0x50;

    ram.write(0x0, 0x50);

    cpu.ADC();

    assert(cpu.A == 0x00);
    assert(cpu.getFlag(CPU6502::C) == true);
    assert(cpu.getFlag(CPU6502::V) == true);

    return 0;
}