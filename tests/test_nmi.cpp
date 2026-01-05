#include "gtest_fixture.hpp"


TEST_F(CPU6502Test, NMI_TriggersInterrupt)
{
    // Arrange
    cpu.reset();

    bus.write(0xFFFA, 0x00); // NMI vector low byte
    bus.write(0xFFFB, 0x80); // NMI vector high byte

    uint16_t initial_pc = cpu.PC;

    // Act
    // Simulate an NMI signal
    cpu.nmi();

    // Execute enough cycles to process the NMI
    for (int i = 0; i < 7; ++i) {
        cpu.clock();
    }

    // Assert
    // Check that the Program Counter has changed to the NMI vector address
    uint16_t pc_after = cpu.PC;
    EXPECT_EQ(pc_after, 0x8000); // NMI vector address
    EXPECT_NE(pc_after, initial_pc); // PC should have changed
}

TEST_F(CPU6502Test, NMI_AlwaysTakesPrecedenceOverIRQ)
{
    // Arrange
    bus.write(0xFFFA, 0x00); // NMI vector low byte
    bus.write(0xFFFB, 0x80); // NMI vector high byte

    bus.write(0xFFFE, 0x00); // IRQ vector low byte
    bus.write(0xFFFF, 0x90); // IRQ vector high byte

    uint16_t initial_pc = cpu.PC;

    // Act
    // Simulate both NMI and IRQ signals
    cpu.nmi();
    cpu.irq();

    // Execute enough cycles to process the interrupts
    stepInstruction();

    // Assert
    // Check that the Program Counter has changed to the NMI vector address
    uint16_t pc_after = cpu.PC;
    EXPECT_EQ(pc_after, 0x8000); // NMI vector address
    EXPECT_NE(pc_after, initial_pc); // PC should have changed
}