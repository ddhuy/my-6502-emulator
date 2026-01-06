#include "gtest_fixture.hpp"


TEST_F(CPU6502Test, DCP_ABS_A_Greater)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint16_t targetAddress = 0x1234;
    uint8_t initialMemoryValue = 0x05;
    uint8_t accumulatorValue = 0x07;

    // Load the DCP instruction at the program start
    loadProgram(programStart, {
        0xCF,               // DCP ABS opcode
        uint8_t (targetAddress & 0x00FF),       // Low byte of address
        uint8_t ((targetAddress >> 8) & 0x00FF) // High byte of address
    });

    // Write the initial value into the target memory address
    bus.write(targetAddress, initialMemoryValue);

    // Set the Accumulator
    cpu.A = accumulatorValue;

    // Act
    stepInstruction(); // Execute DCP instruction

    // Assert
    uint8_t expectedMemoryValue = initialMemoryValue - 1;
    uint8_t memoryValue = bus.read(targetAddress);
    EXPECT_EQ(memoryValue, expectedMemoryValue) << "Memory at target address should be decremented.";

    EXPECT_EQ(cpu.getFlag(CPU6502::C), true) << "Carry flag should be set when A >= memory value.";
    EXPECT_EQ(cpu.getFlag(CPU6502::Z), false) << "Zero flag should not be set when A != memory value.";
    EXPECT_EQ(cpu.getFlag(CPU6502::N), false) << "Negative flag should not be set when result is positive.";
}

TEST_F(CPU6502Test, DCP_ABS_A_Less)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint16_t targetAddress = 0x1234;
    uint8_t initialMemoryValue = 0x05;
    uint8_t accumulatorValue = 0x03;

    // Load the DCP instruction at the program start
    loadProgram(programStart, {
        0xCF,               // DCP ABS opcode
        uint8_t (targetAddress & 0x00FF),       // Low byte of address
        uint8_t ((targetAddress >> 8) & 0x00FF) // High byte of address
    });

    // Write the initial value into the target memory address
    bus.write(targetAddress, initialMemoryValue);

    // Set the Accumulator
    cpu.A = accumulatorValue;

    // Act
    stepInstruction(); // Execute DCP instruction

    // Assert
    uint8_t expectedMemoryValue = initialMemoryValue - 1;
    uint8_t memoryValue = bus.read(targetAddress);
    EXPECT_EQ(memoryValue, expectedMemoryValue) << "Memory at target address should be decremented.";

    EXPECT_EQ(cpu.getFlag(CPU6502::C), false) << "Carry flag should be clear when A < memory value.";
    EXPECT_EQ(cpu.getFlag(CPU6502::Z), false) << "Zero flag should not be set when A != memory value.";
    EXPECT_EQ(cpu.getFlag(CPU6502::N), true) << "Negative flag should be set when result is negative.";
}

TEST_F(CPU6502Test, DCP_ABS_A_Equal)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint16_t targetAddress = 0x1234;
    uint8_t initialMemoryValue = 0x05;
    uint8_t accumulatorValue = 0x04;

    // Load the DCP instruction at the program start
    loadProgram(programStart, {
        0xCF,               // DCP ABS opcode
        uint8_t (targetAddress & 0x00FF),       // Low byte of address
        uint8_t ((targetAddress >> 8) & 0x00FF) // High byte of address
    });

    // Write the initial value into the target memory address
    bus.write(targetAddress, initialMemoryValue);

    // Set the Accumulator
    cpu.A = accumulatorValue;

    // Act
    stepInstruction(); // Execute DCP instruction

    // Assert
    uint8_t expectedMemoryValue = initialMemoryValue - 1;
    uint8_t memoryValue = bus.read(targetAddress);
    EXPECT_EQ(memoryValue, expectedMemoryValue) << "Memory at target address should be decremented.";

    EXPECT_EQ(cpu.getFlag(CPU6502::C), true) << "Carry flag should be set when A >= memory value.";
    EXPECT_EQ(cpu.getFlag(CPU6502::Z), true) << "Zero flag should be set when A == memory value.";
    EXPECT_EQ(cpu.getFlag(CPU6502::N), false) << "Negative flag should not be set when result is zero.";
}