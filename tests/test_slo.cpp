#include "gtest_fixture.hpp"


TEST_F(CPU6502Test, SLO_Basic)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint16_t targetAddress = 0x2000;

    // load the instruction at the program start
    loadProgram(programStart, {
        0x0F, // SLO
        uint8_t (targetAddress & 0x00FF),        // Low byte of address
        uint8_t ((targetAddress >> 8) & 0x00FF), // High byte off address
    });

    // Initial value at address
    bus.write(targetAddress, 0x40);
    // Initial value at Accumulator
    cpu.A = 0x01;

    // Act
    cpu.step(); // Execute instruction

    // Assert
    // ASL: 0x40 -> 0x80
    EXPECT_EQ(bus.read(targetAddress), 0x80);
    // ORA
    EXPECT_EQ(cpu.A, 0x81);
    // 
    EXPECT_TRUE(cpu.getFlag(CPU6502::N));
    EXPECT_FALSE(cpu.getFlag(CPU6502::Z));
}

TEST_F(CPU6502Test, SLO_Carry)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint16_t targetAddress = 0x2000;

    // load the instruction at the program start
    loadProgram(programStart, {
        0x0F, // SLO
        uint8_t (targetAddress & 0x00FF),        // Low byte of address
        uint8_t ((targetAddress >> 8) & 0x00FF), // High byte off address
    });

    // Initial value at address
    bus.write(targetAddress, 0x80);
    // Initial value at Accumulator
    cpu.A = 0x01;

    // Act
    cpu.step(); // Execute instruction

    // Assert
    // ASL: 0x80 -> 0x00
    EXPECT_EQ(bus.read(targetAddress), 0x00);
    // ORA
    EXPECT_EQ(cpu.A, 0x01);
    // 
    EXPECT_TRUE(cpu.getFlag(CPU6502::C));
    EXPECT_FALSE(cpu.getFlag(CPU6502::N));
    EXPECT_FALSE(cpu.getFlag(CPU6502::Z));
}