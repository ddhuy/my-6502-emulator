#include "gtest_fixture.hpp"


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