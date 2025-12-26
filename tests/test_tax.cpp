#include "gtest_fixture.hpp"


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