#include "TestFixture.h"


// Test status flag operations
TEST_F(OpcodeTest, SetFlagCarry)
{
    cpu.SetFlag(CPU::StatusFlag::F_CARRY, true);
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));

    cpu.SetFlag(CPU::StatusFlag::F_CARRY, false);
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
}

TEST_F(OpcodeTest, SetFlagZero)
{
    cpu.SetFlag(CPU::StatusFlag::F_ZERO, true);
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));

    cpu.SetFlag(CPU::StatusFlag::F_ZERO, false);
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
}

TEST_F(OpcodeTest, SetFlagNegative)
{
    cpu.SetFlag(CPU::StatusFlag::F_NEGATIVE, true);
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));

    cpu.SetFlag(CPU::StatusFlag::F_NEGATIVE, false);
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));
}

TEST_F(OpcodeTest, SetFlagOverflow)
{
    cpu.SetFlag(CPU::StatusFlag::F_OVERFLOW, true);
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_OVERFLOW));

    cpu.SetFlag(CPU::StatusFlag::F_OVERFLOW, false);
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_OVERFLOW));
}

TEST_F(OpcodeTest, UpdateZN_Zero)
{
    cpu.UpdateZN(0);
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));
}

TEST_F(OpcodeTest, UpdateZN_Positive)
{
    cpu.UpdateZN(0x7F);
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));
}

TEST_F(OpcodeTest, UpdateZN_Negative)
{
    cpu.UpdateZN(0x80);
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));
}

// Test stack operations
TEST_F(OpcodeTest, PushPopStack)
{
    uint8_t value = 0x42;

    EXPECT_EQ(cpu.SP, 0xFD);
    cpu.PushStack(value);
    EXPECT_EQ(cpu.SP, 0xFC);
    
    uint8_t result = cpu.PopStack();
    EXPECT_EQ(result, value);
    EXPECT_EQ(cpu.SP, 0xFD);
}

TEST_F(OpcodeTest, PushPopStack16)
{
    uint16_t value = 0x1234;

    EXPECT_EQ(cpu.SP, 0xFD);
    cpu.PushStack16(value);
    EXPECT_EQ(cpu.SP, 0xFB);
    
    uint16_t result = cpu.PopStack16();
    EXPECT_EQ(result, value);
    EXPECT_EQ(cpu.SP, 0xFD);
}

TEST_F(OpcodeTest, StackUnderflow)
{
    // Push multiple values
    for (int i = 0; i < 10; ++i)
    {
        cpu.PushStack(static_cast<uint8_t>(i));
    }
    EXPECT_EQ(cpu.SP, 0xF3); // 10 pushes from 0xFD

    // Pop multiple values
    for (int i = 9; i >= 0; --i)
    {
        EXPECT_EQ(cpu.PopStack(), i);
    }
    EXPECT_EQ(cpu.SP, 0xFD); // Back to initial position
    
    // Pop once more to test underflow
    cpu.SP = 0xFF; // Manually set SP to top of stack
    uint8_t result = cpu.PopStack();
    EXPECT_EQ(result, 0x00); // Should read whatever is at 0x01FF
    EXPECT_EQ(cpu.SP, 0x00); // SP wraps around
}

// Test memory operations
TEST_F(OpcodeTest, MemoryRW)
{
    uint16_t address = 0x2000;
    uint8_t value = 0xAB;

    cpu.WriteMemory(address, value);
    uint8_t result = cpu.ReadMemory(address);

    EXPECT_EQ(result, value);
}


TEST_F(OpcodeTest, LoadProgram)
{
    uint8_t program[] = { 0xA9, 0x01, 0x00 }; // LDA #$01; BRK
    cpu.LoadProgram(program, sizeof(program), 0x8000);

    EXPECT_EQ(cpu.PC, 0x8000);
    EXPECT_EQ(cpu.ReadMemory(0x8000), 0xA9);
    EXPECT_EQ(cpu.ReadMemory(0x8001), 0x01);
    EXPECT_EQ(cpu.ReadMemory(0x8002), 0x00);
}

// Test Reset functionality
TEST_F(OpcodeTest, Reset)
{
    cpu.A = 0xFF;
    cpu.X = 0xFF;
    cpu.Y = 0xFF;
    cpu.SP = 0x00;
    cpu.P = 0x00;
    cpu.PC = 0x1234;

    // Write reset vector
    cpu.WriteMemory(0xFFFC, 0x00);
    cpu.WriteMemory(0xFFFD, 0x80);

    cpu.Reset();

    EXPECT_EQ(cpu.A, 0);
    EXPECT_EQ(cpu.X, 0);
    EXPECT_EQ(cpu.Y, 0);
    EXPECT_EQ(cpu.P & CPU::StatusFlag::F_UNUSED, CPU::StatusFlag::F_UNUSED);
    EXPECT_EQ(cpu.P & CPU::StatusFlag::F_INTERRUPT, CPU::StatusFlag::F_INTERRUPT);
    EXPECT_EQ(cpu.SP, 0xFD);
    EXPECT_EQ(cpu.PC, 0x8000);
    EXPECT_EQ(cpu.GetCycles(), (uint64_t) 7);
}
