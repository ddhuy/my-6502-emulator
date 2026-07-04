#include "MapperUxROM.h"
#include "utils/Logger.h"


MapperUxROM::MapperUxROM(uint8_t prgBanks, uint8_t chrBanks, MirrorMode mirror)
    : Mapper(prgBanks, chrBanks, mirror)
{
    LOG_INFO("MapperUxROM created: prgBanks=%d", (int)prgBanks);
}

bool MapperUxROM::CPUMapRead(uint16_t address, uint32_t &mappedAddress)
{
    LOG_DEBUG("address=0x%04x  mappedAddress=0x%04x", address, mappedAddress);

    // Switchable PRG-ROM bank at $8000-$BFFF
    if (0x8000 <= address && address <= 0xBFFF)
    {
        mappedAddress = (uint32_t)(_prgBankSelect * 0x4000) + (address & 0x3FFF);
        return true;
    }
    // Fixed PRG-ROM bank at $C000-$FFFF (last bank)
    else if (0xC000 <= address && address <= 0xFFFF)
    {
        mappedAddress = (uint32_t)(_prgBanks - 1) * 0x4000 + (address & 0x3FFF);
        LOG_DEBUG("UxROM: prg_bank_select -> %d", _prgBankSelect);
        return true;
    }

    return false;
}

bool MapperUxROM::CPUMapWrite(uint16_t address, uint32_t &mappedAddress, uint8_t data)
{
    LOG_DEBUG("address=0x%04x  mappedAddress=0x%04x  data=0x%02x", address, mappedAddress, data);

    // Writing to $8000-$FFFF selects the PRG-ROM bank
    // This is NOT a write to PRG-ROM, but rather a bank select operation
    // we return false to indicate that no actual write to PRG-ROM occurs
    if (address >= 0x8000)
        _prgBankSelect = data & 0x0F; // Only lower 4 bits are used for bank selection (0-15)
    
    LOG_DEBUG("UxROM: prg_bank_select -> %d", _prgBankSelect);

    (void) mappedAddress; // Suppress unused variable warning

    return false;
}

bool MapperUxROM::PPUMapRead(uint16_t address, uint32_t &mappedAddress)
{
    LOG_DEBUG("address=0x%04x  mappedAddress=0x%02x", address, mappedAddress);

    // PPU address range: $0000-$1FFF (pattern table)
    if (address < 0x2000)
    {
        mappedAddress = address;
        return true;
    }

    return false;
}

bool MapperUxROM::PPUMapWrite(uint16_t address, uint32_t &mappedAddress)
{
    LOG_DEBUG("address=0x%04x  mappedAddress=0x%02x", address, mappedAddress);

    // If there is no CHR-ROM (chrBanks == 0), then we have CHR-RAM
    if (address < 0x2000)
    {
        // CHR-RAM is writable
        mappedAddress = address;
        return true;
    }

    return false; // CHR-ROM is not writable
}

void MapperUxROM::Reset()
{
    _prgBankSelect = 0; // Reset to bank 0 on reset
}
