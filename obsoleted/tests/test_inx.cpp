#include <cassert>
#include "cpu/CPU6502.hpp"
#include "bus/Bus.hpp"
#include "mem/RAM.hpp"


int main()
{
    CPU6502 cpu;
    cpu.X = 0xFE;

    // Test incrementing X from a non-boundary value
    cpu.INX();
    assert(cpu.X == 0xFF);
    assert(cpu.getFlag(CPU6502::Z) == false);
    assert(cpu.getFlag(CPU6502::N) == true);
    
    // Test incrementing X to zero
    cpu.INX();
    assert(cpu.X == 0x00);
    assert(cpu.getFlag(CPU6502::Z) == true);
    assert(cpu.getFlag(CPU6502::N) == false);

    // Test incrementing X from zero
    cpu.INX();
    assert(cpu.X == 0x01);
    assert(cpu.getFlag(CPU6502::Z) == false);
    assert(cpu.getFlag(CPU6502::N) == false);
}
