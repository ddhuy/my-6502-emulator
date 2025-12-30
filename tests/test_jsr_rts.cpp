#include "gtest_fixture.hpp"


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