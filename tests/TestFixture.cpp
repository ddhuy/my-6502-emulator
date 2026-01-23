#include "TestFixture.h"


void CPUTest::SetUp()
{
    bus.AttachMemory(&memory);
    cpu.ConnectBus(&bus);
    cpu.Reset();

    // Drain reset cycles
    while (cpu.GetCycles() > 0)
        cpu.Clock();
}

CPUTest::AddressingResult CPUTest::GetAddress(CPU::AddressingMode mode)
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

void CPUTest::LoadAndExecute(const std::vector<uint8_t>& program)
{
    for (size_t i = 0; i < program.size(); i++)
    {
        cpu.WriteMemory(0x8000 + i, program[i]);
    }
    cpu.PC = 0x8000;
    cpu.Step();
}

//-------------------------------------
int main()
{
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
