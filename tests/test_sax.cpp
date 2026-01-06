#include "gtest_fixture.hpp"


TEST_F(CPU6502Test, SAX_ABS_LoadsAandXWithMemoryValue)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint16_t targetAddress = 0x1234;
    uint8_t valueA = 0x42;
    uint8_t valueX = 0x37;
    uint8_t status = cpu.P; // Save initial status register

    // Load the SAX instruction at the program start
    loadProgram(programStart, {
        0x8F,               // SAX ABS opcode
        uint8_t (targetAddress & 0x00FF),       // Low byte of address
        uint8_t ((targetAddress >> 8) & 0x00FF) // High byte of address
    });

    // Set the A and X registers
    cpu.A = valueA;
    cpu.X = valueX;

    // Act
    stepInstruction(); // Execute SAX instruction

    // Assert
    uint8_t expectedValue = valueA & valueX;
    uint8_t memoryValue = bus.read(targetAddress);
    EXPECT_EQ(memoryValue, expectedValue) << "Memory at target address should contain A & X.";
    EXPECT_EQ(cpu.P, status) << "Status register should remain unchanged.";
}

/*
TEST_F(CPU6502Test, SAX_ZPX_LoadsAandXWithMemoryValue)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint8_t zeroPageAddress = 0x40;
    uint8_t xOffset = 0x05;
    uint16_t targetAddress = (zeroPageAddress + xOffset) & 0x00FF; // Wrap around zero page
    uint8_t valueA = 0x5A;
    uint8_t valueX = 0x3C;

    // Load the SAX instruction at the program start
    loadProgram(programStart, {
        0x83,               // SAX ZPX opcode
        zeroPageAddress     // Zero page base address
    });

    // Set the A and X registers
    cpu.A = valueA;
    cpu.X = valueX;

    // Act
    stepInstruction(); // Execute SAX instruction

    // Assert
    uint8_t expectedValue = valueA & valueX;
    uint8_t memoryValue = bus.read(targetAddress);
    EXPECT_EQ(memoryValue, expectedValue) << "Memory at target address should contain A & X.";
}

TEST_F(CPU6502Test, SAX_ZPX_WrapsAroundZeroPage)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint8_t zeroPageAddress = 0xFF; // Edge of zero page
    uint8_t xOffset = 0x02;         // This will cause wrap-around
    uint16_t targetAddress = (zeroPageAddress + xOffset) & 0x00FF; // Wrap around zero page
    uint8_t valueA = 0xAA;
    uint8_t valueX = 0x55;

    // Load the SAX instruction at the program start
    loadProgram(programStart, {
        0x83,               // SAX ZPX opcode
        zeroPageAddress     // Zero page base address
    });

    // Set the A and X registers
    cpu.A = valueA;
    cpu.X = valueX;

    // Act
    stepInstruction(); // Execute SAX instruction

    // Assert
    uint8_t expectedValue = valueA & valueX;
    uint8_t memoryValue = bus.read(targetAddress);
    EXPECT_EQ(memoryValue, expectedValue) << "Memory at wrapped-around target address should contain A & X.";
}
*/
