#include "gtest_fixture.hpp"


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