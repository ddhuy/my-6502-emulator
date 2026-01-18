#include "gtest_fixture.hpp"


TEST_F(CPU6502Test, SEC)
{
    // Arrange
    uint16_t programStart = 0x8000;

    // load the instruction at the program start
    loadProgram(programStart, {
        0x38, // SEC
    });

    cpu.setFlag(CPU6502::C, false);
    cpu.setFlag(CPU6502::Z, true); // should remain unchanged

    // Act
    stepInstruction(); // Execute instruction

    // Assert
    EXPECT_TRUE(cpu.getFlag(CPU6502::C));
    EXPECT_TRUE(cpu.getFlag(CPU6502::Z));
}


TEST_F(CPU6502Test, CLI)
{
    // Arrange
    uint16_t programStart = 0x8000;

    // load the instruction at the program start
    loadProgram(programStart, {
        0x58, // SEC
    });

    cpu.setFlag(CPU6502::I, true);

    // Act
    stepInstruction(); // Execute instruction

    // Assert
    EXPECT_FALSE(cpu.getFlag(CPU6502::I));
    EXPECT_EQ(cpu.cycles(), 0);
}


TEST_F(CPU6502Test, SED)
{
    // Arrange
    uint16_t programStart = 0x8000;

    // load the instruction at the program start
    loadProgram(programStart, {
        0xF8, // SEC
    });

    cpu.setFlag(CPU6502::D, false);

    // Act
    stepInstruction(); // Execute instruction

    // Assert
    EXPECT_TRUE(cpu.getFlag(CPU6502::D));
}


TEST_F(CPU6502Test, TXA_X_To_A)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint8_t value = 0x42;

    // load the instruction at the program start
    loadProgram(programStart, {
        0x8A, // TXA
    });

    cpu.X = value;

    // Act
    stepInstruction(); // Execute instruction

    // Assert
    EXPECT_EQ(cpu.A, value);
    EXPECT_FALSE(cpu.getFlag(CPU6502::Z));
    EXPECT_FALSE(cpu.getFlag(CPU6502::N));
}


TEST_F(CPU6502Test, TXA_SetZFlag)
{
    // Arrange
    uint16_t programStart = 0x8000;

    // load the instruction at the program start
    loadProgram(programStart, {
        0x8A, // TXA
    });

    cpu.X = 0x0;

    // Act
    stepInstruction(); // Execute instruction

    // Assert
    EXPECT_EQ(cpu.A, 0x0);
    EXPECT_TRUE(cpu.getFlag(CPU6502::Z));
    EXPECT_FALSE(cpu.getFlag(CPU6502::N));
}


TEST_F(CPU6502Test, TAY)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint8_t value = 0x80;

    // load the instruction at the program start
    loadProgram(programStart, {
        0xA8, // TAY
    });

    cpu.A = value;

    // Act
    stepInstruction(); // Execute instruction

    // Assert
    EXPECT_EQ(cpu.Y, value);
    EXPECT_TRUE(cpu.getFlag(CPU6502::N));
}


TEST_F(CPU6502Test, TSX)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint8_t value = 0xFD;

    // load the instruction at the program start
    loadProgram(programStart, {
        0xBA, // TSX
    });

    cpu.SP = value;

    // Act
    stepInstruction(); // Execute instruction

    // Assert
    EXPECT_EQ(cpu.X, value);
    EXPECT_FALSE(cpu.getFlag(CPU6502::Z));
}


TEST_F(CPU6502Test, TXS)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint8_t value = 0x12;

    // load the instruction at the program start
    loadProgram(programStart, {
        0x9A, // TSX
    });

    cpu.X = value;

    // Act
    stepInstruction(); // Execute instruction

    // Assert
    EXPECT_EQ(cpu.SP, value);
    EXPECT_FALSE(cpu.getFlag(CPU6502::Z));
    EXPECT_FALSE(cpu.getFlag(CPU6502::N));
}


TEST_F(CPU6502Test, PHA)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint8_t regA = 0x42;
    uint8_t regSP = 0xFD;

    // load the instruction at the program start
    loadProgram(programStart, {
        0x48, // PHA
    });

    cpu.A = regA;
    cpu.SP = regSP;

    // Act
    stepInstruction(); // Execute instruction

    // Assert
    EXPECT_EQ(bus.read(0x0100 | regSP), regA);
    EXPECT_EQ(cpu.SP, regSP - 1);
}


TEST_F(CPU6502Test, PLP_RestoreStatus)
{
    // Arrange
    uint16_t programStart = 0x8000;

    // load the instruction at the program start
    loadProgram(programStart, {
        0x28, // PLP
    });

    cpu.SP = 0xFC;
    bus.write(0x01FD, 0b11010001); // value to pull (C & N set, B set intentionally)

    // Act
    stepInstruction(); // Execute instruction

    // Assert
    EXPECT_TRUE(cpu.getFlag(CPU6502::C));
    EXPECT_TRUE(cpu.getFlag(CPU6502::N));
    EXPECT_FALSE(cpu.getFlag(CPU6502::B));  // cleared internally
    EXPECT_TRUE(cpu.getFlag(CPU6502::U));   // forced to 1
}


TEST_F(CPU6502Test, PLP_IncrementSP)
{
    // Arrange
    uint16_t programStart = 0x8000;

    // load the instruction at the program start
    loadProgram(programStart, {
        0x28, // PLP
    });

    cpu.SP = 0xFC;
    bus.write(0x01FD, 0x00);

    // Act
    stepInstruction(); // Execute instruction

    // Assert
    EXPECT_EQ(cpu.SP, 0xFD);
}


TEST_F(CPU6502Test, DEX)
{
    // Arrange
    uint16_t programStart = 0x8000;

    // load the instruction at the program start
    loadProgram(programStart, {
        0xCA, // DEX
    });

    cpu.X = 0x00;

    // Act
    stepInstruction(); // Execute instruction

    // Assert
    EXPECT_EQ(cpu.X, 0xFF);
    EXPECT_TRUE(cpu.getFlag(CPU6502::N));
}


TEST_F(CPU6502Test, INY)
{
    // Arrange
    uint16_t programStart = 0x8000;

    // load the instruction at the program start
    loadProgram(programStart, {
        0xC8, // INY
    });

    cpu.Y = 0xFF;

    // Act
    stepInstruction(); // Execute instruction

    // Assert
    EXPECT_EQ(cpu.Y, 0x00);
    EXPECT_TRUE(cpu.getFlag(CPU6502::Z));
}


TEST_F(CPU6502Test, INC)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint16_t memoryAddr = 0x9000;
    uint8_t value = 0x7F;

    // load the instruction at the program start
    loadProgram(programStart, {
        0xEE, // INC
        (uint8_t)(memoryAddr & 0xFF), // Low byte memory address
        (uint8_t)((memoryAddr >> 8) & 0xFF) // High byte memory address
    });

    bus.write(memoryAddr, value);

    // Act
    stepInstruction(); // Execute instruction

    // Assert
    EXPECT_EQ(bus.read(memoryAddr), value + 1);
    EXPECT_FALSE(cpu.getFlag(CPU6502::Z));
    EXPECT_TRUE(cpu.getFlag(CPU6502::N));
}


TEST_F(CPU6502Test, DEC)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint16_t memoryAddr = 0x9000;
    uint8_t value = 0x01;

    // load the instruction at the program start
    loadProgram(programStart, {
        0xCE, // DEC
        (uint8_t)(memoryAddr & 0xFF), // Low byte memory address
        (uint8_t)((memoryAddr >> 8) & 0xFF) // High byte memory address
    });

    bus.write(memoryAddr, value);

    // Act
    stepInstruction(); // Execute instruction

    // Assert
    EXPECT_EQ(bus.read(memoryAddr), value - 1);
    EXPECT_TRUE(cpu.getFlag(CPU6502::Z));
}

TEST_F(CPU6502Test, ORA)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint8_t value1 = 0x0F;
    uint8_t value2 = 0xF0;

    // load the instruction at the program start
    loadProgram(programStart, {
        0x09, // ORA
        value2
    });

    // Set initial accumulator value
    cpu.A = value1;

    // Act
    stepInstruction(); // Execute instruction

    // Assert
    EXPECT_EQ(cpu.A, value1 | value2);
    EXPECT_TRUE(cpu.getFlag(CPU6502::N));
    EXPECT_FALSE(cpu.getFlag(CPU6502::Z));
}