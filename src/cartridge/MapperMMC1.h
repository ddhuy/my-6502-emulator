#ifndef MAPPER_MMC1_H
#define MAPPER_MMC1_H

#include "Cartridge.h"
#include "Mapper.h"
#include "MirrorMode.h"


class MapperMMC1 : public Mapper
{
public:
    MapperMMC1(uint8_t prgBanks, uint8_t chrBanks);
    virtual ~MapperMMC1() override = default;

    virtual bool CPUMapRead(uint16_t address, uint32_t &mappedAddress) override;
    virtual bool CPUMapWrite(uint16_t address, uint32_t &mappedAddress, uint8_t data) override;
    virtual bool PPUMapRead(uint16_t address, uint32_t &mappedAddress) override;
    virtual bool PPUMapWrite(uint16_t address, uint32_t &mappedAddress) override;
    virtual void Reset() override;

    // MMC1 controls mirroring dynamically
    MirrorMode GetMirrorMode() const { return _mirrorMode; }

private:
    void WriteControl(uint8_t data);
    void ApplyRegister(uint16_t address, uint8_t data);

    // Shift register state
    uint8_t _shiftRegister; // 5-bit serial shift register
    uint8_t _writeCount;    // how many bits written so far (0-4)

    // Internal register (5 bits each)
    uint8_t _controlReg;    // $8000-$9FFF: mirroring, PRG mode, CHR mode
    uint8_t _chrBank0;      // $A000-$BFFF: CHR bank 0
    uint8_t _chrBank1;      // $A000-$DFFF: CHR bank 1
    uint8_t _prgBank;       // $E000-$FFFF: PRG bank select

    // Decode from control register
    uint8_t _prgMode;       // 0-3: PRG banking mode
    uint8_t _chrMode;       // 0-1: CHR banking mode (0=8KB, 1=4KB)
    MirrorMode _mirrorMode; // current dynamic mirroring

    // CHR-RAM (8KB) for games with no CHR-ROM
    std::vector<uint8_t> _chrRAM;
};


#endif // MAPPER_MMC1_H