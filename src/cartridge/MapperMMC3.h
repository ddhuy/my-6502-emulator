#ifndef MAPPER_MMC3_H
#define MAPPER_MMC3_H

#include <array>

#include "Mapper.h"


class MapperMMC3 : public Mapper
{
public:
    MapperMMC3(uint8_t prgBanks, uint8_t chrBanks, MirrorMode mirror);
    virtual ~MapperMMC3() override = default;

    virtual bool CPUMapRead(uint16_t address, uint32_t &mappedAddress) override;
    virtual bool CPUMapWrite(uint16_t address, uint32_t &mappedAddress, uint8_t data) override;

    virtual bool PPUMapRead(uint16_t address, uint32_t &mappedAddress) override;
    virtual bool PPUMapWrite(uint16_t address, uint32_t &mappedAddress) override;

    virtual void Reset() override;

    // Scanline counter and IRQ support
    virtual bool IRQState() override { return _irqActive; }
    virtual void IRQClear() override { _irqActive = false; }
    virtual void Scanline() override;

private:
    uint8_t _bankSelect; // Bank select register
    std::array<uint8_t, 8> _bankRegister; // R0-R5 = CHR banks, R6-R7 = PRG banks
    
    bool _prgMode; // PRG ROM bank mode (0 or 1)
    bool _chrInversion; // CHR ROM bank mode (0 or 1)

    // IRQ scanline counter
    uint8_t _irqLatch;   // IRQ latch value
    uint8_t _irqCounter; // IRQ reload value
    bool _irqEnable; // IRQ enable flag
    bool _irqReload; // IRQ reload value
    bool _irqActive; // IRQ active flag

    // Four-screen boards (e.g. Gauntlet) ignore the mirroring register
    bool _fourScreen; // Four-screen mirroring flag
};


#endif // MAPPER_MMC3_H