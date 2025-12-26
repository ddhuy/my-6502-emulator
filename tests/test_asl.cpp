#include "gtest_fixture.hpp"


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
