#ifndef PPU_H
#define PPU_H

#include <cstdint>

// Forward declarationc
class Bus;

class PPU
{
public:
    PPU();
    ~PPU();

    void ConnectBus(Bus* b);

    void Reset();
    void Clock();

    bool NMIOccurred();
    void ClearNMI();

    uint8_t Read(uint16_t address);
    void Write(uint16_t address, uint8_t value);

private:
    Bus* _bus;
};

#endif // PPU_H