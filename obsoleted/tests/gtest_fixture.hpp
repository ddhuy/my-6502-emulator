#include <vector>
#include "gtest/gtest.h"
#include "cpu/CPU6502.hpp"
#include "bus/Bus.hpp"
#include "mem/RAM.hpp"


class InstructionTest : public ::testing::Test
{
protected:
    RAM ram;
    Bus bus;
    CPU6502 cpu;

    void SetUp() override {
        bus.attachMemory(&ram);
        cpu.connectBus(&bus);
        cpu.reset();

        // drain the reset cycles
        while (cpu.cycles() > 0) {
            cpu.clock();
        }
    }

    // Helper: load a program at a given address
    void loadProgram(uint16_t start, const std::vector<uint8_t>& bytes)
    {
        for (size_t i = 0; i < bytes.size(); i++) {
            bus.write(start + i, bytes[i]);
        }
        cpu.PC = start;
    }

    // Helper: execute one full instruction
    void stepInstruction()
    {
        cpu.step();
    }
};


int main()
{
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}