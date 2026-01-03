#include "gtest_fixture.hpp"


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