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

    // Branch NOT taken (baseline)
    {
        cpu.PC = 0x8000;
        cpu.setFlag(CPU6502::Z, false);

        ram.write(0x8000, 0xF0); // BEQ
        ram.write(0x8001, 0x02);

        int cycles = 0;
        do {
            cpu.clock();
            cycles++;
        } while (cpu.cycles() > 0);

        assert(cpu.PC == 0x8002);
        assert(cycles == 2); // 2
    }

    // Branch taken, no page cross
    {
        cpu.PC = 0x8000;
        ram.write(0x8000, 0xF0);
        ram.write(0x8001, 0x02);

        cpu.setFlag(CPU6502::Z, true);

        int cycles = 0;
        do {
            cpu.clock();
            cycles++;
        } while (cpu.cycles() > 0);
        
        assert(cpu.PC == 0x8004);
        assert(cycles == 3); // 2 + 1
    }

    // Branch taken, forward page cross
    {
        cpu.PC = 0x80FD;
        ram.write(0x80FD, 0xF0);
        ram.write(0x80FE, 0x02);

        cpu.setFlag(CPU6502::Z, true);

        int cycles = 0;
        do {
            cpu.clock();
            cycles++;
        } while (cpu.cycles() > 0);

        assert(cpu.PC == 0x8101);
        assert(cycles == 4); // 2 + 1 + 1
    }

    // Branch taken backward, no page cross
    {
        cpu.PC = 0x8005;
        ram.write(0x8005, 0xF0);
        ram.write(0x8006, 0xFB); // -5

        cpu.setFlag(CPU6502::Z, true);

        int cycles = 0;
        do {
            cpu.clock();
            cycles++;
        } while (cpu.cycles() > 0);

        assert(cpu.PC == 0x8002);
        assert(cycles == 3); // 2 + 1
    }

    // Branch taken backward, page cross → 4 cycles
    {
        cpu.PC = 0x8102;
        ram.write(0x8102, 0xF0);
        ram.write(0x8103, 0xFB); // -5

        cpu.setFlag(CPU6502::Z, true);

        int cycles = 0;
        do {
            cpu.clock();
            cycles++;
        } while (cpu.cycles() > 0);

        assert(cpu.PC == 0x80FF);
        assert(cycles == 4); // 2 + 1 + 1
    }

    // Zero offset
    {
        cpu.PC = 0x8000;
        cpu.setFlag(CPU6502::Z, true);

        ram.write(0x8000, 0xF0);
        ram.write(0x8001, 0x00);

        int cycles = 0;
        do {
            cpu.clock();
            cycles++;
        } while (cpu.cycles() > 0);

        assert(cpu.PC == 0x8002);
        assert(cycles == 3); // 2 + 1
    }

    return 0;
}