#include "gtest_fixture.hpp"


TEST_F(CPU6502Test, LAX_ABS_LoadsAandXWithMemoryValue)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint16_t targetAddress = 0x1234;
    uint8_t valueToLoad = 0x42;

    // Load the LAX instruction at the program start
    loadProgram(programStart, {
        0xAF,               // LAX ABS opcode
        uint8_t (targetAddress & 0x00FF),       // Low byte of address
        uint8_t ((targetAddress >> 8) & 0x00FF) // High byte of address
    });

    // Write the value to be loaded into the target memory address
    bus.write(targetAddress, valueToLoad);

    // Act
    stepInstruction(); // Execute LAX instruction

    // Assert
    EXPECT_EQ(cpu.A, valueToLoad) << "Accumulator should be loaded with the value from memory.";
    EXPECT_EQ(cpu.X, valueToLoad) << "X register should be loaded with the value from memory.";
    EXPECT_EQ(cpu.getFlag(CPU6502::Z), false) << "Zero flag should not be set.";
    EXPECT_EQ(cpu.getFlag(CPU6502::N), false) << "Negative flag should not be set.";
}