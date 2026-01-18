#include "gtest_fixture.hpp"


TEST_F(CPU6502Test, ADDRESSING_MODE_ABS)
{
    cpu.PC = 0x8000;
    ram.write(0x8000, 0x34);
    ram.write(0x8001, 0x12);
    ram.write(0x1234, 0x77);

    cpu.ABS();
    uint8_t value = cpu.fetch();

    EXPECT_EQ(value, 0x77);
    EXPECT_EQ(cpu.PC, 0x8002);
}