#include <gtest/gtest.h>
#include "bus/Bus.h"


class CPUTest : public ::testing::Test
{
public:
    struct AddressingResult
    {
        uint16_t address;
        bool pageCrossed;
    };


protected:
    Memory memory;
    Bus bus;
    CPU cpu;
    Cartridge cartridge;

    void SetUp() override;

    AddressingResult GetAddress(CPU::AddressingMode mode);

    void LoadAndExecute(const std::vector<uint8_t>& program);
};

