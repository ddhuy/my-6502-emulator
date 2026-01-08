#include "gtest_fixture.hpp"


TEST_F(CPU6502Test, SRE_BASIC)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint16_t targetAddress = 0x9000;

    // load the instruction at the program start
    loadProgram(programStart, {
        0x4F, // SRE
        uint8_t (targetAddress & 0x00FF),        // Low byte of address
        uint8_t ((targetAddress >> 8) & 0x00FF), // High byte off address
    });

    // Initial value at address
    bus.write(targetAddress, 0x0F);
    // Initial value at Accumulator
    cpu.A = 0xFF;

    // Act
    cpu.step(); // Execute instruction

    // Assert
    // LSR
    EXPECT_EQ(bus.read(targetAddress), 0x07);
    // EOR
    EXPECT_EQ(cpu.A, 0xF8);
    //
    EXPECT_TRUE(cpu.getFlag(CPU6502::C));
    EXPECT_TRUE(cpu.getFlag(CPU6502::N));
    EXPECT_FALSE(cpu.getFlag(CPU6502::Z));
    EXPECT_FALSE(cpu.getFlag(CPU6502::V));
}

TEST_F(CPU6502Test, SRE_Carry)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint16_t targetAddress = 0x9000;

    // load the instruction at the program start
    loadProgram(programStart, {
        0x4F, // SRE
        uint8_t (targetAddress & 0x00FF),        // Low byte of address
        uint8_t ((targetAddress >> 8) & 0x00FF), // High byte off address
    });

    // Initial value at address
    bus.write(targetAddress, 0x01);
    // Initial value at Accumulator
    cpu.A = 0x00;

    // Act
    cpu.step(); // Execute instruction

    // Assert
    // LSR
    EXPECT_EQ(bus.read(targetAddress), 0x00);
    // EOR
    EXPECT_EQ(cpu.A, 0x00);
    //
    EXPECT_TRUE(cpu.getFlag(CPU6502::C));
    EXPECT_FALSE(cpu.getFlag(CPU6502::N));
    EXPECT_TRUE(cpu.getFlag(CPU6502::Z));
    EXPECT_FALSE(cpu.getFlag(CPU6502::V));
}