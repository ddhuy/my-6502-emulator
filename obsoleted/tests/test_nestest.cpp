#include <fstream>
#include <iostream>
#include <vector>

#include "gtest/gtest.h"
#include "cpu/CPU6502.hpp"
#include "cpu/Instructions.hpp"
#include "bus/Bus.hpp"
#include "mem/RAM.hpp"


class Nestest : public ::testing::Test
{
protected:
    RAM ram;
    Bus bus;
    CPU6502 cpu;

    std::ofstream trace;

    void SetUp() override
    {
        bus.attachMemory(&ram);
        cpu.connectBus(&bus);
    }

    void TearDown() override
    {
        if (trace.is_open())
            trace.close();
    }

    bool loadROM(const std::string& filename)
    {
        // Check if the NES file exists
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open())
        {
            std::cerr << "Failed to open ROM file: " << filename << std::endl;
            return false;
        }

        // Read the entire file into rom
        std::vector<uint8_t> rom((std::istreambuf_iterator<char>(file)),
                                  std::istreambuf_iterator<char>());
        file.close();

        // NES ROM format:
        // Bytes 0-15: Header (16 bytes)
        // Bytes 16+: PRG ROM data
        if (rom.size() < 16)
        {
            std::cerr << "Invalid ROM size: " << rom.size() << " bytes." << std::endl;
            return false;
        }

        // Check for "NES" magic number
        if (rom[0] != 'N' || rom[1] != 'E' || rom[2] != 'S' || rom[3] != 0x1A)
        {
            std::cerr << "Not a valid NES file: missing NES header." << std::endl;
            return false;
        }
        // Get PRG ROM size
        uint8_t prgRomSize = rom[4]; // in 16KB units

        // Load PRG ROM into memory at $8000-$BFFF and $C000-$FFFF (mirrored)
        size_t prgRomStart = 16; // PRG ROM starts after 16-byte header
        size_t prgRomBytes = prgRomSize * 16384;

        if (prgRomSize == 1) {
            // Single 16KB PRG ROM, mirror it
            for (size_t i = 0; i < prgRomBytes; ++i) {
                bus.write(0x8000 + i, rom[prgRomStart + i]);
                bus.write(0xC000 + i, rom[prgRomStart + i]); // mirror
            }
        } else {
            // Multiple banks
            for (size_t i = 0; i < prgRomBytes && i < 32768; ++i) {
                bus.write(0x8000 + i, rom[prgRomStart + i]);
            }
        }

        return true;
    }

    bool runTest(const std::string& logFilename)
    {
        // ----------------------------
        // Oveerride reset vector
        // ----------------------------
        bus.write(0xFFFC, 0x00);
        bus.write(0xFFFD, 0xC0);
        cpu.reset();
        // Drain any initial cycles from reset
        while (!cpu.complete())
            cpu.clock();

        // ----------------------------
        // Open trace file
        // ----------------------------
        trace.open(logFilename);
        if (!trace.is_open())
        {
            std::cerr << "Failed to open trace output file." << std::endl;
            return false;
        }

        // ----------------------------
        // Run until known end PC
        // ----------------------------
        const uint16_t endPC = 0xC66E; // Known end of nestest program
        while (cpu.PC != endPC)
        {
            // Trace line
            trace << formatTraceLine();
            // Step CPU
            cpu.step();
        }

        std::cout << "Nestest completed successfully: " << cpu.totalCycles() << " cycles." << std::endl;

        return true;
    }
    
private:
    std::string formatTraceLine()
    {
        // Disassemble operands for trace
        const auto& inst = instructionTable[cpu.getOpcode()];
        std::string ops = cpu.disassembleOperands();
        // Format the trace line        
        char buffer[256];
        snprintf(buffer, sizeof(buffer),
                 "%04X  %02X %02X %02X  %-4s %-28s A:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%lu\n",
                 cpu.PC,
                 cpu.getOpcode(),
                 bus.read(cpu.PC),
                 bus.read(cpu.PC + 1),
                 inst.name,
                 ops.c_str(),
                 cpu.A,
                 cpu.X,
                 cpu.Y,
                 cpu.P,
                 cpu.SP,
                 cpu.totalCycles());
        return std::string(buffer);
    }
};


TEST_F(Nestest, LoadROM)
{
    ASSERT_TRUE(loadROM("tests/nestest/nestest.nes")) << "Failed to load nestest.nes ROM.";
}


TEST_F(Nestest, FullTest)
{
    ASSERT_TRUE(loadROM("tests/nestest/nestest.nes")) << "Failed to load nestest.nes ROM.";
    ASSERT_TRUE(runTest("build/nestest.log")) << "Failed to run nestest.";
}


int main()
{
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}