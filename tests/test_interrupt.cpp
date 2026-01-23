#include "TestFixture.h"


TEST_F(CPUTest, NMI_TriggersInterrupt)
{
    // Arrange
    bus.Write(0xFFFA, 0x00); // NMI vector low byte
    bus.Write(0xFFFB, 0x80); // NMI vector high byte

    uint16_t initial_pc = cpu.PC;

    // Act: Simulate an NMI signal
    cpu.NMI();

    // Execute enough cycles to process the NMI
    for (int i = 0; i < 7; ++i)
    {
        cpu.Clock();
    }

    // Assert: Check that the PC has changed to the NMI vector address
    uint16_t pc_after = cpu.PC;
    EXPECT_EQ(pc_after, 0x8000); // NMI vector address
    EXPECT_NE(pc_after, initial_pc); // PC should have changed

    // Check that the stack contains the correct return address and status
    uint8_t status = bus.Read(0x0100 + cpu.SP + 1);
    EXPECT_EQ(status & CPU::StatusFlag::F_UNUSED, CPU::StatusFlag::F_UNUSED); // Unused flag should be set
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_INTERRUPT)); // the Interrupt Disable should be set

    uint8_t pcl = bus.Read(0x0100 + cpu.SP + 2);
    uint8_t pch = bus.Read(0x0100 + cpu.SP + 3);
    uint16_t return_address = (pch << 8) | pcl;
    EXPECT_EQ(return_address, initial_pc);
}


TEST_F(CPUTest, NMI_AlwaysTakesPrecedenceOverIRQ)
{
    // Arrange
    bus.Write(0xFFFA, 0x00); // NMI vector low byte
    bus.Write(0xFFFB, 0x80); // NMI vector high byte

    bus.Write(0xFFFE, 0x00); // IRQ vector low byte
    bus.Write(0xFFFF, 0x90); // IRQ vector high byte

    uint16_t initial_pc = cpu.PC;

    // Act
    // Simulate both NMI and IRQ signals
    cpu.NMI();
    cpu.IRQ();

    // Execute enough cycles to process the interrupts
    cpu.Step();

    // Assert
    // Check that the Program Counter has changed to the NMI vector address
    uint16_t pc_after = cpu.PC;
    EXPECT_EQ(pc_after, 0x8000); // NMI vector address
    EXPECT_NE(pc_after, initial_pc); // PC should have changed
}
