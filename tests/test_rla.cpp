#include "gtest_fixture.hpp"


TEST_F(CPU6502Test, RLA_Absolute)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint16_t targetAddress = 0x2000;

    // load the instruction at the program start
    loadProgram(programStart, {
        0x2F, // RLA Absolute opcode
        uint8_t (targetAddress & 0x00FF), // Low byte of address
        uint8_t ((targetAddress >> 8) & 0x00FF), // High byte off address
    });

    // Initial value at address
    bus.write(targetAddress, 0b00001111);
    // Other value in Accumulator
    cpu.A = 0b11110000;
    // Set Carry flag
    cpu.setFlag(CPU6502::C, true); 

    // Act
    cpu.step(); // Execute RLA instruction

    // Assert
    EXPECT_EQ(bus.read(targetAddress), 0b00011111); // Check rotated value in memory
    EXPECT_EQ(cpu.A, 0b00010000); // Check AND result in Accumulator
    EXPECT_FALSE(cpu.getFlag(CPU6502::C)); // Check Carry flag
    EXPECT_FALSE(cpu.getFlag(CPU6502::Z)); // Check Zero flag
    EXPECT_FALSE(cpu.getFlag(CPU6502::N)); // Check Negative flag
}

TEST_F(CPU6502Test, RLA_CarryAndZero)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint16_t targetAddress = 0x2000;

    // load the instruction at the program start
    loadProgram(programStart, {
        0x2F, // RLA Absolute opcode
        uint8_t (targetAddress & 0x00FF), // Low byte of address
        uint8_t ((targetAddress >> 8) & 0x00FF), // High byte off address
    });

    // Initial value at address
    bus.write(targetAddress, 0x80);
    // Other value in Accumulator
    cpu.A = 0x1;
    // Set Carry flag
    cpu.setFlag(CPU6502::C, false); 

    // Act
    cpu.step(); // Execute RLA instruction

    // Assert
    EXPECT_EQ(bus.read(targetAddress), 0x00); // Check rotated value in memory
    EXPECT_EQ(cpu.A, 0x00); // Check AND result in Accumulator
    EXPECT_TRUE(cpu.getFlag(CPU6502::C)); // Check Carry flag
    EXPECT_TRUE(cpu.getFlag(CPU6502::Z)); // Check Zero flag
    EXPECT_FALSE(cpu.getFlag(CPU6502::N)); // Check Negative flag
}