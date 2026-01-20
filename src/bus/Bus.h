#ifndef BUS_H
#define BUS_H

#include <cstdint>
#include <memory>

// Forward declaration
class Memory;

class Bus
{
public:
    Bus();
    virtual ~Bus();

    void AttachMemory(Memory* memory);

    uint8_t Read(uint16_t address) const;
    void Write(uint16_t address, uint8_t value);

private:
    Memory* _memory;
};


#endif // BUS_H