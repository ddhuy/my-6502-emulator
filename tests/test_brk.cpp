#include "gtest_fixture.hpp"


TEST_F(CPU6502Test, BRK_PushesStateAndJumpsToIRQVector_1)
{
    // Arrange
    cpu.reset();
    cpu.PC = 0x2000;
    cpu.setFlag(CPU6502::I, false); // Clear Interrupt Disable flag

    // Load BRK instruction at current PC
    bus.write(0x2000, 0x00); // BRK opcode

    // Act
    stepInstruction();

    // Assert
    EXPECT_TRUE(cpu.getFlag(CPU6502::I)); // Interrupt Disable flag should be set

    // Check that PC and Status were pushed to stack
    uint8_t sp = cpu.SP + 1; // SP was decremented after push
    uint16_t pc_lo = bus.read(0x0100 + sp + 1);
    uint16_t pc_hi = bus.read(0x0100 + sp + 2);
    uint8_t status = bus.read(0x0100 + sp);

    EXPECT_EQ((pc_hi << 8) | pc_lo, 0x2002); // PC should point to next instruction
    EXPECT_EQ(status & CPU6502::B, CPU6502::B); // Status should have Break flag set
}

TEST_F(CPU6502Test, BRK_PushesStateAndJumpsToIRQVector_2)
{
    loadProgram(0x8000, {0x00,  // BRK opcode
                         0xEA   // NOP opcode to follow BRK
                        });

    // Set IRQ vector to 0x9000
    bus.write(0xFFFE, 0x00);
    bus.write(0xFFFF, 0x90);

    stepInstruction(); // Execute BRK

    // Assert
    EXPECT_EQ(cpu.PC, 0x9000); // PC should jump to IRQ vector
    EXPECT_TRUE(cpu.getFlag(CPU6502::I)); // Interrupt Disable flag should be set

    // Check that PC and Status were pushed to stack
    EXPECT_EQ(bus.read(0x01FD), 0x80); // High byte of return address
    EXPECT_EQ(bus.read(0x01FC), 0x02); // Low byte of return address
    EXPECT_TRUE(bus.read(0x01FB) & CPU6502::B);
}