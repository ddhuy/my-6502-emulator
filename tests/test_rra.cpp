#include "gtest_fixture.hpp"


TEST_F(CPU6502Test, RRA_BASIC)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint16_t targetAddress = 0x9000;

    // load the instruction at the program start
    loadProgram(programStart, {
        0x6F, // RRA
        uint8_t (targetAddress & 0x00FF),        // Low byte of address
        uint8_t ((targetAddress >> 8) & 0x00FF), // High byte off address
    });

    // Initial value at address
    bus.write(targetAddress, 0x02);
    // Initial value at Accumulator
    cpu.A = 0x10;

    // Act
    cpu.step(); // Execute instruction

    // Assert
    // ROR
    EXPECT_EQ(bus.read(targetAddress), 0x01);
    // ADC
    EXPECT_EQ(cpu.A, 0x11);
    //
    EXPECT_FALSE(cpu.getFlag(CPU6502::C));
    EXPECT_FALSE(cpu.getFlag(CPU6502::N));
    EXPECT_FALSE(cpu.getFlag(CPU6502::Z));
    EXPECT_FALSE(cpu.getFlag(CPU6502::V));
}

TEST_F(CPU6502Test, RRA_CarryOverflow)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint16_t targetAddress = 0x9000;

    // load the instruction at the program start
    loadProgram(programStart, {
        0x6F, // RRA
        uint8_t (targetAddress & 0x00FF),        // Low byte of address
        uint8_t ((targetAddress >> 8) & 0x00FF), // High byte off address
    });

    // Initial value at address
    bus.write(targetAddress, 0x01);
    // Initial value at Accumulator
    cpu.A = 0x8F;
    // Set Carry flag
    cpu.setFlag(CPU6502::C, true); 

    // Act
    cpu.step(); // Execute instruction

    // Assert
    // ROR
    EXPECT_EQ(bus.read(targetAddress), 0x80);
    // ADC
    EXPECT_EQ(cpu.A, 0x10);
    //
    EXPECT_TRUE(cpu.getFlag(CPU6502::C));
    EXPECT_FALSE(cpu.getFlag(CPU6502::N));
    EXPECT_FALSE(cpu.getFlag(CPU6502::Z));
    EXPECT_TRUE(cpu.getFlag(CPU6502::V));
}