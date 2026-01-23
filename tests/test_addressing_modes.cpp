#include "TestFixture.h"


// Test Immediate Addressing Mode
TEST_F(CPUTest, IMM)
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
TEST_F(CPUTest, ZP0)
{
    uint8_t program[] = { 0x42 };
    cpu.LoadProgram(program, sizeof(program), 0x8000);

    AddressingResult addrResult = GetAddress(CPU::AddressingMode::M_ZP0);

    EXPECT_EQ(cpu.PC, 0x8001);
    EXPECT_EQ(addrResult.address, 0x42);
    EXPECT_FALSE(addrResult.pageCrossed);
}

// Test Zero Page,X Addressing Mode
TEST_F(CPUTest, ZPX)
{
    uint8_t program[] = { 0x42 };
    cpu.LoadProgram(program, sizeof(program), 0x8000);
    cpu.X = 0x05; // Set X register for indexed addressing

    AddressingResult addrResult = GetAddress(CPU::AddressingMode::M_ZPX);

    EXPECT_EQ(cpu.PC, 0x8001);
    EXPECT_EQ(addrResult.address, 0x47);
    EXPECT_FALSE(addrResult.pageCrossed);
}

TEST_F(CPUTest, ZPX_WrapAround)
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
TEST_F(CPUTest, ZPY)
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
TEST_F(CPUTest, ZPY_WrapAround)
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
TEST_F(CPUTest, ABS)
{
    uint8_t program[] = { 0x34, 0x12 }; // Low byte, High byte
    cpu.LoadProgram(program, sizeof(program), 0x8000);

    AddressingResult addrResult = GetAddress(CPU::AddressingMode::M_ABS);

    EXPECT_EQ(cpu.PC, 0x8002);
    EXPECT_EQ(addrResult.address, 0x1234);
    EXPECT_FALSE(addrResult.pageCrossed);
}

// Test Absolute,X Addressing Mode without page crossing
TEST_F(CPUTest, ABX_NoPageCross)
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
TEST_F(CPUTest, ABX_PageCross)
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
TEST_F(CPUTest, ABY_NoPageCross)
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
TEST_F(CPUTest, ABY_PageCross)
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
TEST_F(CPUTest, REL_PositiveOffset)
{
    uint8_t program[] = {0x10};  // Offset +16
    cpu.LoadProgram(program, sizeof(program), 0x8000);
    
    AddressingResult result = GetAddress(CPU::AddressingMode::M_REL);
    
    EXPECT_EQ(result.address, 0x10);  // 0x10
    EXPECT_FALSE(result.pageCrossed); // REL doesn't report, Branch() handles it
}

TEST_F(CPUTest, REL_NegativeOffset)
{
    uint8_t program[] = {0xF0};  // Offset -16 (two's complement)
    cpu.LoadProgram(program, sizeof(program), 0x8000);
    
    AddressingResult result = GetAddress(CPU::AddressingMode::M_REL);
    
    EXPECT_EQ(result.address, 0xFFF0);  //  -16
    EXPECT_FALSE(result.pageCrossed); // REL doesn't report, Branch() handles it
}

TEST_F(CPUTest, REL_NoPageCross)
{
    uint8_t program[] = {0x7F};  // Offset +127
    cpu.LoadProgram(program, sizeof(program), 0x80FF);

    AddressingResult result = GetAddress(CPU::AddressingMode::M_REL);
    
    EXPECT_EQ(result.address, 0x7F);
    EXPECT_FALSE(result.pageCrossed); // REL doesn't report, Branch() handles it
}

TEST_F(CPUTest, REL_PageCross)
{
    uint8_t program[] = {0x50}; // Offset +80
    cpu.LoadProgram(program, sizeof(program), 0x80DF);

    AddressingResult result = GetAddress(CPU::AddressingMode::M_REL);
    
    EXPECT_EQ(result.address, 0x50);
    EXPECT_FALSE(result.pageCrossed); // REL doesn't report, Branch() handles it
}

// Test Indirect addressing (JMP only)
TEST_F(CPUTest, IND) {
    uint8_t program[] = {0x20, 0x30};  // Pointer at 0x3020
    cpu.LoadProgram(program, sizeof(program), 0x8000);
    
    // Set up target address at 0x3020
    cpu.WriteMemory(0x3020, 0x34);
    cpu.WriteMemory(0x3021, 0x12);
    
    AddressingResult result = GetAddress(CPU::AddressingMode::M_IND);
    
    EXPECT_EQ(result.address, 0x1234);
}

// Test Indirect addressing with page boundary bug
TEST_F(CPUTest, IND_PageBoundaryBug)
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
TEST_F(CPUTest, IZX)
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

TEST_F(CPUTest, IZX_WrapAround)
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
TEST_F(CPUTest, IZY_NoPageCross)
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

TEST_F(CPUTest, IZY_PageCross)
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
