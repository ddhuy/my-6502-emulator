#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <string>
#include <sstream>
#include <iomanip>
#include "cpu/CPU.h"
#include "cpu/Instructions.h"


class Disassembler
{
public:
    static std::string Disassemble(CPU& cpu, uint16_t address)
    {
        uint8_t opcode = cpu.ReadMemory(address);
        const Instruction& instr = INSTRUCTION_TABLE[opcode];

        std::ostringstream oss;
        oss << std::hex << std::uppercase << std::setfill('0');

        // Address
        oss << std::setw(4) << address << "  ";

        // Opcde bytes
        oss << std::setw(2) << (int) opcode << "  ";

        // Operand bytes (depends on addressing mode)
        int bytes = GetInstructionBytes(instr.addrMode);
        for (int i = 0; i < bytes; ++i)
        {
            oss << std::setw(2) << (int) cpu.ReadMemory(address + i) << " ";
        }

        // Padding
        for (int i = bytes; i < 3; ++i)
        {
            oss << "   ";
        }

        // Mnemonic
        oss << " " << instr.name << " ";

        // Operand format
        oss << FormatOperand(cpu, address, instr.addrMode);
        
        return oss.str();
    }

private:
    static int GetInstructionBytes(AddressModeFunc addrMode)
    {
        // Return instruction length based on addressing mode
        if (addrMode == &CPU::IMP) return 1;
        if (addrMode == &CPU::IMM) return 2;
        if (addrMode == &CPU::ZP0) return 2;
        if (addrMode == &CPU::ZPX) return 2;
        if (addrMode == &CPU::ZPY) return 2;
        if (addrMode == &CPU::REL) return 2;
        if (addrMode == &CPU::ABS) return 3;
        if (addrMode == &CPU::ABX) return 3;
        if (addrMode == &CPU::ABY) return 3;
        if (addrMode == &CPU::IND) return 3;
        if (addrMode == &CPU::IZX) return 2;
        if (addrMode == &CPU::IZY) return 2;
        return 1;
    }

    static std::string FormatOperand(CPU& cpu, uint16_t address, AddressModeFunc addrMode)
    {
        std::ostringstream oss;
        oss << std::hex << std::uppercase << std::setfill('0');
        
        if (addrMode == &CPU::IMP) {
            return "";
        } else if (addrMode == &CPU::IMM) {
            uint8_t value = cpu.ReadMemory(address + 1);
            oss << "#$" << std::setw(2) << (int)value;
        } else if (addrMode == &CPU::ZP0) {
            uint8_t addr = cpu.ReadMemory(address + 1);
            oss << "$" << std::setw(2) << (int)addr;
        } else if (addrMode == &CPU::ZPX) {
            uint8_t addr = cpu.ReadMemory(address + 1);
            oss << "$" << std::setw(2) << (int)addr << ",X";
        } else if (addrMode == &CPU::ZPY) {
            uint8_t addr = cpu.ReadMemory(address + 1);
            oss << "$" << std::setw(2) << (int)addr << ",Y";
        } else if (addrMode == &CPU::ABS) {
            uint16_t addr = cpu.ReadMemory(address + 1) | 
                           (cpu.ReadMemory(address + 2) << 8);
            oss << "$" << std::setw(4) << addr;
        } else if (addrMode == &CPU::ABX) {
            uint16_t addr = cpu.ReadMemory(address + 1) | 
                           (cpu.ReadMemory(address + 2) << 8);
            oss << "$" << std::setw(4) << addr << ",X";
        } else if (addrMode == &CPU::ABY) {
            uint16_t addr = cpu.ReadMemory(address + 1) | 
                           (cpu.ReadMemory(address + 2) << 8);
            oss << "$" << std::setw(4) << addr << ",Y";
        } else if (addrMode == &CPU::IND) {
            uint16_t addr = cpu.ReadMemory(address + 1) | 
                           (cpu.ReadMemory(address + 2) << 8);
            oss << "($" << std::setw(4) << addr << ")";
        } else if (addrMode == &CPU::IZX) {
            uint8_t addr = cpu.ReadMemory(address + 1);
            oss << "($" << std::setw(2) << (int)addr << ",X)";
        } else if (addrMode == &CPU::IZY) {
            uint8_t addr = cpu.ReadMemory(address + 1);
            oss << "($" << std::setw(2) << (int)addr << "),Y";
        } else if (addrMode == &CPU::REL) {
            int8_t offset = cpu.ReadMemory(address + 1);
            uint16_t target = address + 2 + offset;
            oss << "$" << std::setw(4) << target;
        }
        
        return oss.str();
    }
};

#endif // DISASSEMBLER_H