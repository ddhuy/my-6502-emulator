#include <gtest/gtest.h>
#include "bus/Bus.h"
#include "cpu/CPU.h"
#include "memory/Memory.h"


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

    void SetUp() override;

    AddressingResult GetAddress(CPU::AddressingMode mode);

    void LoadAndExecute(const std::vector<uint8_t>& program);
};

