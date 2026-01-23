#include <gtest/gtest.h>
#include "cpu/CPU.h"
#include "bus/Bus.h"
#include "memory/Memory.h"


class OpcodeTest : public ::testing::Test
{
protected:
    Memory memory;
    Bus bus;
    CPU cpu;

    void SetUp() override
    {
        bus.AttachMemory(&memory);
        cpu.ConnectBus(&bus);
        cpu.Reset();

        // Drain reset cycles
        while (cpu.GetCycles() > 0)
            cpu.Clock();
    }
};


// ============================================================================
// LDA Tests
// ============================================================================

TEST_F(OpcodeTest, LDA_Immediate)
{
    uint8_t program[] = {0xA9, 0x42};  // LDA #$42
    cpu.LoadProgram(program, sizeof(program));
    
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0x42);
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t) 2 + 7); // 2 for LDA + 7 for Reset
}

TEST_F(OpcodeTest, LDA_ImmediateSetsZeroFlag)
{
    uint8_t program[] = {0xA9, 0x00};  // LDA #$00
    cpu.LoadProgram(program, sizeof(program));
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));
}

TEST_F(OpcodeTest, LDA_ImmediateSetsNegativeFlag)
{
    uint8_t program[] = {0xA9, 0x80};  // LDA #$80
    cpu.LoadProgram(program, sizeof(program));
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0x80);
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));
}

TEST_F(OpcodeTest, LDA_ZeroPage)
{
    uint8_t program[] = {0xA5, 0x10};  // LDA $10
    cpu.LoadProgram(program, sizeof(program));
    cpu.WriteMemory(0x0010, 0x55);
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0x55);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t) 3 + 7); // 3 for LDA + 7 for Reset
}

TEST_F(OpcodeTest, LDA_ZeroPageX)
{
    uint8_t program[] = {0xB5, 0x10};  // LDA $10,X
    cpu.LoadProgram(program, sizeof(program));
    cpu.X = 0x05;
    cpu.WriteMemory(0x0015, 0xAA);
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0xAA);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t) 4 + 7); // 4 for LDA + 7 for Reset
}

TEST_F(OpcodeTest, LDA_Absolute)
{
    uint8_t program[] = {0xAD, 0x34, 0x12};  // LDA $1234
    cpu.LoadProgram(program, sizeof(program));
    cpu.WriteMemory(0x1234, 0x99);
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0x99);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t) 4 + 7); // 4 for LDA + 7 for Reset
}

TEST_F(OpcodeTest, LDA_AbsoluteXNoPageCross)
{
    uint8_t program[] = {0xBD, 0x00, 0x12};  // LDA $1200,X
    cpu.LoadProgram(program, sizeof(program));
    cpu.X = 0x05;
    cpu.WriteMemory(0x1205, 0x77);
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0x77);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t) 4 + 7);  // No page cross
}

TEST_F(OpcodeTest, LDA_AbsoluteXPageCross)
{
    uint8_t program[] = {0xBD, 0xFF, 0x12};  // LDA $12FF,X
    cpu.LoadProgram(program, sizeof(program));
    cpu.X = 0x05;
    cpu.WriteMemory(0x1304, 0x88);
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0x88);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t) 5 + 7);  // +1 for page cross +7 for Reset
}

TEST_F(OpcodeTest, LDA_AbsoluteY)
{
    uint8_t program[] = {0xB9, 0x00, 0x12};  // LDA $1200,Y
    cpu.LoadProgram(program, sizeof(program));
    cpu.Y = 0x03;
    cpu.WriteMemory(0x1203, 0x66);
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0x66);
}

TEST_F(OpcodeTest, LDA_IndexedIndirectX)
{
    uint8_t program[] = {0xA1, 0x40};  // LDA ($40,X)
    cpu.LoadProgram(program, sizeof(program));
    cpu.X = 0x05;
    cpu.WriteMemory(0x45, 0x34);  // Low byte of address
    cpu.WriteMemory(0x46, 0x12);  // High byte of address
    cpu.WriteMemory(0x1234, 0xCC);
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0xCC);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t) 6 + 7); // 6 for LDA + 7 for Reset
}

TEST_F(OpcodeTest, LDA_IndirectIndexedY)
{
    uint8_t program[] = {0xB1, 0x40};  // LDA ($40),Y
    cpu.LoadProgram(program, sizeof(program));
    cpu.Y = 0x10;
    cpu.WriteMemory(0x40, 0x00);
    cpu.WriteMemory(0x41, 0x12);
    cpu.WriteMemory(0x1210, 0xDD);
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0xDD);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t) 5 + 7); // 5 for LDA + 7 for Reset
}

// ============================================================================
// LDX Tests
// ============================================================================

TEST_F(OpcodeTest, LDX_Immediate)
{
    uint8_t program[] = {0xA2, 0x55};  // LDX #$55
    cpu.LoadProgram(program, sizeof(program));
    cpu.Step();
    
    EXPECT_EQ(cpu.X, 0x55);
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));
}

TEST_F(OpcodeTest, LDX_ZeroPage)
{
    uint8_t program[] = {0xA6, 0x20};  // LDX $20
    cpu.LoadProgram(program, sizeof(program));
    cpu.WriteMemory(0x20, 0x33);
    cpu.Step();
    
    EXPECT_EQ(cpu.X, 0x33);
}

TEST_F(OpcodeTest, LDX_ZeroPageY)
{
    uint8_t program[] = {0xB6, 0x20};  // LDX $20,Y
    cpu.LoadProgram(program, sizeof(program));
    cpu.Y = 0x05;
    cpu.WriteMemory(0x25, 0x44);
    cpu.Step();
    
    EXPECT_EQ(cpu.X, 0x44);
}

// ============================================================================
// LDY Tests
// ============================================================================

TEST_F(OpcodeTest, LDY_Immediate)
{
    uint8_t program[] = {0xA0, 0x66};  // LDY #$66
    cpu.LoadProgram(program, sizeof(program));
    cpu.Step();
    
    EXPECT_EQ(cpu.Y, 0x66);
}

TEST_F(OpcodeTest, LDY_ZeroPageX)
{
    uint8_t program[] = {0xB4, 0x20};  // LDY $20,X
    cpu.LoadProgram(program, sizeof(program));
    cpu.X = 0x03;
    cpu.WriteMemory(0x23, 0x77);
    cpu.Step();
    
    EXPECT_EQ(cpu.Y, 0x77);
}

// ============================================================================
// STA Tests
// ============================================================================

TEST_F(OpcodeTest, STA_ZeroPage)
{
    uint8_t program[] = {0x85, 0x10};  // STA $10
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0x42;
    cpu.Step();
    
    EXPECT_EQ(cpu.ReadMemory(0x10), 0x42);
}

TEST_F(OpcodeTest, STA_ZeroPageX)
{
    uint8_t program[] = {0x95, 0x10};  // STA $10,X
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0x55;
    cpu.X = 0x05;
    cpu.Step();
    
    EXPECT_EQ(cpu.ReadMemory(0x15), 0x55);
}

TEST_F(OpcodeTest, STA_Absolute)
{
    uint8_t program[] = {0x8D, 0x34, 0x12};  // STA $1234
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0xAA;
    cpu.Step();
    
    EXPECT_EQ(cpu.ReadMemory(0x1234), 0xAA);
}

TEST_F(OpcodeTest, STA_AbsoluteX)
{
    uint8_t program[] = {0x9D, 0x00, 0x12};  // STA $1200,X
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0xBB;
    cpu.X = 0x10;
    cpu.Step();
    
    EXPECT_EQ(cpu.ReadMemory(0x1210), 0xBB);
}

TEST_F(OpcodeTest, STA_AbsoluteY)
{
    uint8_t program[] = {0x99, 0x00, 0x12};  // STA $1200,Y
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0xCC;
    cpu.Y = 0x08;
    cpu.Step();
    
    EXPECT_EQ(cpu.ReadMemory(0x1208), 0xCC);
}

TEST_F(OpcodeTest, STA_IndexedIndirectX)
{
    uint8_t program[] = {0x81, 0x40};  // STA ($40,X)
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0xDD;
    cpu.X = 0x05;
    cpu.WriteMemory(0x45, 0x34);
    cpu.WriteMemory(0x46, 0x12);
    cpu.Step();
    
    EXPECT_EQ(cpu.ReadMemory(0x1234), 0xDD);
}

TEST_F(OpcodeTest, STA_IndirectIndexedY)
{
    uint8_t program[] = {0x91, 0x40};  // STA ($40),Y
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0xEE;
    cpu.Y = 0x10;
    cpu.WriteMemory(0x40, 0x00);
    cpu.WriteMemory(0x41, 0x12);
    cpu.Step();
    
    EXPECT_EQ(cpu.ReadMemory(0x1210), 0xEE);
}

// ============================================================================
// STX Tests
// ============================================================================

TEST_F(OpcodeTest, STX_ZeroPage)
{
    uint8_t program[] = {0x86, 0x10};  // STX $10
    cpu.LoadProgram(program, sizeof(program));
    cpu.X = 0x99;
    cpu.Step();
    
    EXPECT_EQ(cpu.ReadMemory(0x10), 0x99);
}

TEST_F(OpcodeTest, STX_ZeroPageY)
{
    uint8_t program[] = {0x96, 0x10};  // STX $10,Y
    cpu.LoadProgram(program, sizeof(program));
    cpu.X = 0x88;
    cpu.Y = 0x03;
    cpu.Step();
    
    EXPECT_EQ(cpu.ReadMemory(0x13), 0x88);
}

// ============================================================================
// STY Tests
// ============================================================================

TEST_F(OpcodeTest, STY_ZeroPage)
{
    uint8_t program[] = {0x84, 0x10};  // STY $10
    cpu.LoadProgram(program, sizeof(program));
    cpu.Y = 0x77;
    cpu.Step();
    
    EXPECT_EQ(cpu.ReadMemory(0x10), 0x77);
}

// ============================================================================
// Transfer Tests
// ============================================================================

TEST_F(OpcodeTest, TAX)
{
    uint8_t program[] = {0xAA};  // TAX
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0x42;
    cpu.Step();
    
    EXPECT_EQ(cpu.X, 0x42);
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));
}

TEST_F(OpcodeTest, TAX_SetsZeroFlag)
{
    uint8_t program[] = {0xAA};  // TAX
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0x00;
    cpu.Step();
    
    EXPECT_EQ(cpu.X, 0x00);
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
}

TEST_F(OpcodeTest, TAY)
{
    uint8_t program[] = {0xA8};  // TAY
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0x55;
    cpu.Step();
    
    EXPECT_EQ(cpu.Y, 0x55);
}

TEST_F(OpcodeTest, TXA)
{
    uint8_t program[] = {0x8A};  // TXA
    cpu.LoadProgram(program, sizeof(program));
    cpu.X = 0x33;
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0x33);
}

TEST_F(OpcodeTest, TYA)
{
    uint8_t program[] = {0x98};  // TYA
    cpu.LoadProgram(program, sizeof(program));
    cpu.Y = 0x44;
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0x44);
}

TEST_F(OpcodeTest, TSX)
{
    uint8_t program[] = {0xBA};  // TSX
    cpu.LoadProgram(program, sizeof(program));
    cpu.SP = 0xF0;
    cpu.Step();
    
    EXPECT_EQ(cpu.X, 0xF0);
}

TEST_F(OpcodeTest, TXS)
{
    uint8_t program[] = {0x9A};  // TXS
    cpu.LoadProgram(program, sizeof(program));
    cpu.X = 0xA0;
    cpu.Step();
    
    EXPECT_EQ(cpu.SP, 0xA0);
}

// ============================================================================
// Shift/Rotate Tests
// ============================================================================

TEST_F(OpcodeTest, ASL_Accumulator)
{
    uint8_t program[] = {0x0A};  // ASL A
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0x55;
    
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0xAA);
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));
}

TEST_F(OpcodeTest, ASL_SetsCarry)
{
    uint8_t program[] = {0x0A};  // ASL A
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0x80;
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
}

TEST_F(OpcodeTest, ASL_ZeroPage)
{
    uint8_t program[] = {0x06, 0x10};  // ASL $10
    cpu.LoadProgram(program, sizeof(program));
    cpu.WriteMemory(0x10, 0x01);
    cpu.Step();
    
    EXPECT_EQ(cpu.ReadMemory(0x10), 0x02);
}

TEST_F(OpcodeTest, LSR_Accumulator)
{
    uint8_t program[] = {0x4A};  // LSR A
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0xAA;
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0x55);
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));
}

TEST_F(OpcodeTest, LSR_SetsCarry)
{
    uint8_t program[] = {0x4A};  // LSR A
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0x01;
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
}

TEST_F(OpcodeTest, ROL_Accumulator)
{
    uint8_t program[] = {0x2A};  // ROL A
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0x55;
    cpu.SetFlag(CPU::StatusFlag::F_CARRY, true);
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0xAB);  // 0x55 << 1 | 1
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
}

TEST_F(OpcodeTest, ROL_SetsCarry)
{
    uint8_t program[] = {0x2A};  // ROL A
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0x80;
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
}

TEST_F(OpcodeTest, ROR_Accumulator)
{
    uint8_t program[] = {0x6A};  // ROR A
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0xAA;
    cpu.SetFlag(CPU::StatusFlag::F_CARRY, true);
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0xD5);  // 0x80 | (0xAA >> 1)
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
}

// ============================================================================
// Branch Tests
// ============================================================================

TEST_F(OpcodeTest, BCC_BranchNotTaken)
{
    uint8_t program[] = {0x90, 0x05};  // BCC +5
    cpu.LoadProgram(program, sizeof(program));
    cpu.SetFlag(CPU::StatusFlag::F_CARRY, true);
    uint64_t oldCycles = cpu.GetCycles();
    cpu.Step();
    
    EXPECT_EQ(cpu.PC, 0x8002);  // Didn't branch
    EXPECT_EQ(cpu.GetTotalCycles() - oldCycles, (uint64_t) 2 + 7);  // Base cycles only
                                                                    // 2 for BCC + 7 for Reset
}

TEST_F(OpcodeTest, BCC_BranchTaken)
{
    uint8_t program[] = {0x90, 0x05};  // BCC +5
    cpu.LoadProgram(program, sizeof(program));
    cpu.SetFlag(CPU::StatusFlag::F_CARRY, false);
    uint64_t oldCycles = cpu.GetCycles();
    cpu.Step();
    
    EXPECT_EQ(cpu.PC, 0x8007);  // 0x8002 + 5
    EXPECT_EQ(cpu.GetTotalCycles() - oldCycles, (uint64_t) 3 + 7);  // +1 for branch taken
                                                                    // 3 for BCC + 7 for Reset
}

TEST_F(OpcodeTest, BCS_BranchTaken)
{
    uint8_t program[] = {0xB0, 0x10};  // BCS +16
    cpu.LoadProgram(program, sizeof(program));
    cpu.SetFlag(CPU::StatusFlag::F_CARRY, true);
    cpu.Step();
    
    EXPECT_EQ(cpu.PC, 0x8012);
}

TEST_F(OpcodeTest, BEQ_BranchTaken)
{
    uint8_t program[] = {0xF0, 0x08};  // BEQ +8
    cpu.LoadProgram(program, sizeof(program));
    cpu.SetFlag(CPU::StatusFlag::F_ZERO, true);
    cpu.Step();
    
    EXPECT_EQ(cpu.PC, 0x800A);
}

TEST_F(OpcodeTest, BNE_BranchTaken)
{
    uint8_t program[] = {0xD0, 0x08};  // BNE +8
    cpu.LoadProgram(program, sizeof(program));
    cpu.SetFlag(CPU::StatusFlag::F_ZERO, false);
    cpu.Step();
    
    EXPECT_EQ(cpu.PC, 0x800A);
}

TEST_F(OpcodeTest, BMI_BranchTaken)
{
    uint8_t program[] = {0x30, 0x04};  // BMI +4
    cpu.LoadProgram(program, sizeof(program));
    cpu.SetFlag(CPU::StatusFlag::F_NEGATIVE, true);
    cpu.Step();
    
    EXPECT_EQ(cpu.PC, 0x8006);
}

TEST_F(OpcodeTest, BPL_BranchTaken)
{
    uint8_t program[] = {0x10, 0x04};  // BPL +4
    cpu.LoadProgram(program, sizeof(program));
    cpu.SetFlag(CPU::StatusFlag::F_NEGATIVE, false);
    cpu.Step();
    
    EXPECT_EQ(cpu.PC, 0x8006);
}

TEST_F(OpcodeTest, BVC_BranchTaken)
{
    uint8_t program[] = {0x50, 0x04};  // BVC +4
    cpu.LoadProgram(program, sizeof(program));
    cpu.SetFlag(CPU::StatusFlag::F_OVERFLOW, false);
    cpu.Step();
    
    EXPECT_EQ(cpu.PC, 0x8006);
}

TEST_F(OpcodeTest, BVS_BranchTaken)
{
    uint8_t program[] = {0x70, 0x04};  // BVS +4
    cpu.LoadProgram(program, sizeof(program));
    cpu.SetFlag(CPU::StatusFlag::F_OVERFLOW, true);
    cpu.Step();
    
    EXPECT_EQ(cpu.PC, 0x8006);
}

TEST_F(OpcodeTest, Branch_BackwardOffset)
{
    uint8_t program[] = {0xD0, 0xFC};  // BNE -4 (0xFC = -4 in two's complement)
    cpu.LoadProgram(program, sizeof(program), 0x8010);
    cpu.SetFlag(CPU::StatusFlag::F_ZERO, false);
    cpu.Step();
    
    EXPECT_EQ(cpu.PC, 0x800E);  // 0x8012 - 4
}

// ============================================================================
// Jump/Call Tests
// ============================================================================

TEST_F(OpcodeTest, JMP_Absolute)
{
    uint8_t program[] = {0x4C, 0x34, 0x12};  // JMP $1234
    cpu.LoadProgram(program, sizeof(program));
    cpu.Step();
    
    EXPECT_EQ(cpu.PC, 0x1234);
}

TEST_F(OpcodeTest, JMP_Indirect)
{
    uint8_t program[] = {0x6C, 0x20, 0x30};  // JMP ($3020)
    cpu.LoadProgram(program, sizeof(program));
    cpu.WriteMemory(0x3020, 0x34);
    cpu.WriteMemory(0x3021, 0x12);
    cpu.Step();
    
    EXPECT_EQ(cpu.PC, 0x1234);
}

TEST_F(OpcodeTest, JSR_RTS)
{
    uint8_t program[] = {
        0x20, 0x05, 0x80,  // JSR $8005
        0xEA,              // NOP (placeholder)
        0xEA,              // NOP (placeholder)
        0x60               // RTS
    };
    cpu.LoadProgram(program, sizeof(program));
    
    cpu.Step();  // JSR
    EXPECT_EQ(cpu.PC, 0x8005);
    EXPECT_EQ(cpu.SP, 0xFB);  // Two bytes pushed
    
    cpu.Step();  // RTS
    EXPECT_EQ(cpu.PC, 0x8003);  // Return to next instruction after JSR
    EXPECT_EQ(cpu.SP, 0xFD);  // Stack restored
}

// ============================================================================
// Stack Tests
// ============================================================================

TEST_F(OpcodeTest, PHA_PLA)
{
    uint8_t program[] = {0x48, 0x68};  // PHA, PLA
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0x42;
    
    cpu.Step();  // PHA
    EXPECT_EQ(cpu.SP, 0xFC);
    
    cpu.A = 0x00;  // Clear A
    cpu.Step();  // PLA
    EXPECT_EQ(cpu.A, 0x42);
    EXPECT_EQ(cpu.SP, 0xFD);
}

TEST_F(OpcodeTest, PHP_PLP)
{
    uint8_t program[] = {0x08, 0x28};  // PHP, PLP
    cpu.LoadProgram(program, sizeof(program));
    cpu.SetFlag(CPU::StatusFlag::F_CARRY, true);
    cpu.SetFlag(CPU::StatusFlag::F_ZERO, true);
    
    cpu.Step();  // PHP
    
    cpu.P = 0x00;  // Clear all flags
    cpu.Step();  // PLP
    
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
}

// ============================================================================
// Status Flag Tests
// ============================================================================

TEST_F(OpcodeTest, CLC)
{
    uint8_t program[] = {0x18};  // CLC
    cpu.LoadProgram(program, sizeof(program));
    cpu.SetFlag(CPU::StatusFlag::F_CARRY, true);
    cpu.Step();
    
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
}

TEST_F(OpcodeTest, SEC)
{
    uint8_t program[] = {0x38};  // SEC
    cpu.LoadProgram(program, sizeof(program));
    cpu.Step();
    
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
}

TEST_F(OpcodeTest, CLI)
{
    uint8_t program[] = {0x58};  // CLI
    cpu.LoadProgram(program, sizeof(program));
    cpu.SetFlag(CPU::StatusFlag::F_INTERRUPT, true);
    cpu.Step();
    
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_INTERRUPT));
}

TEST_F(OpcodeTest, SEI)
{
    uint8_t program[] = {0x78};  // SEI
    cpu.LoadProgram(program, sizeof(program));
    cpu.Step();
    
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_INTERRUPT));
}

TEST_F(OpcodeTest, CLV)
{
    uint8_t program[] = {0xB8};  // CLV
    cpu.LoadProgram(program, sizeof(program));
    cpu.SetFlag(CPU::StatusFlag::F_OVERFLOW, true);
    cpu.Step();
    
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_OVERFLOW));
}

TEST_F(OpcodeTest, CLD)
{
    uint8_t program[] = {0xD8};  // CLD
    cpu.LoadProgram(program, sizeof(program));
    cpu.SetFlag(CPU::StatusFlag::F_DECIMAL, true);
    cpu.Step();
    
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_DECIMAL));
}

TEST_F(OpcodeTest, SED)
{
    uint8_t program[] = {0xF8};  // SED
    cpu.LoadProgram(program, sizeof(program));
    cpu.Step();
    
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_DECIMAL));
}

// ============================================================================
// Comparison Tests
// ============================================================================

TEST_F(OpcodeTest, CMP_Equal)
{
    uint8_t program[] = {0xC9, 0x42};  // CMP #$42
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0x42;
    cpu.Step();
    
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));
}

TEST_F(OpcodeTest, CMP_AGreater)
{
    uint8_t program[] = {0xC9, 0x40};  // CMP #$40
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0x42;
    cpu.Step();
    
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));
}

TEST_F(OpcodeTest, CMP_ALess)
{
    uint8_t program[] = {0xC9, 0x50};  // CMP #$50
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0x42;
    cpu.Step();
    
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));
}

TEST_F(OpcodeTest, CPX_Equal)
{
    uint8_t program[] = {0xE0, 0x33};  // CPX #$33
    cpu.LoadProgram(program, sizeof(program));
    cpu.X = 0x33;
    cpu.Step();
    
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
}

TEST_F(OpcodeTest, CPY_Equal)
{
    uint8_t program[] = {0xC0, 0x44};  // CPY #$44
    cpu.LoadProgram(program, sizeof(program));
    cpu.Y = 0x44;
    cpu.Step();
    
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
}
// ============================================================================
// ADC Tests
// ============================================================================

TEST_F(OpcodeTest, ADC_ImmediateNoCarry)
{
    uint8_t program[] = {0x69, 0x05};  // ADC #$05
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0x10;
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0x15);
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_OVERFLOW));
}

TEST_F(OpcodeTest, ADC_ImmediateWithCarry)
{
    uint8_t program[] = {0x69, 0x05};  // ADC #$05
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0x10;
    cpu.SetFlag(CPU::StatusFlag::F_CARRY, true);
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0x16);  // 0x10 + 0x05 + 1
}

TEST_F(OpcodeTest, ADC_SetsCarryFlag)
{
    uint8_t program[] = {0x69, 0xFF};  // ADC #$FF
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0x02;
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0x01);  // Wraps around
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
}

TEST_F(OpcodeTest, ADC_SetsOverflowFlag)
{
    uint8_t program[] = {0x69, 0x7F};  // ADC #$7F (127)
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0x01;  // 1 + 127 = 128 (overflow in signed arithmetic)
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0x80);
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_OVERFLOW));  // Positive + Positive = Negative
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));
}

TEST_F(OpcodeTest, ADC_NoOverflowWhenSignsDiffer)
{
    uint8_t program[] = {0x69, 0x7F};  // ADC #$7F (127)
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0x80;  // -128 + 127 = -1 (no overflow)
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0xFF);
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_OVERFLOW));
}

// ============================================================================
// SBC Tests
// ============================================================================

TEST_F(OpcodeTest, SBC_ImmediateNoBorrow)
{
    uint8_t program[] = {0xE9, 0x05};  // SBC #$05
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0x10;
    cpu.SetFlag(CPU::StatusFlag::F_CARRY, true);  // Carry set = no borrow
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0x0B);
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
}

TEST_F(OpcodeTest, SBC_ImmediateWithBorrow)
{
    uint8_t program[] = {0xE9, 0x05};  // SBC #$05
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0x10;
    cpu.SetFlag(CPU::StatusFlag::F_CARRY, false);  // Carry clear = borrow
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0x0A);  // 0x10 - 0x05 - 1
}

TEST_F(OpcodeTest, SBC_ClearsCarryOnBorrow)
{
    uint8_t program[] = {0xE9, 0x10};  // SBC #$10
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0x05;
    cpu.SetFlag(CPU::StatusFlag::F_CARRY, true);
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0xF5);  // Wraps around
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
}

TEST_F(OpcodeTest, SBC_SetsOverflowFlag)
{
    uint8_t program[] = {0xE9, 0x01};  // SBC #$01
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0x80;  // -128 - 1 = -129 (overflow)
    cpu.SetFlag(CPU::StatusFlag::F_CARRY, true);
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0x7F);
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_OVERFLOW));
}

// ============================================================================
// INC/DEC Tests
// ============================================================================

TEST_F(OpcodeTest, INC_ZeroPage)
{
    uint8_t program[] = {0xE6, 0x10};  // INC $10
    cpu.LoadProgram(program, sizeof(program));
    cpu.WriteMemory(0x10, 0x42);
    cpu.Step();
    
    EXPECT_EQ(cpu.ReadMemory(0x10), 0x43);
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));
}

TEST_F(OpcodeTest, INC_Wraps)
{
    uint8_t program[] = {0xE6, 0x10};  // INC $10
    cpu.LoadProgram(program, sizeof(program));
    cpu.WriteMemory(0x10, 0xFF);
    cpu.Step();
    
    EXPECT_EQ(cpu.ReadMemory(0x10), 0x00);
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
}

TEST_F(OpcodeTest, DEC_ZeroPage)
{
    uint8_t program[] = {0xC6, 0x10};  // DEC $10
    cpu.LoadProgram(program, sizeof(program));
    cpu.WriteMemory(0x10, 0x42);
    cpu.Step();
    
    EXPECT_EQ(cpu.ReadMemory(0x10), 0x41);
}

TEST_F(OpcodeTest, DEC_Wraps)
{
    uint8_t program[] = {0xC6, 0x10};  // DEC $10
    cpu.LoadProgram(program, sizeof(program));
    cpu.WriteMemory(0x10, 0x00);
    cpu.Step();
    
    EXPECT_EQ(cpu.ReadMemory(0x10), 0xFF);
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));
}

TEST_F(OpcodeTest, INX)
{
    uint8_t program[] = {0xE8};  // INX
    cpu.LoadProgram(program, sizeof(program));
    cpu.X = 0x10;
    cpu.Step();
    
    EXPECT_EQ(cpu.X, 0x11);
}

TEST_F(OpcodeTest, DEX)
{
    uint8_t program[] = {0xCA};  // DEX
    cpu.LoadProgram(program, sizeof(program));
    cpu.X = 0x10;
    cpu.Step();
    
    EXPECT_EQ(cpu.X, 0x0F);
}

TEST_F(OpcodeTest, INY)
{
    uint8_t program[] = {0xC8};  // INY
    cpu.LoadProgram(program, sizeof(program));
    cpu.Y = 0x10;
    cpu.Step();
    
    EXPECT_EQ(cpu.Y, 0x11);
}

TEST_F(OpcodeTest, DEY)
{
    uint8_t program[] = {0x88};  // DEY
    cpu.LoadProgram(program, sizeof(program));
    cpu.Y = 0x10;
    cpu.Step();
    
    EXPECT_EQ(cpu.Y, 0x0F);
}

// ============================================================================
// Logical Operations Tests
// ============================================================================

TEST_F(OpcodeTest, AND_Immediate)
{
    uint8_t program[] = {0x29, 0x0F};  // AND #$0F
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0xFF;
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0x0F);
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));
}

TEST_F(OpcodeTest, AND_SetsZeroFlag)
{
    uint8_t program[] = {0x29, 0x00};  // AND #$00
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0xFF;
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
}

TEST_F(OpcodeTest, ORA_Immediate)
{
    uint8_t program[] = {0x09, 0x0F};  // ORA #$0F
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0xF0;
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0xFF);
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));
}

TEST_F(OpcodeTest, EOR_Immediate)
{
    uint8_t program[] = {0x49, 0xFF};  // EOR #$FF
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0xAA;
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0x55);  // 0xAA XOR 0xFF = 0x55
}

TEST_F(OpcodeTest, EOR_SetsZeroFlag)
{
    uint8_t program[] = {0x49, 0xFF};  // EOR #$FF
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0xFF;
    cpu.Step();
    
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
}

TEST_F(OpcodeTest, BIT_ZeroPage)
{
    uint8_t program[] = {0x24, 0x10};  // BIT $10
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0xFF;
    cpu.WriteMemory(0x10, 0xC0);  // Bit 7 and 6 set
    cpu.Step();
    
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));  // A & value != 0
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));  // Bit 7 of value
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_OVERFLOW));  // Bit 6 of value
}

TEST_F(OpcodeTest, BIT_SetsZeroFlag)
{
    uint8_t program[] = {0x24, 0x10};  // BIT $10
    cpu.LoadProgram(program, sizeof(program));
    cpu.A = 0x0F;
    cpu.WriteMemory(0x10, 0xF0);
    cpu.Step();
    
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));  // 0x0F & 0xF0 = 0
}

// ============================================================================
// System Tests
// ============================================================================

TEST_F(OpcodeTest, NOP)
{
    uint8_t program[] = {0xEA};  // NOP
    cpu.LoadProgram(program, sizeof(program));
    uint16_t oldPC = cpu.PC;
    cpu.Step();
    
    EXPECT_EQ(cpu.PC, oldPC + 1);
}

// ------------------------------------
int main()
{
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}