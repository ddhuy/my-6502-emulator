#include "gtest_fixture.hpp"


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
