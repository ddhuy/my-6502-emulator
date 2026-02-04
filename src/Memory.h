#ifndef MEMORY_H
#define MEMORY_H


#include <cstdint>


class Memory
{
public:
    Memory();
    virtual ~Memory();

    // Read a byte from memory
    uint8_t Read(uint16_t address) const;

    // Write a byte to memory
    void Write(uint16_t address, uint8_t value);

    // Clear all memory to zero
    void Clear();

private:
    // 64KB of memory
    uint8_t ram[0x10000];
};
#endif // MEMORY_H