#include "gtest_fixture.hpp"



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