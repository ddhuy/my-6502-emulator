#include <gtest/gtest.h>
#include "CPU.h"
#include "Bus.h"
#include "Memory.h"


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

