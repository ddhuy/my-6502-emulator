#include "MapperCNROM.h"
#include "utils/Logger.h"


#include "MapperCNROM.h"
#include "utils/Logger.h"


MapperCNROM::MapperCNROM(uint8_t prgBanks, uint8_t chrBanks)
    : Mapper(prgBanks, chrBanks)
{
}

bool MapperCNROM::CPUMapRead(uint16_t address, uint32_t &mappedAddress)
{
    LOG_DEBUG("address=0x%04x  mappedAddress=0x%04x", address, mappedAddress);

    // Fixed PRG-ROM bank exactly like NROM
    if (address >= 0x8000)
    {
        mappedAddress = address & (_prgBanks > 1 ? 0x7FFF : 0x3FFF);
        LOG_INFO("CNROM: prg_bank -> %d. chr_bank -> %d", _prgBanks, _chrBanks);
        return true;
    }

    return false;
}

bool MapperCNROM::CPUMapWrite(uint16_t address, uint32_t &mappedAddress, uint8_t data)
{
    LOG_DEBUG("address=0x%04x  mappedAddress=0x%04x  data=0x%02x", address, mappedAddress, data);

    // Writing $8000-$FFFF selects an 8KB CHR bank
    if (address >= 0x8000)
        _chrBanks = data & 0x03;

    (void)mappedAddress; // Suppress unused variable warning

    return false;
}

bool MapperCNROM::PPUMapRead(uint16_t address, uint32_t &mappedAddress)
{
    LOG_DEBUG("address=0x%04x  mappedAddress=0x%02x", address, mappedAddress);

    if (address < 0x2000)
    {
        mappedAddress = _chrBanks * 0x2000 + address; // Each CHR bank is 8KB
        return true;
    }

    return false;
}

bool MapperCNROM::PPUMapWrite(uint16_t address, uint32_t &mappedAddress)
{
    LOG_DEBUG("address=0x%04x  mappedAddress=0x%02x", address, mappedAddress);
    (void) mappedAddress; // Suppress unused variable warning
    return false; // CHR-ROM is not writable
}

void MapperCNROM::Reset()
{
    _chrBanks = 0; // Reset to bank 0 on reset
}
