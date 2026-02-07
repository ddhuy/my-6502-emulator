#ifndef MAPPER_H
#define MAPPER_H

#include <cstdint>
#include <vector>

// Base class for all mappers
class Mapper
{
public:
    Mapper(uint8_t prgBanks, uint8_t chrBanks);
    virtual ~Mapper() = default;

    // CPU Memory mapping (PRG)
    virtual bool CPUMapRead(uint16_t address, uint32_t &mappedAddress) = 0;
    virtual bool CPUMapWrite(uint16_t address, uint32_t &mappedAddress, uint8_t data) = 0;

    // PPU Memory mapping (CHR)
    virtual bool PPUMapRead(uint16_t address, uint32_t &mappedAddress) = 0;
    virtual bool PPUMapWrite(uint16_t address, uint32_t &mappedAddress) = 0;

    // Reset mapper state
    virtual void Reset() = 0;

    // IRQ Support (for mappers that have it)
    virtual bool IRQState() { return false; }
    virtual void IRQClear() {}
    virtual void Scanline() {} // Called once per scanline

protected:
    uint8_t _prgBanks; // Number of PRG-ROM banks
    uint8_t _chrBanks; // Number of CHR-ROM banks

};

#endif // MAPPER_H