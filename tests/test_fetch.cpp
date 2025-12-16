#include <cassert>

#include "cpu/CPU6502.hpp"
#include "bus/Bus.hpp"
#include "mem/RAM.hpp"


int main()
{
    // Create bus and RAM
    Bus bus;
    RAM ram;

    // Attach RAM to bus
    bus.attachMemory(&ram);

    // Create CPU and connect it to the bus
    CPU6502 cpu;
    cpu.connectBus(&bus);

    // Initialize RAM with test data
    ram.write(0x1000, 0x42); // Arbitrary test value
    cpu.PC = 0x1000;         // Set Program Counter to test address

    // Perform a fetch operation
    uint8_t value = cpu.fetchByte();
    assert(value == 0x42);
    assert(cpu.PC == 0x1001);

    return 0;
}