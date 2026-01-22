#include <gtest/gtest.h>
#include "cpu/CPU.h"
#include "bus/Bus.h"
#include "memory/Memory.h"


class OpcodeTest : public ::testing::Test {
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
    
    void LoadAndExecute(const std::vector<uint8_t>& program)
    {
        for (size_t i = 0; i < program.size(); i++)
        {
            cpu.WriteMemory(0x8000 + i, program[i]);
        }
        cpu.PC = 0x8000;
        cpu.Step();
    }
};

// ============================================================================
// LOAD INSTRUCTIONS - LDA, LDX, LDY (all addressing modes)
// ============================================================================

TEST_F(OpcodeTest, LDA_IMM_0xA9)
{
    LoadAndExecute({0xA9, 0x42});  // LDA #$42
    EXPECT_EQ(cpu.A, 0x42);
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 2));
}

TEST_F(OpcodeTest, LDA_ZP0_0xA5)
{
    cpu.WriteMemory(0x10, 0x55);
    LoadAndExecute({0xA5, 0x10});  // LDA $10
    EXPECT_EQ(cpu.A, 0x55);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 3));
}

TEST_F(OpcodeTest, LDA_ZPX_0xB5)
{
    cpu.X = 0x05;
    cpu.WriteMemory(0x15, 0xAA);
    LoadAndExecute({0xB5, 0x10});  // LDA $10,X
    EXPECT_EQ(cpu.A, 0xAA);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 4));
}

TEST_F(OpcodeTest, LDA_ABS_0xAD)
{
    cpu.WriteMemory(0x1234, 0x99);
    LoadAndExecute({0xAD, 0x34, 0x12});  // LDA $1234
    EXPECT_EQ(cpu.A, 0x99);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 4));
}

TEST_F(OpcodeTest, LDA_ABX_0xBD_NoPageCross)
{
    cpu.X = 0x05;
    cpu.WriteMemory(0x1205, 0x77);
    LoadAndExecute({0xBD, 0x00, 0x12});  // LDA $1200,X
    EXPECT_EQ(cpu.A, 0x77);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 4));
}

TEST_F(OpcodeTest, LDA_ABX_0xBD_PageCross)
{
    cpu.X = 0x05;
    cpu.WriteMemory(0x1304, 0x88);
    LoadAndExecute({0xBD, 0xFF, 0x12});  // LDA $12FF,X
    EXPECT_EQ(cpu.A, 0x88);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 5));  // +1 for page cross
}

TEST_F(OpcodeTest, LDA_ABY_0xB9_PageCross)
{
    cpu.Y = 0x10;
    cpu.WriteMemory(0x1305, 0xBB);
    LoadAndExecute({0xB9, 0xF5, 0x12});  // LDA $12F5,Y
    EXPECT_EQ(cpu.A, 0xBB);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 5));
}

TEST_F(OpcodeTest, LDA_IZX_0xA1)
{
    cpu.X = 0x05;
    cpu.WriteMemory(0x45, 0x34);
    cpu.WriteMemory(0x46, 0x12);
    cpu.WriteMemory(0x1234, 0xCC);
    LoadAndExecute({0xA1, 0x40});  // LDA ($40,X)
    EXPECT_EQ(cpu.A, 0xCC);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 6));
}

TEST_F(OpcodeTest, LDA_IZY_0xB1_PageCross)
{
    cpu.Y = 0x10;
    cpu.WriteMemory(0x40, 0xF0);
    cpu.WriteMemory(0x41, 0x12);
    cpu.WriteMemory(0x1300, 0xDD);
    LoadAndExecute({0xB1, 0x40});  // LDA ($40),Y
    EXPECT_EQ(cpu.A, 0xDD);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 6));  // +1 for page cross
}

TEST_F(OpcodeTest, LDX_IMM_0xA2)
{
    LoadAndExecute({0xA2, 0x55});  // LDX #$55
    EXPECT_EQ(cpu.X, 0x55);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 2));
}

TEST_F(OpcodeTest, LDX_ZP0_0xA6)
{
    cpu.WriteMemory(0x20, 0x33);
    LoadAndExecute({0xA6, 0x20});  // LDX $20
    EXPECT_EQ(cpu.X, 0x33);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 3));
}

TEST_F(OpcodeTest, LDX_ZPY_0xB6)
{
    cpu.Y = 0x05;
    cpu.WriteMemory(0x25, 0x44);
    LoadAndExecute({0xB6, 0x20});  // LDX $20,Y
    EXPECT_EQ(cpu.X, 0x44);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 4));
}

TEST_F(OpcodeTest, LDX_ABS_0xAE)
{
    cpu.WriteMemory(0x1234, 0x66);
    LoadAndExecute({0xAE, 0x34, 0x12});  // LDX $1234
    EXPECT_EQ(cpu.X, 0x66);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 4));
}

TEST_F(OpcodeTest, LDX_ABY_0xBE)
{
    cpu.Y = 0x10;
    cpu.WriteMemory(0x1210, 0x77);
    LoadAndExecute({0xBE, 0x00, 0x12});  // LDX $1200,Y
    EXPECT_EQ(cpu.X, 0x77);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 4));
}

TEST_F(OpcodeTest, LDY_IMM_0xA0)
{
    LoadAndExecute({0xA0, 0x66});  // LDY #$66
    EXPECT_EQ(cpu.Y, 0x66);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 2));
}

TEST_F(OpcodeTest, LDY_ZP0_0xA4)
{
    cpu.WriteMemory(0x30, 0x88);
    LoadAndExecute({0xA4, 0x30});  // LDY $30
    EXPECT_EQ(cpu.Y, 0x88);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 3));
}

TEST_F(OpcodeTest, LDY_ZPX_0xB4)
{
    cpu.X = 0x03;
    cpu.WriteMemory(0x23, 0x77);
    LoadAndExecute({0xB4, 0x20});  // LDY $20,X
    EXPECT_EQ(cpu.Y, 0x77);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 4));
}

TEST_F(OpcodeTest, LDY_ABS_0xAC)
{
    cpu.WriteMemory(0x1234, 0x99);
    LoadAndExecute({0xAC, 0x34, 0x12});  // LDY $1234
    EXPECT_EQ(cpu.Y, 0x99);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 4));
}

TEST_F(OpcodeTest, LDY_ABX_0xBC)
{
    cpu.X = 0x05;
    cpu.WriteMemory(0x1205, 0xAA);
    LoadAndExecute({0xBC, 0x00, 0x12});  // LDY $1200,X
    EXPECT_EQ(cpu.Y, 0xAA);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 4));
}

// ============================================================================
// STORE INSTRUCTIONS - STA, STX, STY
// ============================================================================

TEST_F(OpcodeTest, STA_ZP0_0x85)
{
    cpu.A = 0x42;
    LoadAndExecute({0x85, 0x10});  // STA $10
    EXPECT_EQ(cpu.ReadMemory(0x10), 0x42);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 3));
}

TEST_F(OpcodeTest, STA_ZPX_0x95)
{
    cpu.A = 0x55;
    cpu.X = 0x05;
    LoadAndExecute({0x95, 0x10});  // STA $10,X
    EXPECT_EQ(cpu.ReadMemory(0x15), 0x55);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 4));
}

TEST_F(OpcodeTest, STA_ABS_0x8D)
{
    cpu.A = 0xAA;
    LoadAndExecute({0x8D, 0x34, 0x12});  // STA $1234
    EXPECT_EQ(cpu.ReadMemory(0x1234), 0xAA);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 4));
}

TEST_F(OpcodeTest, STA_ABX_0x9D_PageCross)
{
    cpu.A = 0xBB;
    cpu.X = 0x10;
    LoadAndExecute({0x9D, 0xF0, 0x12});  // STA $12F0,X
    EXPECT_EQ(cpu.ReadMemory(0x1300), 0xBB);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 5));  // No extra cycle for writes!
}

TEST_F(OpcodeTest, STA_ABY_0x99)
{
    cpu.A = 0xCC;
    cpu.Y = 0x08;
    LoadAndExecute({0x99, 0x00, 0x12});  // STA $1200,Y
    EXPECT_EQ(cpu.ReadMemory(0x1208), 0xCC);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 5));
}

TEST_F(OpcodeTest, STA_IZX_0x81)
{
    cpu.A = 0xDD;
    cpu.X = 0x05;
    cpu.WriteMemory(0x45, 0x34);
    cpu.WriteMemory(0x46, 0x12);
    LoadAndExecute({0x81, 0x40});  // STA ($40,X)
    EXPECT_EQ(cpu.ReadMemory(0x1234), 0xDD);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 6));
}

TEST_F(OpcodeTest, STA_IZY_0x91)
{
    cpu.A = 0xEE;
    cpu.Y = 0x10;
    cpu.WriteMemory(0x40, 0x00);
    cpu.WriteMemory(0x41, 0x12);
    LoadAndExecute({0x91, 0x40});  // STA ($40),Y
    EXPECT_EQ(cpu.ReadMemory(0x1210), 0xEE);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 6));
}

TEST_F(OpcodeTest, STX_ZP0_0x86)
{
    cpu.X = 0x99;
    LoadAndExecute({0x86, 0x10});  // STX $10
    EXPECT_EQ(cpu.ReadMemory(0x10), 0x99);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 3));
}

TEST_F(OpcodeTest, STX_ZPY_0x96)
{
    cpu.X = 0x88;
    cpu.Y = 0x03;
    LoadAndExecute({0x96, 0x10});  // STX $10,Y
    EXPECT_EQ(cpu.ReadMemory(0x13), 0x88);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 4));
}

TEST_F(OpcodeTest, STX_ABS_0x8E)
{
    cpu.X = 0x77;
    LoadAndExecute({0x8E, 0x34, 0x12});  // STX $1234
    EXPECT_EQ(cpu.ReadMemory(0x1234), 0x77);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 4));
}

TEST_F(OpcodeTest, STY_ZP0_0x84)
{
    cpu.Y = 0x77;
    LoadAndExecute({0x84, 0x10});  // STY $10
    EXPECT_EQ(cpu.ReadMemory(0x10), 0x77);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 3));
}

TEST_F(OpcodeTest, STY_ZPX_0x94)
{
    cpu.Y = 0x66;
    cpu.X = 0x02;
    LoadAndExecute({0x94, 0x10});  // STY $10,X
    EXPECT_EQ(cpu.ReadMemory(0x12), 0x66);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 4));
}

TEST_F(OpcodeTest, STY_ABS_0x8C)
{
    cpu.Y = 0x55;
    LoadAndExecute({0x8C, 0x34, 0x12});  // STY $1234
    EXPECT_EQ(cpu.ReadMemory(0x1234), 0x55);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 4));
}

// ============================================================================
// TRANSFER INSTRUCTIONS
// ============================================================================

TEST_F(OpcodeTest, TAX_0xAA)
{
    cpu.A = 0x42;
    LoadAndExecute({0xAA});  // TAX
    EXPECT_EQ(cpu.X, 0x42);
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 2));
}

TEST_F(OpcodeTest, TAX_0xAA_Zero)
{
    cpu.A = 0x00;
    LoadAndExecute({0xAA});  // TAX
    EXPECT_EQ(cpu.X, 0x00);
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
}

TEST_F(OpcodeTest, TAX_0xAA_Negative)
{
    cpu.A = 0x80;
    LoadAndExecute({0xAA});  // TAX
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));
}

TEST_F(OpcodeTest, TAY_0xA8)
{
    cpu.A = 0x55;
    LoadAndExecute({0xA8});  // TAY
    EXPECT_EQ(cpu.Y, 0x55);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 2));
}

TEST_F(OpcodeTest, TXA_0x8A)
{
    cpu.X = 0x33;
    LoadAndExecute({0x8A});  // TXA
    EXPECT_EQ(cpu.A, 0x33);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 2));
}

TEST_F(OpcodeTest, TYA_0x98)
{
    cpu.Y = 0x44;
    LoadAndExecute({0x98});  // TYA
    EXPECT_EQ(cpu.A, 0x44);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 2));
}

TEST_F(OpcodeTest, TSX_0xBA)
{
    cpu.SP = 0xF0;
    LoadAndExecute({0xBA});  // TSX
    EXPECT_EQ(cpu.X, 0xF0);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 2));
}

TEST_F(OpcodeTest, TXS_0x9A)
{
    cpu.X = 0xA0;
    LoadAndExecute({0x9A});  // TXS
    EXPECT_EQ(cpu.SP, 0xA0);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 2));
}

// ============================================================================
// ARITHMETIC - ADC
// ============================================================================

TEST_F(OpcodeTest, ADC_IMM_0x69_NoCarry)
{
    cpu.A = 0x10;
    LoadAndExecute({0x69, 0x05});  // ADC #$05
    EXPECT_EQ(cpu.A, 0x15);
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_OVERFLOW));
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 2));
}

TEST_F(OpcodeTest, ADC_IMM_0x69_WithCarry)
{
    cpu.A = 0x10;
    cpu.SetFlag(CPU::StatusFlag::F_CARRY, true);
    LoadAndExecute({0x69, 0x05});  // ADC #$05
    EXPECT_EQ(cpu.A, 0x16);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 2));
}

TEST_F(OpcodeTest, ADC_IMM_0x69_SetsCarry)
{
    cpu.A = 0xFF;
    LoadAndExecute({0x69, 0x02});  // ADC #$02
    EXPECT_EQ(cpu.A, 0x01);
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
}

TEST_F(OpcodeTest, ADC_IMM_0x69_Overflow)
{
    cpu.A = 0x7F;  // 127
    LoadAndExecute({0x69, 0x01});  // ADC #$01
    EXPECT_EQ(cpu.A, 0x80);
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_OVERFLOW));
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));
}

TEST_F(OpcodeTest, ADC_ZP0_0x65)
{
    cpu.A = 0x20;
    cpu.WriteMemory(0x10, 0x30);
    LoadAndExecute({0x65, 0x10});  // ADC $10
    EXPECT_EQ(cpu.A, 0x50);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 3));
}

TEST_F(OpcodeTest, ADC_ZPX_0x75)
{
    cpu.A = 0x15;
    cpu.X = 0x05;
    cpu.WriteMemory(0x15, 0x25);
    LoadAndExecute({0x75, 0x10});  // ADC $10,X
    EXPECT_EQ(cpu.A, 0x3A);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 4));
}

TEST_F(OpcodeTest, ADC_ABS_0x6D)
{
    cpu.A = 0x40;
    cpu.WriteMemory(0x1234, 0x50);
    LoadAndExecute({0x6D, 0x34, 0x12});  // ADC $1234
    EXPECT_EQ(cpu.A, 0x90);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 4));
}

TEST_F(OpcodeTest, ADC_ABX_0x7D)
{
    cpu.A = 0x10;
    cpu.X = 0x05;
    cpu.WriteMemory(0x1205, 0x20);
    LoadAndExecute({0x7D, 0x00, 0x12});  // ADC $1200,X
    EXPECT_EQ(cpu.A, 0x30);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 4));
}

TEST_F(OpcodeTest, ADC_ABY_0x79)
{
    cpu.A = 0x11;
    cpu.Y = 0x03;
    cpu.WriteMemory(0x1203, 0x22);
    LoadAndExecute({0x79, 0x00, 0x12});  // ADC $1200,Y
    EXPECT_EQ(cpu.A, 0x33);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 4));
}

TEST_F(OpcodeTest, ADC_IZX_0x61)
{
    cpu.A = 0x12;
    cpu.X = 0x05;
    cpu.WriteMemory(0x45, 0x34);
    cpu.WriteMemory(0x46, 0x12);
    cpu.WriteMemory(0x1234, 0x34);
    LoadAndExecute({0x61, 0x40});  // ADC ($40,X)
    EXPECT_EQ(cpu.A, 0x46);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 6));
}

TEST_F(OpcodeTest, ADC_IZY_0x71)
{
    cpu.A = 0x20;
    cpu.Y = 0x10;
    cpu.WriteMemory(0x40, 0x00);
    cpu.WriteMemory(0x41, 0x12);
    cpu.WriteMemory(0x1210, 0x30);
    LoadAndExecute({0x71, 0x40});  // ADC ($40),Y
    EXPECT_EQ(cpu.A, 0x50);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 5));
}

// ============================================================================
// ARITHMETIC - SBC
// ============================================================================

TEST_F(OpcodeTest, SBC_IMM_0xE9)
{
    cpu.A = 0x10;
    cpu.SetFlag(CPU::StatusFlag::F_CARRY, true);  // No borrow
    LoadAndExecute({0xE9, 0x05});  // SBC #$05
    EXPECT_EQ(cpu.A, 0x0B);
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 2));
}

TEST_F(OpcodeTest, SBC_IMM_0xE9_WithBorrow)
{
    cpu.A = 0x10;
    cpu.SetFlag(CPU::StatusFlag::F_CARRY, false);  // Borrow
    LoadAndExecute({0xE9, 0x05});  // SBC #$05
    EXPECT_EQ(cpu.A, 0x0A);
}

TEST_F(OpcodeTest, SBC_IMM_0xE9_Underflow)
{
    cpu.A = 0x05;
    cpu.SetFlag(CPU::StatusFlag::F_CARRY, true);
    LoadAndExecute({0xE9, 0x10});  // SBC #$10
    EXPECT_EQ(cpu.A, 0xF5);
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
}

TEST_F(OpcodeTest, SBC_ZP0_0xE5)
{
    cpu.A = 0x50;
    cpu.SetFlag(CPU::StatusFlag::F_CARRY, true);
    cpu.WriteMemory(0x10, 0x20);
    LoadAndExecute({0xE5, 0x10});  // SBC $10
    EXPECT_EQ(cpu.A, 0x30);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 3));
}

TEST_F(OpcodeTest, SBC_ZPX_0xF5)
{
    cpu.A = 0x40;
    cpu.X = 0x05;
    cpu.SetFlag(CPU::StatusFlag::F_CARRY, true);
    cpu.WriteMemory(0x15, 0x15);
    LoadAndExecute({0xF5, 0x10});  // SBC $10,X
    EXPECT_EQ(cpu.A, 0x2B);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 4));
}

TEST_F(OpcodeTest, SBC_ABS_0xED)
{
    cpu.A = 0x80;
    cpu.SetFlag(CPU::StatusFlag::F_CARRY, true);
    cpu.WriteMemory(0x1234, 0x30);
    LoadAndExecute({0xED, 0x34, 0x12});  // SBC $1234
    EXPECT_EQ(cpu.A, 0x50);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 4));
}

TEST_F(OpcodeTest, SBC_ABX_0xFD)
{
    cpu.A = 0x60;
    cpu.X = 0x05;
    cpu.SetFlag(CPU::StatusFlag::F_CARRY, true);
    cpu.WriteMemory(0x1205, 0x20);
    LoadAndExecute({0xFD, 0x00, 0x12});  // SBC $1200,X
    EXPECT_EQ(cpu.A, 0x40);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 4));
}

TEST_F(OpcodeTest, SBC_ABY_0xF9)
{
    cpu.A = 0x70;
    cpu.Y = 0x03;
    cpu.SetFlag(CPU::StatusFlag::F_CARRY, true);
    cpu.WriteMemory(0x1203, 0x30);
    LoadAndExecute({0xF9, 0x00, 0x12});  // SBC $1200,Y
    EXPECT_EQ(cpu.A, 0x40);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 4));
}

TEST_F(OpcodeTest, SBC_IZX_0xE1)
{
    cpu.A = 0x50;
    cpu.X = 0x05;
    cpu.SetFlag(CPU::StatusFlag::F_CARRY, true);
    cpu.WriteMemory(0x45, 0x34);
    cpu.WriteMemory(0x46, 0x12);
    cpu.WriteMemory(0x1234, 0x20);
    LoadAndExecute({0xE1, 0x40});  // SBC ($40,X)
    EXPECT_EQ(cpu.A, 0x30);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 6));
}

TEST_F(OpcodeTest, SBC_IZY_0xF1)
{
    cpu.A = 0x60;
    cpu.Y = 0x10;
    cpu.SetFlag(CPU::StatusFlag::F_CARRY, true);
    cpu.WriteMemory(0x40, 0x00);
    cpu.WriteMemory(0x41, 0x12);
    cpu.WriteMemory(0x1210, 0x30);
    LoadAndExecute({0xF1, 0x40});  // SBC ($40),Y
    EXPECT_EQ(cpu.A, 0x30);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 5));
}

// Note: 0xEB is unofficial SBC
TEST_F(OpcodeTest, SBC_IMM_0xEB_Unofficial)
{
    cpu.A = 0x20;
    cpu.SetFlag(CPU::StatusFlag::F_CARRY, true);
    LoadAndExecute({0xEB, 0x10});  // SBC #$10 (unofficial)
    EXPECT_EQ(cpu.A, 0x10);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 2));
}

// ============================================================================
// INCREMENT/DECREMENT - INC, DEC, INX, DEX, INY, DEY
// ============================================================================

TEST_F(OpcodeTest, INC_ZP0_0xE6)
{
    cpu.WriteMemory(0x10, 0x42);
    LoadAndExecute({0xE6, 0x10});  // INC $10
    EXPECT_EQ(cpu.ReadMemory(0x10), 0x43);
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 5));
}

TEST_F(OpcodeTest, INC_ZP0_0xE6_Wrap)
{
    cpu.WriteMemory(0x10, 0xFF);
    LoadAndExecute({0xE6, 0x10});  // INC $10
    EXPECT_EQ(cpu.ReadMemory(0x10), 0x00);
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
}

TEST_F(OpcodeTest, INC_ZPX_0xF6)
{
    cpu.X = 0x05;
    cpu.WriteMemory(0x15, 0x50);
    LoadAndExecute({0xF6, 0x10});  // INC $10,X
    EXPECT_EQ(cpu.ReadMemory(0x15), 0x51);
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 6));
}

TEST_F(OpcodeTest, INC_ABS_0xEE)
{
    cpu.WriteMemory(0x1234, 0x7F);
    LoadAndExecute({0xEE, 0x34, 0x12});  // INC $1234
    EXPECT_EQ(cpu.ReadMemory(0x1234), 0x80);
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));
    EXPECT_EQ(cpu.GetTotalCycles(), (uint64_t)(7 + 6));
}

// TEST_F(OpcodeTest, INC_ABX_0xFE)
// {
//     cpu.X = 0x05;
//     cpu.WriteMemory(0x1205, 0

// ------------------------------------
int main()
{
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
