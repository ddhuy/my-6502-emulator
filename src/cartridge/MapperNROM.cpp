#include "MapperNROM.h"


MapperNROM::MapperNROM(uint8_t prgBanks, uint8_t chrBanks)
    : Mapper(prgBanks, chrBanks)
{
}

bool MapperNROM::CPUMapRead(uint16_t address, uint32_t &mappedAddress)
{
    // CPU Address range: $8000-$FFFF
    if (0x8000 <= address && address <= 0xFFFF)
    {
        // If only 1 PRG bank (16KB), mirror it
        // If 2 PRG banks (32KB), use both
        mappedAddress = address & (_prgBanks > 1 ? 0x7FFF : 0x3FFF);
        return true;
    }

    return false;
}

bool MapperNROM::CPUMapWrite(uint16_t address, uint32_t &mappedAddress, uint8_t data)
{
    // NROM has no PRG-RAM, writing does nothing
    (void) data;

    // Some NROM games might have RAM at $6000-$7FFF
    if (0x6000 <= address && address < 0x8000)
    {
        mappedAddress = address - 0x6000;
        return true; // Allow write to PRG-RAM
    }

    return false;
}

bool MapperNROM::PPUMapRead(uint16_t address, uint32_t &mappedAddress)
{
    // PPU address range: $0000-$1FFF (pattern table)
    if (0x0000 <= address && address < 0x2000)
    {
        mappedAddress = address;
        return true;
    }

    return false;
}

bool MapperNROM::PPUMapWrite(uint16_t address, uint32_t &mappedAddress)
{
    // If there is no CHR-ROM (chrBanks == 0), then we have CHR-RAM
    if (0x0000 <= address && address < 0x2000)
    {
        if (_chrBanks == 0)
        {
            // CHR-RAM is writable
            mappedAddress = address;
            return true;
        }
    }

    return false; // CHR-ROM is not writable
}

void MapperNROM::Reset()
{
    // NROM has no state to reset
}
