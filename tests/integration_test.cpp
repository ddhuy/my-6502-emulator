#include <gtest/gtest.h>
#include "cpu/CPU.h"
#include "bus/Bus.h"
#include "memory/Memory.h"


class IntegrationTest : public ::testing::Test
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

// Simple program: Load values, add them, store result
TEST_F(IntegrationTest, SimpleAdditionProgram)
{
    uint8_t program[] = {
        0xA9, 0x10,        // LDA #$10
        0x69, 0x20,        // ADC #$20
        0x85, 0x30,        // STA $30
        0xEA               // NOP
    };
    
    cpu.LoadProgram(program, sizeof(program));
    
    // Execute program
    cpu.Step();  // LDA #$10
    EXPECT_EQ(cpu.A, 0x10);
    
    cpu.Step();  // ADC #$20
    EXPECT_EQ(cpu.A, 0x30);
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
    
    cpu.Step();  // STA $30
    EXPECT_EQ(cpu.ReadMemory(0x30), 0x30);
    
    cpu.Step();  // NOP
    EXPECT_EQ(cpu.PC, 0x8007);
}

// Test loop: Count from 0 to 10
TEST_F(IntegrationTest, CountingLoop)
{
    uint8_t program[] = {
        0xA2, 0x00,        // LDX #$00        ; 0x8000
        0xE8,              // INX             ; 0x8002
        0xE0, 0x0A,        // CPX #$0A        ; 0x8003
        0xD0, 0xFB,        // BNE -5          ; 0x8005 (branch to 0x8002)
        0xEA               // NOP             ; 0x8007
    };
    
    cpu.LoadProgram(program, sizeof(program));
    
    cpu.Step();  // LDX #$00
    EXPECT_EQ(cpu.X, 0x00);
    
    // Run the loop
    for (uint8_t loopCount = 1; loopCount <= 10; loopCount++ )
    {
        cpu.Step();  // INX
        EXPECT_EQ(cpu.X, loopCount);
        
        cpu.Step();  // CPX #$0A
        
        if (cpu.X < 10)
        {
            cpu.Step();  // BNE (branch taken)
            EXPECT_EQ(cpu.PC, 0x8002);
        }
        else
        {
            cpu.Step();  // BNE (branch not taken)
            EXPECT_EQ(cpu.PC, 0x8007);
        }
    }
    
    EXPECT_EQ(cpu.X, 10);
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));
}

// Test subroutine call and return
TEST_F(IntegrationTest, SubroutineCall)
{
    uint8_t program[] = {
        0x20, 0x06, 0x80,  // JSR $8006       ; 0x8000
        0xEA,              // NOP             ; 0x8003
        0xEA,              // NOP             ; 0x8004
        0x00,              // BRK             ; 0x8005
        // Subroutine at 0x8006:
        0xA9, 0x42,        // LDA #$42        ; 0x8006
        0x60               // RTS             ; 0x8008
    };
    
    cpu.LoadProgram(program, sizeof(program));
    
    uint8_t initialSP = cpu.SP;
    
    cpu.Step();  // JSR $8006
    EXPECT_EQ(cpu.PC, 0x8006);
    EXPECT_EQ(cpu.SP, initialSP - 2);  // Return address pushed
    
    cpu.Step();  // LDA #$42
    EXPECT_EQ(cpu.A, 0x42);
    
    cpu.Step();  // RTS
    EXPECT_EQ(cpu.PC, 0x8003);  // Returned to next instruction after JSR
    EXPECT_EQ(cpu.SP, initialSP);  // Stack restored
}

// Test stack operations
TEST_F(IntegrationTest, StackOperations)
{
    uint8_t program[] = {
        0xA9, 0x55,        // LDA #$55
        0x48,              // PHA
        0xA9, 0xAA,        // LDA #$AA
        0x68,              // PLA
        0xEA               // NOP
    };
    
    cpu.LoadProgram(program, sizeof(program));
    
    cpu.Step();  // LDA #$55
    EXPECT_EQ(cpu.A, 0x55);
    
    cpu.Step();  // PHA
    EXPECT_EQ(cpu.ReadMemory(0x01FD), 0x55);
    
    cpu.Step();  // LDA #$AA
    EXPECT_EQ(cpu.A, 0xAA);
    
    cpu.Step();  // PLA
    EXPECT_EQ(cpu.A, 0x55);  // Restored from stack
}

// Test flag preservation
TEST_F(IntegrationTest, FlagOperations)
{
    uint8_t program[] = {
        0x38,              // SEC
        0x08,              // PHP
        0x18,              // CLC
        0x28,              // PLP
        0xEA               // NOP
    };
    
    cpu.LoadProgram(program, sizeof(program));
    
    cpu.Step();  // SEC
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
    
    cpu.Step();  // PHP
    
    cpu.Step();  // CLC
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
    
    cpu.Step();  // PLP
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));  // Restored from stack
}

// Test overflow detection
TEST_F(IntegrationTest, OverflowDetection)
{
    uint8_t program[] = {
        0xA9, 0x7F,        // LDA #$7F (127)
        0x69, 0x01,        // ADC #$01 (should overflow)
        0xEA               // NOP
    };
    
    cpu.LoadProgram(program, sizeof(program));
    
    cpu.Step();  // LDA #$7F
    cpu.Step();  // ADC #$01
    
    EXPECT_EQ(cpu.A, 0x80);
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_OVERFLOW));  // Positive + Positive = Negative
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_NEGATIVE));
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));
}

// Test all comparison operations
TEST_F(IntegrationTest, ComparisonOperations)
{
    uint8_t program[] = {
        0xA9, 0x50,        // LDA #$50
        0xC9, 0x50,        // CMP #$50 (equal)
        0xA2, 0x30,        // LDX #$30
        0xE0, 0x20,        // CPX #$20 (greater)
        0xA0, 0x10,        // LDY #$10
        0xC0, 0x20,        // CPY #$20 (less)
        0xEA               // NOP
    };
    
    cpu.LoadProgram(program, sizeof(program));
    
    cpu.Step();  // LDA #$50
    cpu.Step();  // CMP #$50
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));   // Equal
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));  // A >= value
    
    cpu.Step();  // LDX #$30
    cpu.Step();  // CPX #$20
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));  // Not equal
    EXPECT_TRUE(cpu.GetFlag(CPU::StatusFlag::F_CARRY));  // X >= value
    
    cpu.Step();  // LDY #$10
    cpu.Step();  // CPY #$20
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_ZERO));  // Not equal
    EXPECT_FALSE(cpu.GetFlag(CPU::StatusFlag::F_CARRY)); // Y < value
}

// ------------------------------------
int main()
{
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}