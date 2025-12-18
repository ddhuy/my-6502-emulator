#include <sstream>
#include <iomanip>
#include "Disassembler.hpp"
#include "cpu/CPU6502.hpp"
#include "cpu/Instructions.hpp"
#include "bus/Bus.hpp"


std::map<uint16_t, std::string>
Disassembler::disassemble(Bus& bus, uint16_t start, uint16_t end)
{
    std::map<uint16_t, std::string> result;
    uint16_t pc = start;

    while (pc <= end) {
        uint16_t line_addr = pc;
        uint8_t opcode = bus.read(pc++);
        auto& inst = instructionTable[opcode];

        std::stringstream ss;
        ss << inst.name << " ";

        if (inst.addrmode == &CPU6502::IMP) {
            // nothing
        }
        else if (inst.addrmode == &CPU6502::IMM) {
            uint8_t value = bus.read(pc++);
            ss << "#$" << std::hex << std::setw(2) << std::setfill('0') << int(value);
        }
        else if (inst.addrmode == &CPU6502::ABS) {
            uint8_t lo = bus.read(pc++);
            uint8_t hi = bus.read(pc++);
            uint16_t addr = (hi << 8) | lo;
            ss << "$" << std::hex << std::setw(4) << std::setfill('0') << addr;
        }
        else if (inst.addrmode == &CPU6502::REL) {
            int8_t offset = bus.read(pc++);
            uint16_t target = pc + offset;
            ss << "$" << std::hex << std::setw(4) << target;
        }
        else {
            ss << "<TODO>";
        }

        result[line_addr] = ss.str();
    }

    return result;
}
