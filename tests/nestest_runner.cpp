#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

#include "bus/Bus.h"
#include "cpu/Instructions.h"
#include "utils/Logger.h"


class NESTestRunner
{
public:
    Memory memory;
    Bus bus;
    CPU cpu;

    std::ofstream logFile;

    int instructionCount;

    NESTestRunner() : instructionCount(0)
    {
        bus.ConnectMemory(&memory);
        cpu.ConnectBus(&bus);
        cpu.Reset();

        // Drain reset cycles
        while (cpu.GetCycles() > 0)
            cpu.Clock();
    }

    virtual ~NESTestRunner() {}

    bool LoadROM(const char* filename)
    {
        std::ifstream file(filename, std::ios::binary);
        if (!file)
        {
            LOG_ERROR("Could not open NES file: %s", filename);
            return false;
        }

        // Read iNES header
        char header[16];
        file.read(header, sizeof(header));

        if (header[0] != 'N' || header[1] != 'E' || header[2] != 'S' || header[3] != 0x1A)
        {
            LOG_ERROR("Invalid NES ROM header");
            return false;
        }

        // Get PRG ROM size (int 16KB units)
        int prgSize = header[4] * 0x4000;

        // Read PRG ROM (16KB)
        std::vector<uint8_t> prgRom(prgSize);
        file.read((char *) prgRom.data(), prgSize);

        // Load into CPU mermoy at 0x8000-0xBFFF and mirror at 0xC000-0xFFFF
        for (int i = 0; i < prgSize && i < 0x4000; ++i)
        {
            cpu.WriteMemory(0x8000 + i, prgRom[i]);
            cpu.WriteMemory(0xC000 + i, prgRom[i]);
        }

        LOG_INFO("Loaded %d bytes of PRG ROM", prgSize);
        return true;
    }

    std::string GetMnemonic(uint16_t address)
    {
        uint8_t opcode = cpu.ReadMemory(address);
        return INSTRUCTION_TABLE[opcode].name;
    }

    int GetInstructionLength(uint16_t address)
    {
        uint8_t opcode = cpu.ReadMemory(address);
        auto addrmode = INSTRUCTION_TABLE[opcode].addrMode;
        
        // Determine instruction length based on addressing mode
        if (addrmode == &CPU::IMP) return 1;
        if (addrmode == &CPU::IMM) return 2;
        if (addrmode == &CPU::ZP0) return 2;
        if (addrmode == &CPU::ZPX) return 2;
        if (addrmode == &CPU::ZPY) return 2;
        if (addrmode == &CPU::REL) return 2;
        if (addrmode == &CPU::IZX) return 2;
        if (addrmode == &CPU::IZY) return 2;
        if (addrmode == &CPU::ABS) return 3;
        if (addrmode == &CPU::ABX) return 3;
        if (addrmode == &CPU::ABY) return 3;
        if (addrmode == &CPU::IND) return 3;
        
        return 1;
    }

    // Returns true and sets effAddr to the effective memory address for modes
    // that nestest annotates with " = XX". Returns false for modes that don't.
    bool GetEffectiveAddress(uint16_t address, uint16_t& effAddr)
    {
        uint8_t opcode = cpu.ReadMemory(address);
        auto addrmode  = INSTRUCTION_TABLE[opcode].addrMode;
        const char* mn = INSTRUCTION_TABLE[opcode].name;

        uint8_t op1 = cpu.ReadMemory(address + 1);
        uint8_t op2 = cpu.ReadMemory(address + 2);

        // JMP/JSR print the target, not a "= value", so skip them.
        bool isJump = (mn[0] == 'J');

        if (addrmode == &CPU::ZP0) {
            effAddr = op1;                              return true;
        } else if (addrmode == &CPU::ZPX) {
            effAddr = (uint8_t)(op1 + cpu.X);           return true;  // wraps in page 0
        } else if (addrmode == &CPU::ZPY) {
            effAddr = (uint8_t)(op1 + cpu.Y);           return true;
        } else if (addrmode == &CPU::ABS) {
            if (isJump) return false;
            effAddr = op1 | (op2 << 8);                 return true;
        } else if (addrmode == &CPU::ABX) {
            effAddr = (op1 | (op2 << 8)) + cpu.X;       return true;
        } else if (addrmode == &CPU::ABY) {
            effAddr = (op1 | (op2 << 8)) + cpu.Y;       return true;
        } else if (addrmode == &CPU::IZX) {
            uint8_t ptr = (uint8_t)(op1 + cpu.X);
            uint8_t lo  = cpu.ReadMemory(ptr);
            uint8_t hi  = cpu.ReadMemory((uint8_t)(ptr + 1));
            effAddr = lo | (hi << 8);                   return true;
        } else if (addrmode == &CPU::IZY) {
            uint8_t lo = cpu.ReadMemory(op1);
            uint8_t hi = cpu.ReadMemory((uint8_t)(op1 + 1));
            effAddr = (lo | (hi << 8)) + cpu.Y;         return true;
        }
        return false;  // IMM, IMP, ACC, REL, JMP/JSR — no "= XX"
    }

    std::string FormatInstruction(uint16_t address)
    {
        uint8_t opcode = cpu.ReadMemory(address);
        auto addrmode = INSTRUCTION_TABLE[opcode].addrMode;
        const char* mnemonic = INSTRUCTION_TABLE[opcode].name;

        std::ostringstream oss;
        oss << std::hex << std::uppercase << std::setfill('0');
        
        // Get operand bytes
        uint8_t op1 = 0, op2 = 0;
        int length = GetInstructionLength(address);
        if (length >= 2) op1 = cpu.ReadMemory(address + 1);
        if (length >= 3) op2 = cpu.ReadMemory(address + 2);
        
        oss << mnemonic << " ";
        
        // Format based on addressing mode
        if (addrmode == &CPU::IMP) {
            return std::string(mnemonic);
        } else if (addrmode == &CPU::IMM) {
            oss << "#$" << std::setw(2) << (int)op1;
        } else if (addrmode == &CPU::ZP0) {
            oss << "$" << std::setw(2) << (int)op1;
        } else if (addrmode == &CPU::ZPX) {
            oss << "$" << std::setw(2) << (int)op1 << ",X";
        } else if (addrmode == &CPU::ZPY) {
            oss << "$" << std::setw(2) << (int)op1 << ",Y";
        } else if (addrmode == &CPU::ABS) {
            uint16_t addr = op1 | (op2 << 8);
            oss << "$" << std::setw(4) << addr;
        } else if (addrmode == &CPU::ABX) {
            uint16_t addr = op1 | (op2 << 8);
            oss << "$" << std::setw(4) << addr << ",X";
        } else if (addrmode == &CPU::ABY) {
            uint16_t addr = op1 | (op2 << 8);
            oss << "$" << std::setw(4) << addr << ",Y";
        } else if (addrmode == &CPU::IND) {
            uint16_t addr = op1 | (op2 << 8);
            oss << "($" << std::setw(4) << addr << ")";
        } else if (addrmode == &CPU::IZX) {
            oss << "($" << std::setw(2) << (int)op1 << ",X)";
        } else if (addrmode == &CPU::IZY) {
            oss << "($" << std::setw(2) << (int)op1 << "),Y";
        } else if (addrmode == &CPU::REL) {
            int8_t offset = (int8_t)op1;
            uint16_t target = address + 2 + offset;
            oss << "$" << std::setw(4) << target;
        }

        uint16_t effAddr;
        if (GetEffectiveAddress(address, effAddr))
        {
            oss << " = " << std::setw(2) << (int)cpu.ReadMemory(effAddr);
        }
        
        return oss.str();
    }

    void LogState()
    {
        std::ostringstream oss;
        oss << std::hex << std::uppercase << std::setfill('0');
        
        uint16_t pc = cpu.PC;
        
        // PC and opcode bytes
        oss << std::setw(4) << pc << "  ";
        
        int length = GetInstructionLength(pc);
        for (int i = 0; i < length; i++)
        {
            oss << std::setw(2) << (int)cpu.ReadMemory(pc + i) << " ";
        }
        
        // Padding to align instruction (nestest uses 3 bytes max)
        for (int i = length; i < 3; i++)
        {
            oss << "   ";
        }
        
        // Instruction
        oss << std::hex << std::uppercase << std::setfill(' ');
        oss << " " << std::setw(32) << std::left << FormatInstruction(pc);
        
        // Registers
        oss << "A:"  << std::setw(2) << std::setfill('0') << std::right << (int)cpu.A << " ";
        oss << "X:"  << std::setw(2) << std::setfill('0') << std::right << (int)cpu.X << " ";
        oss << "Y:"  << std::setw(2) << std::setfill('0') << std::right << (int)cpu.Y << " ";
        oss << "P:"  << std::setw(2) << std::setfill('0') << std::right << (int)cpu.P << " ";
        oss << "SP:" << std::setw(2) << std::setfill('0') << std::right << (int)cpu.SP << " ";

        // PPU
        uint64_t ppu_cycles = cpu.GetTotalCycles() * 3;

        int scanline = (ppu_cycles / 341) % 262;
        int dot = ppu_cycles % 341;

        oss << "PPU:"
            << std::dec << std::setw(3) << std::setfill(' ') << std::right
            << scanline << ","
            << std::dec << std::setw(3) << std::setfill(' ') << std::right
            << dot << " ";
        
        // Cycles (PPU cycles = CPU cycles * 3)
        oss << "CYC:" << std::dec << cpu.GetTotalCycles();
        
        logFile << oss.str() << std::endl;
    }

    bool Run()
    {
        // Start at automated test entry point
        cpu.Reset();
        cpu.PC = 0xC000;

        // Drain reset cycles
        while (cpu.GetCycles() > 0)
            cpu.Clock();
        
        logFile.open("my_nestest.log");
        if (!logFile)
        {
            LOG_ERROR("Failed to create log file");
            return false;
        }

        LOG_INFO("Starting nestest at PC: 0x%04X", cpu.PC);

        const int MAX_INSTRUCTIONS = 30000;
        uint16_t currentPC = 0;

        while (instructionCount < MAX_INSTRUCTIONS)
        {
            LogState();

            currentPC = cpu.PC;

            // Check for infinite looping (test complete)
            if (currentPC == 0xC66E)
            {
                LOG_INFO("Test completed at PC: 0x%04X", currentPC);
                LOG_INFO("Final A register: 0x%02X", (int)cpu.A);

                logFile.close();

                if (cpu.PC == 0xC66E && cpu.A == 0x00)
                {
                    LOG_INFO("SUCCESS! All tests passed.");
                    LOG_INFO("  PC = 0xC66E (expected)");
                    LOG_INFO("  A  = 0x00 (no errors)");
                    return true;
                }
                else
                {
                    LOG_ERROR("FAILED! Error code: 0x%02X", (int)cpu.A);
                    LOG_ERROR("  Expected: PC=0xC66E, A=0x00");
                    LOG_ERROR("  Got:      PC=0x%04X, A=0x%02X", currentPC, (int)cpu.A);
                    return false;
                }
            }

            // Execute one instruction
            cpu.Step();
            instructionCount++;

            // Program indicator
            if (instructionCount % 1000 == 0)
            {
                LOG_INFO("Executed %d instructions", instructionCount);
            }
        }

        logFile.close();
        LOG_INFO("\nTest did not complete within %d instructions", MAX_INSTRUCTIONS);
        LOG_INFO("  Expected: PC=0xC66E, A=0x00");
        LOG_INFO("  Got:      PC=0x%04X, A=0x%02X", currentPC, (int)cpu.A);
        LOG_INFO("  Error code in A: 0x%02X", (int)cpu.A);
        return false;
    }
};


int main(int argc, char** argv)
{
    const char* romFile = nullptr;
    if (argc >= 2)
        romFile = argv[1];
    else
        romFile = "nestest.nes";

    LOG_INFO("============================================");
    LOG_INFO("NES Test ROM Runner");
    LOG_INFO("============================================");

    NESTestRunner nestest_runner;

    if (!nestest_runner.LoadROM(romFile))
    {
        LOG_ERROR("Failed to load nestest.nes");
        return 2;
    }

    bool success = nestest_runner.Run();

    LOG_INFO("============================================");
    LOG_INFO("Comparison with golden log (nestest.log):");
    LOG_INFO("  diff nestest.log my_nestest.log");
    LOG_INFO("Or use:");
    LOG_INFO("  diff nestest.log my_nestest.log | head -20");
    LOG_INFO("============================================");

    return success ? 0 : 3;
}