#include "MapperMMC1.h"
#include "Cartridge.h"

MapperMMC1::MapperMMC1(uint8_t prgBanks, uint8_t chrBanks)
    : Mapper(prgBanks, chrBanks),
      _shiftRegister(0x10),  // bit 4 set = empty/reset state sentinel
      _writeCount(0),
      _controlReg(0x0C),     // power-on: PRG mode 3 (fixed last bank), CHR mode 0
      _chrBank0(0),
      _chrBank1(0),
      _prgBank(0),
      _prgMode(3),           // decoded from _controlReg bits 2-3
      _chrMode(0),           // decoded from _controlReg bit 4
      _mirrorMode(MirrorMode::MIRROR_MODE_HORIZONTAL)
{
    // Allocate CHR-RAM if no CHR-ROM banks
    if (_chrBanks == 0)
        _chrRAM.resize(0x2000, 0x00);
}

void MapperMMC1::Reset()
{
    _shiftRegister  = 0x10;
    _writeCount     = 0;
    _controlReg     = 0x0C;
    _chrBank0       = 0;
    _chrBank1       = 0;
    _prgBank        = 0;
    _prgMode        = 3;
    _chrMode        = 0;
    _mirrorMode     = MirrorMode::MIRROR_MODE_HORIZONTAL;
}

void MapperMMC1::ApplyRegister(uint16_t address, uint8_t data)
{
    // bits 13-14 of address select which internal register
    uint8_t target = (address >> 13) & 0x03;

    switch (target)
    {
        case 0: // $8000-$9FFF: Control
            _controlReg = data & 0x1F;
            _prgMode    = (_controlReg >> 2) & 0x03;
            _chrMode    = (_controlReg >> 4) & 0x01;

            switch (_controlReg & 0x03)
            {
                case 0: _mirrorMode = MirrorMode::MIRROR_MODE_ONE_SCREEN_LOWER; break;
                case 1: _mirrorMode = MirrorMode::MIRROR_MODE_ONE_SCREEN_UPPER; break;
                case 2: _mirrorMode = MirrorMode::MIRROR_MODE_VERTICAL;         break;
                case 3: _mirrorMode = MirrorMode::MIRROR_MODE_HORIZONTAL;       break;
            }
            break;

        case 1: // $A000-$BFFF: CHR bank 0
            _chrBank0 = data & 0x1F;
            break;

        case 2: // $C000-$DFFF: CHR bank 1
            _chrBank1 = data & 0x1F;
            break;

        case 3: // $E000-$FFFF: PRG bank
            _prgBank = data & 0x0F;  // bit 4 = PRG-RAM enable (ignore for now)
            break;
    }
}

bool MapperMMC1::CPUMapWrite(uint16_t address, uint32_t &mappedAddress, uint8_t data)
{
    if (address < 0x8000)
        return false;

    // Bit 7 set: reset shift register immediately
    if (data & 0x80)
    {
        _shiftRegister = 0x10;
        _writeCount    = 0;
        _controlReg   |= 0x0C;  // force PRG mode 3 on reset
        _prgMode       = 3;
        return false;
    }

    // Feed bit 0 of data into shift register (LSB first)
    _shiftRegister = ((_shiftRegister >> 1) | ((data & 0x01) << 4));
    _writeCount++;

    // Fifth write: latch into the appropriate register
    if (_writeCount == 5)
    {
        ApplyRegister(address, _shiftRegister & 0x1F);
        _shiftRegister = 0x10;  // reset for next sequence
        _writeCount    = 0;
    }

    (void)mappedAddress;
    return false;  // never an actual ROM write
}

bool MapperMMC1::CPUMapRead(uint16_t address, uint32_t &mappedAddress)
{
    if (address < 0x8000)
        return false;

    switch (_prgMode)
    {
        case 0:
        case 1:
            // 32 KB mode: switch two banks at once (ignore low bit of _prgBank)
            mappedAddress = (uint32_t)(_prgBank & 0xFE) * 0x4000 + (address & 0x7FFF);
            return true;

        case 2:
            // Fix first bank at 0, switch upper bank
            if (address <= 0xBFFF)
                mappedAddress = (address & 0x3FFF);  // bank 0 fixed
            else
                mappedAddress = (uint32_t)_prgBank * 0x4000 + (address & 0x3FFF);
            return true;

        case 3:
            // Most common: fix last bank at $C000, switch lower bank
            if (address <= 0xBFFF)
                mappedAddress = (uint32_t)_prgBank * 0x4000 + (address & 0x3FFF);
            else
                mappedAddress = (uint32_t)(_prgBanks - 1) * 0x4000 + (address & 0x3FFF);
            return true;
    }
    return false;
}

bool MapperMMC1::PPUMapRead(uint16_t address, uint32_t &mappedAddress)
{
    if (address >= 0x2000)
        return false;

    // CHR-RAM path (no CHR-ROM banks)
    if (_chrBanks == 0)
    {
        mappedAddress = address;
        return true;
    }

    // CHR mode 0: single 8 KB bank, _chrBank0 selects it (ignores bit 0)
    if (_chrMode == 0)
    {
        mappedAddress = (uint32_t)(_chrBank0 & 0x1E) * 0x1000 + address;
        return true;
    }

    // CHR mode 1: two independent 4 KB banks
    if (address < 0x1000)
        mappedAddress = (uint32_t)_chrBank0 * 0x1000 + (address & 0x0FFF);
    else
        mappedAddress = (uint32_t)_chrBank1 * 0x1000 + (address & 0x0FFF);
    return true;
}

bool MapperMMC1::PPUMapWrite(uint16_t address, uint32_t &mappedAddress)
{
    if (address >= 0x2000)
        return false;

    // Only writable if CHR-RAM
    if (_chrBanks == 0)
    {
        mappedAddress = address;
        return true;
    }
    return false;
}