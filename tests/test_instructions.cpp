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


TEST_F(CPU6502Test, A)
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


TEST_F(CPU6502Test, PHP)
{
    // Arrange
    uint16_t programStart = 0x8000;

    // load the instruction at the program start
    loadProgram(programStart, {
        0x08, // PHP
    });

    cpu.setFlag(CPU6502::C, true);
    cpu.setFlag(CPU6502::Z, false);
    cpu.setFlag(CPU6502::I, true);
    cpu.setFlag(CPU6502::D, false);
    cpu.setFlag(CPU6502::B, false);
    cpu.setFlag(CPU6502::U, false);
    cpu.setFlag(CPU6502::V, false);
    cpu.setFlag(CPU6502::N, true);

    // Act
    stepInstruction(); // Execute instruction

    // Assert
    EXPECT_EQ(bus.read(0x0100 | (cpu.SP + 1)), cpu.P | CPU6502::B | CPU6502::U);
    EXPECT_EQ(cpu.SP, 0xFC);
}


TEST_F(CPU6502Test, PLA)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint8_t value = 0x55;

    // load the instruction at the program start
    loadProgram(programStart, {
        0x68, // PLA
    });

    cpu.SP = 0xFC;
    bus.write(0x01FD, value); // value to pull

    // Act
    stepInstruction(); // Execute instruction

    // Assert
    EXPECT_EQ(cpu.A, value);
    EXPECT_FALSE(cpu.getFlag(CPU6502::Z));
    EXPECT_FALSE(cpu.getFlag(CPU6502::N));
}


TEST_F(CPU6502Test, PLA_SetZFlag)
{
    // Arrange
    uint16_t programStart = 0x8000;

    // load the instruction at the program start
    loadProgram(programStart, {
        0x68, // PLA
    });

    cpu.SP = 0xFC;
    bus.write(0x01FD, 0x00); // value to pull

    // Act
    stepInstruction(); // Execute instruction

    // Assert
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_TRUE(cpu.getFlag(CPU6502::Z));
    EXPECT_FALSE(cpu.getFlag(CPU6502::N));
}


TEST_F(CPU6502Test, PLA_SetNFlag)
{
    // Arrange
    uint16_t programStart = 0x8000;

    // load the instruction at the program start
    loadProgram(programStart, {
        0x68, // PLA
    });

    cpu.SP = 0xFC;
    bus.write(0x01FD, 0x80); // value to pull

    // Act
    stepInstruction(); // Execute instruction

    // Assert
    EXPECT_EQ(cpu.A, 0x80);
    EXPECT_FALSE(cpu.getFlag(CPU6502::Z));
    EXPECT_TRUE(cpu.getFlag(CPU6502::N));
}


TEST_F(CPU6502Test, CLC)
{
    // Arrange
    uint16_t programStart = 0x8000;

    // load the instruction at the program start
    loadProgram(programStart, {
        0x18, // CLC
    });

    cpu.setFlag(CPU6502::C, true);

    // Act
    stepInstruction(); // Execute instruction

    // Assert
    EXPECT_FALSE(cpu.getFlag(CPU6502::C));
}


TEST_F(CPU6502Test, CLV)
{
    // Arrange
    uint16_t programStart = 0x8000;

    // load the instruction at the program start
    loadProgram(programStart, {
        0xB8, // CLV
    });

    cpu.setFlag(CPU6502::V, true);

    // Act
    stepInstruction(); // Execute instruction

    // Assert
    EXPECT_FALSE(cpu.getFlag(CPU6502::V));
}


TEST_F(CPU6502Test, CLD)
{
    // Arrange
    uint16_t programStart = 0x8000;

    // load the instruction at the program start
    loadProgram(programStart, {
        0xD8, // CLD
    });

    cpu.setFlag(CPU6502::D, true);

    // Act
    stepInstruction(); // Execute instruction

    // Assert
    EXPECT_FALSE(cpu.getFlag(CPU6502::D));
}


TEST_F(CPU6502Test, SEI)
{
    // Arrange
    uint16_t programStart = 0x8000;

    // load the instruction at the program start
    loadProgram(programStart, {
        0x78, // SEI
    });

    cpu.setFlag(CPU6502::I, false);

    // Act
    stepInstruction(); // Execute instruction

    // Assert
    EXPECT_TRUE(cpu.getFlag(CPU6502::I));
}


TEST_F(CPU6502Test, TYA)
{
    // Arrange
    uint16_t programStart = 0x8000;
    uint8_t value = 0x85;

    // load the instruction at the program start
    loadProgram(programStart, {
        0x98, // TYA
    });

    cpu.Y = value;

    // Act
    stepInstruction(); // Execute instruction

    // Assert
    EXPECT_EQ(cpu.A, value);
    EXPECT_FALSE(cpu.getFlag(CPU6502::Z));
    EXPECT_TRUE(cpu.getFlag(CPU6502::N));
}


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


TEST_F(CPU6502Test, BRK_PushesStateAndJumpsToIRQVector_1)
{
    // Arrange
    uint16_t programStart = 0x8000;
    // Set IRQ vector to 0x9000
    bus.write(0xFFFE, 0x00);
    bus.write(0xFFFF, 0x90);

    // Load BRK instruction at current PC
    loadProgram(programStart, {
        0x00,  // BRK opcode
        0xEA   // NOP opcode to follow BRK
    });

    // Act
    stepInstruction();

    // Assert
    EXPECT_TRUE(cpu.getFlag(CPU6502::I)); // Interrupt Disable flag should be set

    // Check that return address and Status were pushed to stack
    // SP was decremented after pushes
    uint8_t status = bus.read(0x0100 + cpu.SP + 1); 
    uint16_t addr_lo = bus.read(0x0100 + cpu.SP + 2);
    uint16_t addr_hi = bus.read(0x0100 + cpu.SP + 3);

    EXPECT_EQ((addr_hi << 8) | addr_lo, 0x8002); // Check return address
    EXPECT_TRUE(status & CPU6502::B); // Status should have Break flag set
}


TEST_F(CPU6502Test, BRK_PushesStateAndJumpsToIRQVector_2)
{
    loadProgram(0x8000, {0x00,  // BRK opcode
                         0xEA   // NOP opcode to follow BRK
                        });

    // Set IRQ vector to 0x9000
    bus.write(0xFFFE, 0x00);
    bus.write(0xFFFF, 0x90);

    stepInstruction(); // Execute BRK

    // Assert
    EXPECT_EQ(cpu.PC, 0x9000); // PC should jump to IRQ vector
    EXPECT_TRUE(cpu.getFlag(CPU6502::I)); // Interrupt Disable flag should be set

    // Check that PC and Status were pushed to stack
    EXPECT_EQ(bus.read(0x01FD), 0x80); // High byte of return address
    EXPECT_EQ(bus.read(0x01FC), 0x02); // Low byte of return address
    EXPECT_TRUE(bus.read(0x01FB) & CPU6502::B);
}


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


TEST_F(CPU6502Test, JSR_RTS_Instruction)
{
    // Load a program that calls JSR and then RTS

    loadProgram(0x8000, { 0x20, 0x00, 0x90 }); // JSR $9000
    stepInstruction(); // Execute JSR
    EXPECT_EQ(cpu.PC, 0x9000);
    EXPECT_EQ(cpu.SP, 0xFB);
    EXPECT_EQ(ram.read(0x01FC), 0x02); // Low byte of return address
    EXPECT_EQ(ram.read(0x01FD), 0x80); // High byte of return address

    loadProgram(0x9000, { 0x60 }); // RTS
    stepInstruction(); // Execute RTS
    EXPECT_EQ(cpu.PC, 0x8003); // Return address + 1
    EXPECT_EQ(cpu.SP, 0xFD);   // Stack Pointer should be back to original position
}


TEST_F(CPU6502Test, ASL)
{
    // ASL Accumulator
    cpu.A = 0x01;
    loadProgram(0x8000, {0x0A}); // ASL A
    stepInstruction();
    EXPECT_EQ(cpu.A, 0x02);
    EXPECT_FALSE(cpu.getFlag(CPU6502::C));
    EXPECT_FALSE(cpu.getFlag(CPU6502::Z));
    EXPECT_FALSE(cpu.getFlag(CPU6502::N));

    cpu.A = 0x80;
    loadProgram(0x8000, {0x0A}); // ASL A
    stepInstruction();
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_TRUE(cpu.getFlag(CPU6502::C));
    EXPECT_TRUE(cpu.getFlag(CPU6502::Z));
    EXPECT_FALSE(cpu.getFlag(CPU6502::N));

    // ASL Zero Page
    ram.write(0x10, 0x01);
    loadProgram(0x8000, {0x06, 0x10}); // ASL $10
    stepInstruction();
    EXPECT_EQ(ram.read(0x10), 0x02);
    EXPECT_FALSE(cpu.getFlag(CPU6502::C));
    EXPECT_FALSE(cpu.getFlag(CPU6502::Z));
    EXPECT_FALSE(cpu.getFlag(CPU6502::N));

    ram.write(0x10, 0x80);
    loadProgram(0x8000, {0x06, 0x10}); // ASL $10
    stepInstruction();
    EXPECT_EQ(ram.read(0x10), 0x00);
    EXPECT_TRUE(cpu.getFlag(CPU6502::C));
    EXPECT_TRUE(cpu.getFlag(CPU6502::Z));
    EXPECT_FALSE(cpu.getFlag(CPU6502::N));
}


TEST_F(CPU6502Test, ADC_IMM_WithoutCarry)
{
    // Test ADC instruction in various scenarios

    // Basic addition without carry
    cpu.A = 0x10;
    loadProgram(0x8000, {0x69, 0x20}); // ADC Immediate
    stepInstruction();
    EXPECT_EQ(cpu.A, 0x30);
    EXPECT_FALSE(cpu.getFlag(CPU6502::C));
    EXPECT_FALSE(cpu.getFlag(CPU6502::Z));
    EXPECT_FALSE(cpu.getFlag(CPU6502::N));
    EXPECT_FALSE(cpu.getFlag(CPU6502::V));
}


TEST_F(CPU6502Test, ADC_IMM_WithCarry)
{
    // Addition with carry
    cpu.A = 0xFF;
    loadProgram(0x8000, {0x69, 0x01}); // ADC Immediate
    stepInstruction();
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_TRUE(cpu.getFlag(CPU6502::C));
    EXPECT_TRUE(cpu.getFlag(CPU6502::Z));
    EXPECT_FALSE(cpu.getFlag(CPU6502::N));
    EXPECT_FALSE(cpu.getFlag(CPU6502::V));
}


TEST_F(CPU6502Test, ADC_IMM_NegativeAndOverflow)
{
    // Addition resulting in negative
    cpu.A = 0x40;
    loadProgram(0x8000, {0x69, 0x40}); // ADC Immediate
    stepInstruction();
    EXPECT_EQ(cpu.A, 0x80);
    EXPECT_FALSE(cpu.getFlag(CPU6502::C));
    EXPECT_FALSE(cpu.getFlag(CPU6502::Z));
    EXPECT_TRUE(cpu.getFlag(CPU6502::N));
    EXPECT_TRUE(cpu.getFlag(CPU6502::V));
}


TEST_F(CPU6502Test, ADC_IMM_OverflowCases)
{
    // Addition causing overflow (Negative + Negative = Positive)
    cpu.A = 0x50;
    loadProgram(0x8000, {0x69, 0x50}); // ADC Immediate
    stepInstruction();
    EXPECT_EQ(cpu.A, 0xA0);
    EXPECT_FALSE(cpu.getFlag(CPU6502::C));
    EXPECT_FALSE(cpu.getFlag(CPU6502::Z));
    EXPECT_TRUE(cpu.getFlag(CPU6502::N));
    EXPECT_TRUE(cpu.getFlag(CPU6502::V));
}


TEST_F(CPU6502Test, ADC_IMM_ZeroFlag)
{
    // Zero flag test
    cpu.A = 0x01;
    loadProgram(0x8000, {0x69, 0xFF});
    stepInstruction();
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_TRUE(cpu.getFlag(CPU6502::Z));
}


TEST_F(CPU6502Test, ADC_IMM_NegativeFlag)
{
    // Negative flag test
    cpu.A = 0x40;
    loadProgram(0x8000, {0x69, 0x40}); // ADC Immediate
    stepInstruction();
    EXPECT_EQ(cpu.A, 0x80);
    EXPECT_FALSE(cpu.getFlag(CPU6502::C));
    EXPECT_FALSE(cpu.getFlag(CPU6502::Z));
    EXPECT_TRUE(cpu.getFlag(CPU6502::N));
    EXPECT_TRUE(cpu.getFlag(CPU6502::V));
}


TEST_F(CPU6502Test, ADC_IMM_MixedSignNoOverflow)
{
    // Mixed sign addition (no overflow)
    cpu.A = 0x50;
    loadProgram(0x8000, {0x69, 0x90}); // ADC Immediate
    stepInstruction();
    EXPECT_EQ(cpu.A, 0xE0);
    EXPECT_FALSE(cpu.getFlag(CPU6502::C));
    EXPECT_FALSE(cpu.getFlag(CPU6502::Z));
    EXPECT_TRUE(cpu.getFlag(CPU6502::N));
    EXPECT_FALSE(cpu.getFlag(CPU6502::V));
}


TEST_F(CPU6502Test, ADC_IMM_SimpleDecimalMode)
{
    // Decimal mode addition
    cpu.setFlag(CPU6502::D, true);
    cpu.A = 0x15;
    loadProgram(0x8000, {0x69, 0x27}); // ADC Immediate
    stepInstruction();
    EXPECT_EQ(cpu.A, 0x42);
    EXPECT_FALSE(cpu.getFlag(CPU6502::C));
    EXPECT_FALSE(cpu.getFlag(CPU6502::Z));
    EXPECT_FALSE(cpu.getFlag(CPU6502::N));
    EXPECT_FALSE(cpu.getFlag(CPU6502::V));
}


TEST_F(CPU6502Test, ADC_IMM_DecimalModeWithCarry)
{
    // Decimal mode addition with carry
    cpu.setFlag(CPU6502::D, true);
    cpu.A = 0x45;
    loadProgram(0x8000, {0x69, 0x55}); // ADC Immediate
    stepInstruction();
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_TRUE(cpu.getFlag(CPU6502::C));
    EXPECT_TRUE(cpu.getFlag(CPU6502::Z));
    EXPECT_FALSE(cpu.getFlag(CPU6502::N));
    EXPECT_TRUE(cpu.getFlag(CPU6502::V));
}


TEST_F(CPU6502Test, ADC_IMM_DecimalModeOverflow)
{
    // Decimal mode addition causing overflow
    cpu.setFlag(CPU6502::D, true);
    cpu.A = 0x50;
    loadProgram(0x8000, {0x69, 0x50}); // ADC Immediate
    stepInstruction();
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_TRUE(cpu.getFlag(CPU6502::C));
    EXPECT_TRUE(cpu.getFlag(CPU6502::Z));
    EXPECT_FALSE(cpu.getFlag(CPU6502::N));
    EXPECT_TRUE(cpu.getFlag(CPU6502::V));
}


TEST_F(CPU6502Test, ADC_IMM_DecimalModeWithCarryIn)
{
    // Decimal mode addition with carry in
    cpu.setFlag(CPU6502::D, true);
    cpu.setFlag(CPU6502::C, true);
    cpu.A = 0x45;
    loadProgram(0x8000, {0x69, 0x54}); // ADC Immediate
    stepInstruction();
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_TRUE(cpu.getFlag(CPU6502::C));
    EXPECT_TRUE(cpu.getFlag(CPU6502::Z));
    EXPECT_FALSE(cpu.getFlag(CPU6502::N));
    EXPECT_TRUE(cpu.getFlag(CPU6502::V));
}


TEST_F(CPU6502Test, RTI_FullRoundTtrip)
{
    // Arrange
    loadProgram(0x8000, {0x00, 0xEA}); // RTI opcode + NOP opcode

    bus.write(0x9000, 0x40); // RTI opcode at 0x9000

    bus.write(0xFFFE, 0x00); // IRQ vector low byte
    bus.write(0xFFFF, 0x90); // IRQ vector high byte

    // Act
    stepInstruction(); // Execute BRK at 0x8000
                       // This will push PC and Status onto the stack
                       // and set PC to IRQ vector (0x9000)
    // Assert
    EXPECT_EQ(cpu.PC, 0x9000); // PC should jump to IRQ vector

    // Act
    stepInstruction(); // Execute RTI at 0x9000
    // Assert
    EXPECT_EQ(cpu.PC, 0x8002); // PC should be restored to 0x8002 (after BRK)
}


// Branch NOT taken (baseline)
TEST_F(CPU6502Test, BEQ_NotTaken)
{
    loadProgram(0x8000, { 0xF0, 0x02 }); // BEQ +2

    int cycles = 0;
    do {
        cpu.clock();
        cycles++;
    } while (cpu.cycles() > 0);

    EXPECT_EQ(cpu.cycles(), 0); // Ensure all cycles consumed
    EXPECT_EQ(cpu.PC, 0x8002);
    EXPECT_EQ(cycles, 2); // 2
}


// Branch taken, no page cross
TEST_F(CPU6502Test, BEQ_Taken_NoPageCross)
{
    loadProgram(0x8000, { 0xF0, 0x02 }); // BEQ +2
    
    cpu.setFlag(CPU6502::Z, true); // Set Zero flag to take the branch

    int cycles = 0;
    do {
        cpu.clock();
        cycles++;
    } while (cpu.cycles() > 0);

    EXPECT_EQ(cpu.cycles(), 0); // Ensure all cycles consumed
    EXPECT_EQ(cpu.PC, 0x8004);
    EXPECT_EQ(cycles, 3); // 2 + 1
}


// Branch taken, forward page cross
TEST_F(CPU6502Test, BEQ_Taken_ForwardPageCross)
{
    loadProgram(0x80FD, { 0xF0, 0x02 }); // BEQ +2

    cpu.setFlag(CPU6502::Z, true); // Set Zero flag to take the branch

    int cycles = 0;
    do {
        cpu.clock();
        cycles++;
    } while (cpu.cycles() > 0);

    EXPECT_EQ(cpu.cycles(), 0); // Ensure all cycles consumed
    EXPECT_EQ(cpu.PC, 0x8101);
    EXPECT_EQ(cycles, 4); // 2 + 1 + 1
}


// Branch taken backward, no page cross
TEST_F(CPU6502Test, BEQ_Taken_BackwardNoPageCross)
{
    loadProgram(0x8005, { 0xF0, 0xFB }); // -5

    cpu.setFlag(CPU6502::Z, true);

    int cycles = 0;
    do {
        cpu.clock();
        cycles++;
    } while (cpu.cycles() > 0);

    EXPECT_EQ(cpu.cycles(), 0); // Ensure all cycles consumed
    EXPECT_EQ(cpu.PC, 0x8002);
    EXPECT_EQ(cycles, 3); // 2 + 1
}


// Branch taken backward, page cross → 4 cycles
TEST_F(CPU6502Test, BEQ_Taken_BackwardPageCross)
{
    loadProgram(0x8102, { 0xF0, 0xFB }); // BEQ -5

    cpu.setFlag(CPU6502::Z, true);

    int cycles = 0;
    do {
        cpu.clock();
        cycles++;
    } while (cpu.cycles() > 0);

    EXPECT_EQ(cpu.cycles(), 0); // Ensure all cycles consumed
    EXPECT_EQ(cpu.PC, 0x80FF);
    EXPECT_EQ(cycles, 4); // 2 + 1 + 1
}


// Zero offset
TEST_F(CPU6502Test, BEQ_Taken_ZeroOffset)
{
    loadProgram(0x8000, { 0xF0, 0x00 }); // BEQ +0
    
    cpu.setFlag(CPU6502::Z, true);

    int cycles = 0;
    do {
        cpu.clock();
        cycles++;
    } while (cpu.cycles() > 0);

    EXPECT_EQ(cpu.cycles(), 0); // Ensure all cycles consumed
    EXPECT_EQ(cpu.PC, 0x8002);
    EXPECT_EQ(cycles, 3); // 2 + 1
}


// Page crossing for memory ops
TEST_F(CPU6502Test, LDA_ABX_PageCross)
{
    cpu.X = 0x01; // Set X to cause page crossing

    loadProgram(0x80FE, { 0xBD, 0xFF, 0x01 }); // LDA $01FF,X
    bus.write(0x0200, 0x42); // Value to be loaded

    int cycles = 0;
    do {
        cpu.clock();
        cycles++;
    } while (cpu.cycles() > 0);

    EXPECT_EQ(cpu.cycles(), 0); // Ensure all cycles consumed
    EXPECT_EQ(cpu.PC, 0x8101); // Next instruction address
    EXPECT_EQ(cpu.A, 0x42); // Loaded value -> A
    EXPECT_EQ(cycles, 5); // LDA (3) + BEQ (3 with page cross)
}


// Control-flow timing
TEST_F(CPU6502Test, JSR_Cycles)
{
    loadProgram(0x8000, {
        0x20, 0x00, 0x90, // JSR $9000
    });

    int cycles = 0;
    do {
        cpu.clock();
        cycles++;
    } while (cpu.cycles() > 0);

    EXPECT_EQ(cpu.cycles(), 0); // Ensure all cycles consumed
    EXPECT_EQ(cpu.PC, 0x9000); // Jumped to subroutine
    EXPECT_EQ(cycles, 6); // JSR (6)
}


// Interrupt timing
TEST_F(CPU6502Test, IRQ_Cycles)
{
    loadProgram(0x8000, {
        0x00, // BRK (to enable IRQ handling)
    });

    // Set IRQ vector to 0x9000
    bus.write(0xFFFE, 0x00);
    bus.write(0xFFFF, 0x90);

    cpu.irq(); // Trigger IRQ

    int cycles = 0;
    do {
        cpu.clock();
        cycles++;
    } while (cpu.cycles() > 0); 

    EXPECT_EQ(cpu.cycles(), 0); // Ensure all cycles consumed
    EXPECT_EQ(cpu.PC, 0x9000); // Jumped to IRQ vector
    EXPECT_EQ(cycles, 7); // IRQ handling (7)
}


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


TEST_F(CPU6502Test, IRQ_TriggersInterrupt)
{
    // Arrange
    bus.write(0xFFFE, 0x00); // IRQ vector low byte
    bus.write(0xFFFF, 0x80); // IRQ vector high byte

    uint16_t initial_pc = cpu.PC;

    // Act
    // Simulate an IRQ signal
    cpu.irq();

    // Execute enough cycles to process the IRQ
    stepInstruction();

    // Assert
    // Check that the Program Counter has changed to the IRQ vector address
    uint16_t pc_after = cpu.PC;
    EXPECT_EQ(pc_after, 0x8000); // IRQ vector address
    EXPECT_NE(pc_after, initial_pc); // PC should have changed
    EXPECT_TRUE(cpu.getFlag(CPU6502::I)); // Interrupt Disable flag should be set
}


TEST_F(CPU6502Test, IRQ_DisableFlagPreventsInterrupt)
{
    // Arrange
    cpu.setFlag(CPU6502::I, true); // Set Interrupt Disable flag

    // Simulate an IRQ signal
    cpu.irq();

    // Act
    // Execute enough cycles to process the IRQ if it were to be handled
    stepInstruction();

    // Assert
    // Check that the Program Counter has not changed to the IRQ vector address
    EXPECT_NE(cpu.PC, 0xFFFE); // IRQ vector address
}


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


TEST_F(CPU6502Test, JSR_Instruction)
{
    loadProgram(0x8000, { 0x20, 0x34, 0x12 }); // JSR $1234

    stepInstruction();
    EXPECT_EQ(cpu.PC, 0x1234);
    EXPECT_EQ(cpu.SP, 0xFB);
    EXPECT_EQ(ram.read(0x01FC), 0x02);
    EXPECT_EQ(ram.read(0x01FD), 0x80);
}


TEST_F(CPU6502Test, JSR_Push_Return_And_Jump)
{
    loadProgram(0x8000, {0x20, 0x00, 0x90}); // JSR $9000
    stepInstruction();
    EXPECT_EQ(cpu.PC, 0x9000);
    EXPECT_EQ(cpu.SP, 0xFB);
    EXPECT_EQ(ram.read(0x01FC), 0x02); // Low byte of return address
    EXPECT_EQ(ram.read(0x01FD), 0x80); // High byte of return address
}


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


TEST_F(CPU6502Test, NMI_TriggersInterrupt)
{
    // Arrange
    bus.write(0xFFFA, 0x00); // NMI vector low byte
    bus.write(0xFFFB, 0x80); // NMI vector high byte

    uint16_t initial_pc = cpu.PC;

    // Act: Simulate an NMI signal
    cpu.nmi();

    // Execute enough cycles to process the NMI
    for (int i = 0; i < 7; ++i) {
        cpu.clock();
    }

    // Assert: Check that the PC has changed to the NMI vector address
    uint16_t pc_after = cpu.PC;
    EXPECT_EQ(pc_after, 0x8000); // NMI vector address
    EXPECT_NE(pc_after, initial_pc); // PC should have changed

    // Check that the stack contains the correct return address and status
    uint8_t status = bus.read(0x0100 + cpu.SP + 1);
    EXPECT_EQ(status & CPU6502::U, CPU6502::U); // Unused flag should be set
    EXPECT_TRUE(cpu.getFlag(CPU6502::I)); // the Interrupt Disable should be set

    uint8_t pcl = bus.read(0x0100 + cpu.SP + 2);
    uint8_t pch = bus.read(0x0100 + cpu.SP + 3);
    uint16_t return_address = (pch << 8) | pcl;
    EXPECT_EQ(return_address, initial_pc);
}


TEST_F(CPU6502Test, NMI_AlwaysTakesPrecedenceOverIRQ)
{
    // Arrange
    bus.write(0xFFFA, 0x00); // NMI vector low byte
    bus.write(0xFFFB, 0x80); // NMI vector high byte

    bus.write(0xFFFE, 0x00); // IRQ vector low byte
    bus.write(0xFFFF, 0x90); // IRQ vector high byte

    uint16_t initial_pc = cpu.PC;

    // Act
    // Simulate both NMI and IRQ signals
    cpu.nmi();
    cpu.irq();

    // Execute enough cycles to process the interrupts
    stepInstruction();

    // Assert
    // Check that the Program Counter has changed to the NMI vector address
    uint16_t pc_after = cpu.PC;
    EXPECT_EQ(pc_after, 0x8000); // NMI vector address
    EXPECT_NE(pc_after, initial_pc); // PC should have changed
}


TEST_F(CPU6502Test, ROR)
{
    // ROR Accumulator
    cpu.A = 0x02;
    cpu.setFlag(CPU6502::C, false);
    loadProgram(0x8000, {0x6A}); // ROR A
    stepInstruction();
    EXPECT_EQ(cpu.A, 0x01);
    EXPECT_FALSE(cpu.getFlag(CPU6502::C));
    EXPECT_FALSE(cpu.getFlag(CPU6502::Z));
    EXPECT_FALSE(cpu.getFlag(CPU6502::N));

    cpu.A = 0x01;
    cpu.setFlag(CPU6502::C, false);
    loadProgram(0x8000, {0x6A}); // ROR A
    stepInstruction();
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_TRUE(cpu.getFlag(CPU6502::C));
    EXPECT_TRUE(cpu.getFlag(CPU6502::Z));
    EXPECT_FALSE(cpu.getFlag(CPU6502::N));

    cpu.A = 0x00;
    cpu.setFlag(CPU6502::C, true);
    loadProgram(0x8000, {0x6A}); // ROR A
    stepInstruction();
    EXPECT_EQ(cpu.A, 0x80);
    EXPECT_FALSE(cpu.getFlag(CPU6502::C));
    EXPECT_FALSE(cpu.getFlag(CPU6502::Z));
    EXPECT_TRUE(cpu.getFlag(CPU6502::N));
}


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


TEST_F(CPU6502Test, RTI_RestoresStateFromStack)
{
    // Arrange
    loadProgram(0x8000, {0x40}); // RTI opcode

    bus.write(0x01FB, CPU6502::U); // Status byte with unused flag set
    bus.write(0x01FC, 0x34); // Low byte of return address
    bus.write(0x01FD, 0x12); // High byte of return address

    cpu.SP = 0xFA; // Set stack pointer to point to the status byte

    // Act
    stepInstruction();

    // Assert
    EXPECT_EQ(cpu.PC, 0x1234); // PC should be restored to 0x1234
}


TEST_F(CPU6502Test, RTS_Instruction)
{
    // Load a program that calls RTS
    loadProgram(0x8000, { 0x60 }); // RTS

    // Manually set up the stack to simulate a return address
    ram.write(0x01FC, 0x04); // Low byte of return address
    ram.write(0x01FD, 0x80); // High byte of return address
    cpu.SP = 0xFB; // Stack Pointer after pushing two bytes

    stepInstruction();

    // After RTS, PC should be set to 0x8005 (return address + 1)
    EXPECT_EQ(cpu.PC, 0x8005);
    EXPECT_EQ(cpu.SP, 0xFD); // Stack Pointer should be back to original position
}


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


TEST_F(CPU6502Test, TAX)
{
    // Test TAX instruction
    cpu.A = 0x00;
    loadProgram(0x8000, {0xAA}); // TAX
    stepInstruction();
    EXPECT_EQ(cpu.X, 0x00);
    EXPECT_TRUE(cpu.getFlag(CPU6502::Z));
    EXPECT_FALSE(cpu.getFlag(CPU6502::N));

    cpu.A = 0x7F;
    loadProgram(0x8000, {0xAA}); // TAX
    stepInstruction();
    EXPECT_EQ(cpu.X, 0x7F);
    EXPECT_FALSE(cpu.getFlag(CPU6502::Z));
    EXPECT_FALSE(cpu.getFlag(CPU6502::N));

    cpu.A = 0x80;
    loadProgram(0x8000, {0xAA}); // TAX
    stepInstruction();
    EXPECT_EQ(cpu.X, 0x80);
    EXPECT_FALSE(cpu.getFlag(CPU6502::Z));
    EXPECT_TRUE(cpu.getFlag(CPU6502::N));
}
