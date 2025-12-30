#include "gtest_fixture.hpp"


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
