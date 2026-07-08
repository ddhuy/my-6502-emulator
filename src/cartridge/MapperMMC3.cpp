#include "MapperMMC3.h"
#include "utils/Logger.h"


MapperMMC3::MapperMMC3(uint8_t prgBanks, uint8_t chrBanks, MirrorMode mirror)
    : Mapper(prgBanks, chrBanks, mirror),
      _bankSelect(0),
      _prgMode(false),
      _chrInversion(false),
      _irqLatch(0),
      _irqCounter(0),
      _irqEnable(false),
      _irqReload(false),
      _irqActive(false),
      _fourScreen(mirror == MirrorMode::MIRROR_MODE_FOUR_SCREEN)
{
    _bankRegister.fill(0);
    LOG_INFO("MapperMMC3 initialized: with prgBanks=%d  chrBanks=%d", prgBanks, chrBanks);
}

void MapperMMC3::Reset()
{
    _bankSelect   = 0;
    _prgMode      = false;
    _chrInversion = false;
    _irqLatch     = 0;
    _irqCounter   = 0;
    _irqEnable    = false;
    _irqReload    = false;
    _irqActive    = false;

    _bankRegister.fill(0);
}

bool MapperMMC3::CPUMapRead(uint16_t address, uint32_t &mappedAddress)
{
    if (address < 0x8000)
        return false; // Not handled by this mapper

    // MMC3 switches PRG in 8KB units; header banks are 16KB
    uint32_t total8K = (uint32_t) _prgBanks * 2; // Total 8KB banks
    uint32_t bank = 0;

    switch ((address >> 13) & 0x03) // 0x8000-0xFFFF divided into 4 regions
    {
        case 0: // $8000-$9FFF
            bank = _prgMode ? (total8K - 2) : _bankRegister[6];
            break;
        case 1: // $A000-$BFFF
            bank = _bankRegister[7];
            break;
        case 2: // $C000-$DFFF
            bank = _prgMode ? _bankRegister[6] : (total8K - 2);
            break;
        case 3: // $E000-$FFFF
            bank = (total8K - 1); // Fixed to last bank
            break;
    }

    bank = bank % total8K; // Wrap around if bank exceeds total
    mappedAddress = (bank * 0x2000) + (address & 0x1FFF); // 8KB bank size
    return true;
}

bool MapperMMC3::CPUMapWrite(uint16_t address, uint32_t &mappedAddress, uint8_t data)
{
    (void) mappedAddress; // Unused parameter

    if (address < 0x8000)
        return false; // Not handled by this mapper

    bool even = (address & 0x0001) == 0;

    switch (address & 0xE000)
    {
        case 0x8000:
        {
            if (even) // Bank select
            {
                _bankSelect   = data & 0x07; // Only lower 3 bits used
                _prgMode      = (data & 0x40) != 0;
                _chrInversion = (data & 0x80) != 0;
            }
            else // Bank data
            {
                uint8_t target = _bankSelect & 0x07;
                if (target == 0 || target == 1) // CHR banks
                    data &= 0xFE; // Ensure even bank for 2KB CHR banks
                else if (target == 6 || target == 7) // PRG banks
                    data &= 0x3F; // Mask to valid range
                _bankRegister[target] = data;
            }
            break;
        }

        case 0xA000:
        {
            if (even) // Mirroring
            {
                if (!_fourScreen)
                {
                    _mirrorMode = (data & 0x01) ? MirrorMode::MIRROR_MODE_HORIZONTAL : MirrorMode::MIRROR_MODE_VERTICAL;
                }
            }
            else // PRG RAM protect
            {
                // Not implemented; typically used for battery-backed RAM
            }
            break;
        }

        case 0xC000:
        {
            if (even) // IRQ latch
            {
                _irqLatch = data;
            }
            else // IRQ reload: coutner reloads on next scanline clock
            {
                _irqCounter = 0;
                _irqReload  = true;
            }
            break;
        }

        case 0xE000:
        {
            if (even) // IRQ disable
            {
                _irqEnable = false;
                _irqActive = false;
            }
            else // IRQ enable
            {
                _irqEnable = true;
            }
            break;
        }
    }

    return  false;
}

bool MapperMMC3::PPUMapRead(uint16_t address, uint32_t &mappedAddress)
{
    if (address >= 0x2000)
        return false; // Not handled by this mapper
    
    // CHR-RAM path (no CHR-ROM banks): palin 8KB, no bank switching
    if (_chrBanks == 0)
    {
        mappedAddress = address; // Direct mapping for CHR-RAM
        return true;
    }

    // Bit 7 of bank select swaps the 2KB and 1KB CHR regions (A12 inversion)
    uint16_t a = _chrInversion ? (address ^ 0x1000) : address; // Invert A12 if needed

    uint32_t total1K = (uint32_t) _chrBanks * 8; // Total 1KB banks
    uint32_t bank = 0;

    if (a < 0x0800) // R0 for $0000-$07FF
        bank = _bankRegister[0] + ((a >> 10) & 0x01);
    else if (a < 0x1000) // R1 for $0800-$0FFF
        bank = _bankRegister[1] + ((a >> 10) & 0x01);
    else
        bank = _bankRegister[2 + ((a - 0x1000) >> 10)]; // R2-R5: 1KB at $1000-$1FFF

    bank %= total1K; // Wrap around if bank exceeds total
    mappedAddress = (bank * 0x0400) + (a & 0x03FF); // 1KB bank size

    return true;
}

bool MapperMMC3::PPUMapWrite(uint16_t address, uint32_t &mappedAddress)
{
    if (address >= 0x2000)
        return false; // Not handled by this mapper

    // CHR-RAM path (no CHR-ROM banks): palin 8KB, no bank switching
    if (_chrBanks == 0)
    {
        mappedAddress = address; // Direct mapping for CHR-RAM
        return true;
    }

    // CHR-ROM is read-only; writes are ignored
    return false;
}

void MapperMMC3::Scanline()
{
    if (_irqCounter == 0 || _irqReload)
    {
        _irqCounter = _irqLatch;
        _irqReload  = false;
    }
    else
    {
        --_irqCounter;
    }

    if (_irqCounter == 0 && _irqEnable)
    {
        _irqActive = true; // Signal IRQ to CPU
    }
}