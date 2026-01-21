#include <gtest/gtest.h>
#include "cpu/CPU.h"
#include "bus/Bus.h"
#include "memory/Memory.h"


class AddressingModeTest : public ::testing::Test
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
    }

    struct AddressingResult
    {
        uint16_t address;
        bool pageCrossed;
    };

    AddressingResult GetAddress(CPU::AddressingMode mode)
    {
        AddressingResult result = {0, false};
        switch (mode)
        {
            case CPU::M_IMM:
                result.pageCrossed = (cpu.IMM() == 1);
                result.address = cpu.GetAddressAbsolute();
                break;
            case CPU::M_ZP0:
                result.pageCrossed = (cpu.ZP0() == 1);
                result.address = cpu.GetAddressAbsolute();
                break;
            case CPU::M_ZPX:
                result.pageCrossed = (cpu.ZPX() == 1);
                result.address = cpu.GetAddressAbsolute();
                break;
            case CPU::M_ZPY:
                result.pageCrossed = (cpu.ZPY() == 1);
                result.address = cpu.GetAddressAbsolute();
                break;
            case CPU::M_ABS:
                result.pageCrossed = (cpu.ABS() == 1);
                result.address = cpu.GetAddressAbsolute();
                break;
            case CPU::M_ABX:
                result.pageCrossed = (cpu.ABX() == 1);
                result.address = cpu.GetAddressAbsolute();
                break;
            case CPU::M_ABY:
                result.pageCrossed = (cpu.ABY() == 1);
                result.address = cpu.GetAddressAbsolute();
                break;
            case CPU::M_IMP:
                result.pageCrossed = (cpu.IMP() == 1);
                result.address = cpu.GetAddressAbsolute();
                break;
            case CPU::M_ACC:
                result.pageCrossed = (cpu.ACC() == 1);
                result.address = cpu.GetAddressAbsolute();
                break;
            case CPU::M_REL:
                result.pageCrossed = (cpu.REL() == 1);
                result.address = cpu.GetAddressRelative();
                break;
            case CPU::M_IND:
                result.pageCrossed = (cpu.IND() == 1);
                result.address = cpu.GetAddressAbsolute();
                break;
            case CPU::M_IZX:
                result.pageCrossed = (cpu.IZX() == 1);
                result.address = cpu.GetAddressAbsolute();
                break;
            case CPU::M_IZY:
                result.pageCrossed = (cpu.IZY() == 1);
                result.address = cpu.GetAddressAbsolute();
                break;
            default:
                assert(false && "Unknown addressing mode");
                break;
        }
        return result;
    }
};


// Test Immediate Addressing Mode
TEST_F(AddressingModeTest, IMM)
{
    // Load a program that uses immediate addressing
    uint8_t program[] = { 0x42 };
    cpu.LoadProgram(program, sizeof(program), 0x8000);

    AddressingResult addrResult = GetAddress(CPU::AddressingMode::M_IMM);
 
    EXPECT_EQ(cpu.PC, 0x8001);
    EXPECT_EQ(addrResult.address, 0x8000);
    EXPECT_FALSE(addrResult.pageCrossed);
}

// Test Zero Page Addressing Mode
TEST_F(AddressingModeTest, ZP0)
{
    uint8_t program[] = { 0x42 };
    cpu.LoadProgram(program, sizeof(program), 0x8000);

    AddressingResult addrResult = GetAddress(CPU::AddressingMode::M_ZP0);

    EXPECT_EQ(cpu.PC, 0x8001);
    EXPECT_EQ(addrResult.address, 0x42);
    EXPECT_FALSE(addrResult.pageCrossed);
}

// Test Zero Page,X Addressing Mode
TEST_F(AddressingModeTest, ZPX)
{
    uint8_t program[] = { 0x42 };
    cpu.LoadProgram(program, sizeof(program), 0x8000);
    cpu.X = 0x05; // Set X register for indexed addressing

    AddressingResult addrResult = GetAddress(CPU::AddressingMode::M_ZPX);

    EXPECT_EQ(cpu.PC, 0x8001);
    EXPECT_EQ(addrResult.address, 0x47);
    EXPECT_FALSE(addrResult.pageCrossed);
}

TEST_F(AddressingModeTest, ZPX_WrapAround)
{
    uint8_t program[] = { 0xFF };
    cpu.LoadProgram(program, sizeof(program), 0x8000);
    cpu.X = 0x02; // Set X register for indexed addressing

    AddressingResult addrResult = GetAddress(CPU::AddressingMode::M_ZPX);

    EXPECT_EQ(cpu.PC, 0x8001);
    EXPECT_EQ(addrResult.address, 0x01); // Wrap around to 0x01, not 0x101
    EXPECT_FALSE(addrResult.pageCrossed);
}

// Test Zero Page,Y Addressing Mode
TEST_F(AddressingModeTest, ZPY)
{
    uint8_t program[] = { 0x42 };
    cpu.LoadProgram(program, sizeof(program), 0x8000);
    cpu.Y = 0x05; // Set Y register for indexed addressing

    AddressingResult addrResult = GetAddress(CPU::AddressingMode::M_ZPY);

    EXPECT_EQ(cpu.PC, 0x8001);
    EXPECT_EQ(addrResult.address, 0x47);
    EXPECT_FALSE(addrResult.pageCrossed);
}

// Test Zero Page,Y Addressing Mode with Wrap Around
TEST_F(AddressingModeTest, ZPY_WrapAround)
{
    uint8_t program[] = { 0xFF };
    cpu.LoadProgram(program, sizeof(program), 0x8000);
    cpu.Y = 0x02; // Set Y register for indexed addressing

    AddressingResult addrResult = GetAddress(CPU::AddressingMode::M_ZPY);

    EXPECT_EQ(cpu.PC, 0x8001);
    EXPECT_EQ(addrResult.address, 0x01); // Wrap around to 0x01, not 0x101
    EXPECT_FALSE(addrResult.pageCrossed);
}

// Test Absolute Addressing Mode
TEST_F(AddressingModeTest, ABS)
{
    uint8_t program[] = { 0x34, 0x12 }; // Low byte, High byte
    cpu.LoadProgram(program, sizeof(program), 0x8000);

    AddressingResult addrResult = GetAddress(CPU::AddressingMode::M_ABS);

    EXPECT_EQ(cpu.PC, 0x8002);
    EXPECT_EQ(addrResult.address, 0x1234);
    EXPECT_FALSE(addrResult.pageCrossed);
}

// Test Absolute,X Addressing Mode without page crossing
TEST_F(AddressingModeTest, ABX_NoPageCross)
{
    uint8_t program[] = { 0xF0, 0x12 }; // Low byte, High byte
    cpu.LoadProgram(program, sizeof(program), 0x8000);
    cpu.X = 0x0F; // Set X register

    AddressingResult addrResult = GetAddress(CPU::AddressingMode::M_ABX);

    EXPECT_EQ(cpu.PC, 0x8002);
    EXPECT_EQ(addrResult.address, 0x12FF);
    EXPECT_FALSE(addrResult.pageCrossed);
}

// Test Absolute,X Addressing Mode with page crossing
TEST_F(AddressingModeTest, ABX_PageCross)
{
    uint8_t program[] = { 0xF0, 0x12 }; // Low byte, High byte
    cpu.LoadProgram(program, sizeof(program), 0x8000);
    cpu.X = 0x20; // Set X register

    AddressingResult addrResult = GetAddress(CPU::AddressingMode::M_ABX);

    EXPECT_EQ(cpu.PC, 0x8002);
    EXPECT_EQ(addrResult.address, 0x1310);
    EXPECT_TRUE(addrResult.pageCrossed);
}

// Test Absolute,Y Addressing Mode without page crossing
TEST_F(AddressingModeTest, ABY_NoPageCross)
{
    uint8_t program[] = { 0xF0, 0x12 }; // Low byte, High byte
    cpu.LoadProgram(program, sizeof(program), 0x8000);
    cpu.Y = 0x0F; // Set Y register

    AddressingResult addrResult = GetAddress(CPU::AddressingMode::M_ABY);

    EXPECT_EQ(cpu.PC, 0x8002);
    EXPECT_EQ(addrResult.address, 0x12FF);
    EXPECT_FALSE(addrResult.pageCrossed);
}

// Test Absolute,Y Addressing Mode with page crossing
TEST_F(AddressingModeTest, ABY_PageCross)
{
    uint8_t program[] = { 0xF0, 0x12 }; // Low byte, High byte
    cpu.LoadProgram(program, sizeof(program), 0x8000);
    cpu.Y = 0x20; // Set Y register

    AddressingResult addrResult = GetAddress(CPU::AddressingMode::M_ABY);

    EXPECT_EQ(cpu.PC, 0x8002);
    EXPECT_EQ(addrResult.address, 0x1310);
    EXPECT_TRUE(addrResult.pageCrossed);
}

// Test Relative addressing (for branches)
TEST_F(AddressingModeTest, REL_PositiveOffset)
{
    uint8_t program[] = {0x10};  // Offset +16
    cpu.LoadProgram(program, sizeof(program), 0x8000);
    
    AddressingResult result = GetAddress(CPU::AddressingMode::M_REL);
    
    EXPECT_EQ(result.address, 0x10);  // 0x10
    EXPECT_FALSE(result.pageCrossed); // REL doesn't report, Branch() handles it
}

TEST_F(AddressingModeTest, REL_NegativeOffset)
{
    uint8_t program[] = {0xF0};  // Offset -16 (two's complement)
    cpu.LoadProgram(program, sizeof(program), 0x8000);
    
    AddressingResult result = GetAddress(CPU::AddressingMode::M_REL);
    
    EXPECT_EQ(result.address, 0xFFF0);  //  -16
    EXPECT_FALSE(result.pageCrossed); // REL doesn't report, Branch() handles it
}

TEST_F(AddressingModeTest, REL_NoPageCross)
{
    uint8_t program[] = {0x7F};  // Offset +127
    cpu.LoadProgram(program, sizeof(program), 0x80FF);

    AddressingResult result = GetAddress(CPU::AddressingMode::M_REL);
    
    EXPECT_EQ(result.address, 0x7F);
    EXPECT_FALSE(result.pageCrossed); // REL doesn't report, Branch() handles it
}

TEST_F(AddressingModeTest, REL_PageCross)
{
    uint8_t program[] = {0x50}; // Offset +80
    cpu.LoadProgram(program, sizeof(program), 0x80DF);

    AddressingResult result = GetAddress(CPU::AddressingMode::M_REL);
    
    EXPECT_EQ(result.address, 0x50);
    EXPECT_FALSE(result.pageCrossed); // REL doesn't report, Branch() handles it
}

// Test Indirect addressing (JMP only)
TEST_F(AddressingModeTest, IND) {
    uint8_t program[] = {0x20, 0x30};  // Pointer at 0x3020
    cpu.LoadProgram(program, sizeof(program), 0x8000);
    
    // Set up target address at 0x3020
    cpu.WriteMemory(0x3020, 0x34);
    cpu.WriteMemory(0x3021, 0x12);
    
    AddressingResult result = GetAddress(CPU::AddressingMode::M_IND);
    
    EXPECT_EQ(result.address, 0x1234);
}

// Test Indirect addressing with page boundary bug
TEST_F(AddressingModeTest, IND_PageBoundaryBug)
{
    uint8_t program[] = {0xFF, 0x30};  // Pointer at 0x30FF
    cpu.LoadProgram(program, sizeof(program), 0x8000);
    
    // Set up values
    cpu.WriteMemory(0x30FF, 0x34);
    cpu.WriteMemory(0x3000, 0x12);  // Wraps to start of page, not 0x3100!
    cpu.WriteMemory(0x3100, 0x99);  // This should NOT be read
    
    AddressingResult result = GetAddress(CPU::AddressingMode::M_IND);
    
    EXPECT_EQ(result.address, 0x1234);  // Not 0x9934!
}

// Test Indexed Indirect (X) - (d,X)
TEST_F(AddressingModeTest, IZX)
{
    uint8_t program[] = {0x40};  // Zero page base
    cpu.LoadProgram(program, sizeof(program), 0x8000);
    cpu.X = 0x05;
    
    // Set up pointer at 0x45 (0x40 + 0x05)
    cpu.WriteMemory(0x45, 0x34);
    cpu.WriteMemory(0x46, 0x12);
    
    AddressingResult result = GetAddress(CPU::AddressingMode::M_IZX);
    
    EXPECT_EQ(result.address, 0x1234);
    EXPECT_FALSE(result.pageCrossed);
}

TEST_F(AddressingModeTest, IZX_WrapAround)
{
    uint8_t program[] = {0xFF};
    cpu.LoadProgram(program, sizeof(program), 0x8000);
    cpu.X = 0x05;
    
    // Wraps to 0x04
    cpu.WriteMemory(0x04, 0x34);
    cpu.WriteMemory(0x05, 0x12);
    
    AddressingResult result = GetAddress(CPU::AddressingMode::M_IZX);
    
    EXPECT_EQ(result.address, 0x1234);
}

// Test Indirect Indexed (Y) - (d),Y
TEST_F(AddressingModeTest, IZY_NoPageCross)
{
    uint8_t program[] = {0x40};
    cpu.LoadProgram(program, sizeof(program), 0x8000);
    cpu.Y = 0x05;
    
    // Set up pointer at 0x40
    cpu.WriteMemory(0x40, 0x00);
    cpu.WriteMemory(0x41, 0x12);  // Base address 0x1200
    
    AddressingResult result = GetAddress(CPU::AddressingMode::M_IZY);
    
    EXPECT_EQ(result.address, 0x1205);
    EXPECT_FALSE(result.pageCrossed);
}

TEST_F(AddressingModeTest, IZY_PageCross)
{
    uint8_t program[] = {0x40};
    cpu.LoadProgram(program, sizeof(program), 0x8000);
    cpu.Y = 0x10;
    
    // Set up pointer at 0x40
    cpu.WriteMemory(0x40, 0xF8);
    cpu.WriteMemory(0x41, 0x12);  // Base address 0x12F8
    
    AddressingResult result = GetAddress(CPU::AddressingMode::M_IZY);
    
    EXPECT_EQ(result.address, 0x1308);  // 0x12F8 + 0x10
    EXPECT_TRUE(result.pageCrossed);
}

// ------------------------------------
int main()
{
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
