#include "gtest_fixture.hpp"


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
