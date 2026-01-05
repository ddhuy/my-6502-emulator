#include "gtest_fixture.hpp"


TEST_F(CPU6502Test, IRQ_TriggersInterrupt)
{
    // Arrange
    bus.write(0xFFFE, 0x00); // IRQ vector low byte
    bus.write(0xFFFF, 0x80); // IRQ vector high byte

    uint16_t initial_pc = cpu.PC;

    // Act
    // Simulate an IRQ signal
    cpu.irq();

    // Execute enough cycles to process the IRQ
    stepInstruction();

    // Assert
    // Check that the Program Counter has changed to the IRQ vector address
    uint16_t pc_after = cpu.PC;
    EXPECT_EQ(pc_after, 0x8000); // IRQ vector address
    EXPECT_NE(pc_after, initial_pc); // PC should have changed
    EXPECT_TRUE(cpu.getFlag(CPU6502::I)); // Interrupt Disable flag should be set
}

TEST_F(CPU6502Test, IRQ_DisableFlagPreventsInterrupt)
{
    // Arrange
    cpu.setFlag(CPU6502::I, true); // Set Interrupt Disable flag

    // Simulate an IRQ signal
    cpu.irq();

    // Act
    // Execute enough cycles to process the IRQ if it were to be handled
    stepInstruction();

    // Assert
    // Check that the Program Counter has not changed to the IRQ vector address
    EXPECT_NE(cpu.PC, 0xFFFE); // IRQ vector address
}
