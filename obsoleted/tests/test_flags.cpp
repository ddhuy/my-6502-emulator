#include <cassert>

#include "cpu/CPU6502.hpp"


int main()
{
    CPU6502 cpu;

    // Test setting and getting flags
    cpu.setFlag(CPU6502::StatusFlag::C, true);
    assert(cpu.getFlag(CPU6502::StatusFlag::C) == true);
    cpu.setFlag(CPU6502::StatusFlag::C, false);
    assert(cpu.getFlag(CPU6502::StatusFlag::C) == false);

    // Test updating Zero
    cpu.updateZN(0);
    assert(cpu.getFlag(CPU6502::StatusFlag::Z) == true);
    assert(cpu.getFlag(CPU6502::StatusFlag::N) == false);
    
    // Test with a negative value
    cpu.updateZN(0x80);
    assert(cpu.getFlag(CPU6502::StatusFlag::Z) == false);
    assert(cpu.getFlag(CPU6502::StatusFlag::N) == true);

    return 0;
}