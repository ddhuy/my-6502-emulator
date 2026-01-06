#include "gtest_fixture.hpp"


TEST_F(CPU6502Test, ISC_ABS_NoBorrow)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint16_t targetAddress = 0x1234;
    uint8_t initialMemoryValue = 0x05;
    uint8_t accumulatorValue = 0x10;

    // Load the ISC instruction at the program start
    loadProgram(programStart, {
        0xEF,               // ISC ABS opcode
        uint8_t (targetAddress & 0x00FF),       // Low byte of address
        uint8_t ((targetAddress >> 8) & 0x00FF) // High byte of address
    });

    // Write the initial value into the target memory address
    bus.write(targetAddress, initialMemoryValue);

    // Set the Accumulator
    cpu.A = accumulatorValue;
    // Set the Carry flag to indicate no borrow
    cpu.setFlag(CPU6502::C, true);

    // Act
    stepInstruction(); // Execute ISC instruction

    // Assert
    uint8_t expectedMemoryValue = initialMemoryValue + 1;
    uint8_t memoryValue = bus.read(targetAddress);
    EXPECT_EQ(memoryValue, expectedMemoryValue) << "Memory at target address should be incremented.";

    uint8_t expectedResult = accumulatorValue - expectedMemoryValue;
    EXPECT_EQ(cpu.A, expectedResult) << "Accumulator should contain the result of A - memory.";

    EXPECT_EQ(cpu.getFlag(CPU6502::C), true) << "Carry flag should be set when no borrow occurs.";
    EXPECT_EQ(cpu.getFlag(CPU6502::Z), expectedResult == 0) << "Zero flag should be set if result is zero.";
    EXPECT_EQ(cpu.getFlag(CPU6502::N), (expectedResult & 0x80) != 0) << "Negative flag should reflect sign of result.";
    EXPECT_EQ(cpu.getFlag(CPU6502::V), false) << "Overflow flag should not be set when no signed overflow occurs.";
}

TEST_F(CPU6502Test, ISC_ABS_WithBorrow)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint16_t targetAddress = 0x1234;
    uint8_t initialMemoryValue = 0x10;
    uint8_t accumulatorValue = 0x05;

    // Load the ISC instruction at the program start
    loadProgram(programStart, {
        0xEF,               // ISC ABS opcode
        uint8_t (targetAddress & 0x00FF),       // Low byte of address
        uint8_t ((targetAddress >> 8) & 0x00FF) // High byte of address
    });

    // Write the initial value into the target memory address
    bus.write(targetAddress, initialMemoryValue);
    // Set the Carry flag to indicate no borrow
    cpu.setFlag(CPU6502::C, true);

    // Set the Accumulator
    cpu.A = accumulatorValue;

    // Act
    stepInstruction(); // Execute ISC instruction

    // Assert
    uint8_t expectedMemoryValue = initialMemoryValue + 1;
    uint8_t memoryValue = bus.read(targetAddress);
    EXPECT_EQ(memoryValue, expectedMemoryValue) << "Memory at target address should be incremented.";

    uint8_t expectedResult = accumulatorValue - expectedMemoryValue;
    EXPECT_EQ(cpu.A, expectedResult) << "Accumulator should contain the result of A - memory.";

    EXPECT_EQ(cpu.getFlag(CPU6502::C), false) << "Carry flag should be clear when borrow occurs.";
    EXPECT_EQ(cpu.getFlag(CPU6502::Z), expectedResult == 0) << "Zero flag should be set if result is zero.";
    EXPECT_EQ(cpu.getFlag(CPU6502::N), (expectedResult & 0x80) != 0) << "Negative flag should reflect sign of result.";
    EXPECT_EQ(cpu.getFlag(CPU6502::V), false) << "Overflow flag should not be set when no signed overflow occurs.";
}

TEST_F(CPU6502Test, ISC_ABS_WithBorrowIn)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint16_t targetAddress = 0x1234;
    uint8_t initialMemoryValue = 0x20;
    uint8_t accumulatorValue = 0x10;

    // Load the ISC instruction at the program start
    loadProgram(programStart, {
        0xEF,               // ISC ABS opcode
        uint8_t (targetAddress & 0x00FF),       // Low byte of address
        uint8_t ((targetAddress >> 8) & 0x00FF) // High byte of address
    });

    // Write the initial value into the target memory address
    bus.write(targetAddress, initialMemoryValue);
    // Clear the Carry flag to indicate borrow in
    cpu.setFlag(CPU6502::C, false);

    // Set the Accumulator
    cpu.A = accumulatorValue;

    // Act
    stepInstruction(); // Execute ISC instruction

    // Assert
    uint8_t expectedMemoryValue = initialMemoryValue + 1;
    uint8_t memoryValue = bus.read(targetAddress);
    EXPECT_EQ(memoryValue, expectedMemoryValue) << "Memory at target address should be incremented.";

    uint8_t expectedResult = accumulatorValue - expectedMemoryValue - 1; // Account for borrow in
    EXPECT_EQ(cpu.A, expectedResult) << "Accumulator should contain the result of A - memory.";

    EXPECT_EQ(cpu.getFlag(CPU6502::C), false) << "Carry flag should be clear when borrow occurs.";
    EXPECT_EQ(cpu.getFlag(CPU6502::Z), expectedResult == 0) << "Zero flag should be set if result is zero.";
    EXPECT_EQ(cpu.getFlag(CPU6502::N), (expectedResult & 0x80) != 0) << "Negative flag should reflect sign of result.";
    EXPECT_EQ(cpu.getFlag(CPU6502::V), false) << "Overflow flag should not be set when no signed overflow occurs.";
}

TEST_F(CPU6502Test, ISC_ABS_ResultZero)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint16_t targetAddress = 0x1234;
    uint8_t initialMemoryValue = 0x0A;
    uint8_t accumulatorValue = 0x0B;

    // Load the ISC instruction at the program start
    loadProgram(programStart, {
        0xEF,               // ISC ABS opcode
        uint8_t (targetAddress & 0x00FF),       // Low byte of address
        uint8_t ((targetAddress >> 8) & 0x00FF) // High byte of address
    });

    // Write the initial value into the target memory address
    bus.write(targetAddress, initialMemoryValue);

    // Set the Accumulator
    cpu.A = accumulatorValue;
    // Set the Carry flag to indicate no borrow
    cpu.setFlag(CPU6502::C, true);

    // Act
    stepInstruction(); // Execute ISC instruction

    // Assert
    uint8_t expectedMemoryValue = initialMemoryValue + 1;
    uint8_t memoryValue = bus.read(targetAddress);
    EXPECT_EQ(memoryValue, expectedMemoryValue) << "Memory at target address should be incremented.";

    uint8_t expectedResult = accumulatorValue - expectedMemoryValue;
    EXPECT_EQ(cpu.A, expectedResult) << "Accumulator should contain the result of A - memory.";

    EXPECT_EQ(cpu.getFlag(CPU6502::C), true) << "Carry flag should be set when no borrow occurs.";
    EXPECT_EQ(cpu.getFlag(CPU6502::Z), true) << "Zero flag should be set when result is zero.";
    EXPECT_EQ(cpu.getFlag(CPU6502::N), false) << "Negative flag should not be set when result is zero.";
    EXPECT_EQ(cpu.getFlag(CPU6502::V), false) << "Overflow flag should not be set when no signed overflow occurs.";
}

TEST_F(CPU6502Test, ISC_ABS_ResultNegative)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint16_t targetAddress = 0x1234;
    uint8_t initialMemoryValue = 0x20;
    uint8_t accumulatorValue = 0x10;

    // Load the ISC instruction at the program start
    loadProgram(programStart, {
        0xEF,               // ISC ABS opcode
        uint8_t (targetAddress & 0x00FF),       // Low byte of address
        uint8_t ((targetAddress >> 8) & 0x00FF) // High byte of address
    });

    // Write the initial value into the target memory address
    bus.write(targetAddress, initialMemoryValue);

    // Set the Accumulator
    cpu.A = accumulatorValue;
    // Set the Carry flag to indicate no borrow
    cpu.setFlag(CPU6502::C, true);

    // Act
    stepInstruction(); // Execute ISC instruction

    // Assert
    uint8_t expectedMemoryValue = initialMemoryValue + 1;
    uint8_t memoryValue = bus.read(targetAddress);
    EXPECT_EQ(memoryValue, expectedMemoryValue) << "Memory at target address should be incremented.";

    uint8_t expectedResult = accumulatorValue - expectedMemoryValue;
    EXPECT_EQ(cpu.A, expectedResult) << "Accumulator should contain the result of A - memory.";

    EXPECT_EQ(cpu.getFlag(CPU6502::C), false) << "Carry flag should be clear when borrow occurs.";
    EXPECT_EQ(cpu.getFlag(CPU6502::Z), false) << "Zero flag should not be set when result is non-zero.";
    EXPECT_EQ(cpu.getFlag(CPU6502::N), true) << "Negative flag should be set when result is negative.";
    EXPECT_EQ(cpu.getFlag(CPU6502::V), false) << "Overflow flag should not be set when no signed overflow occurs.";
}

TEST_F(CPU6502Test, ISC_ABS_Overflow)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint16_t targetAddress = 0x1234;
    uint8_t initialMemoryValue = 0x80; // 128 in decimal
    uint8_t accumulatorValue = 0x7F;   // 127 in decimal

    // Load the ISC instruction at the program start
    loadProgram(programStart, {
        0xEF,               // ISC ABS opcode
        uint8_t (targetAddress & 0x00FF),       // Low byte of address
        uint8_t ((targetAddress >> 8) & 0x00FF) // High byte of address
    });

    // Write the initial value into the target memory address
    bus.write(targetAddress, initialMemoryValue);

    // Set the Accumulator
    cpu.A = accumulatorValue;
    // Set the Carry flag to indicate no borrow
    cpu.setFlag(CPU6502::C, true);

    // Act
    stepInstruction(); // Execute ISC instruction

    // Assert
    uint8_t expectedMemoryValue = initialMemoryValue + 1;
    uint8_t memoryValue = bus.read(targetAddress);
    EXPECT_EQ(memoryValue, expectedMemoryValue) << "Memory at target address should be incremented.";

    uint8_t expectedResult = accumulatorValue - expectedMemoryValue;
    EXPECT_EQ(cpu.A, expectedResult) << "Accumulator should contain the result of A - memory.";

    EXPECT_EQ(cpu.getFlag(CPU6502::C), false) << "Carry flag should be clear when borrow occurs.";
    EXPECT_EQ(cpu.getFlag(CPU6502::Z), false) << "Zero flag should not be set when result is non-zero.";
    EXPECT_EQ(cpu.getFlag(CPU6502::N), true) << "Negative flag should be set when result is negative.";
    EXPECT_EQ(cpu.getFlag(CPU6502::V), true) << "Overflow flag should be set on signed overflow.";
}
