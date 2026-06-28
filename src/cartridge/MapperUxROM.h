#ifndef MAPPER_UXROM_H
#define MAPPER_UXROM_H

#include "Mapper.h"


class MapperUxROM : public Mapper
{
public:
    MapperUxROM(uint8_t prgBanks, uint8_t chrBanks, MirrorMode mirror);
    virtual ~MapperUxROM() override = default;

    virtual bool CPUMapRead(uint16_t address, uint32_t &mappedAddress) override;
    virtual bool CPUMapWrite(uint16_t address, uint32_t &mappedAddress, uint8_t data) override;

    virtual bool PPUMapRead(uint16_t address, uint32_t &mappedAddress) override;
    virtual bool PPUMapWrite(uint16_t address, uint32_t &mappedAddress) override;

    virtual void Reset() override;

private:
    uint8_t _prgBankSelect; // Currently selected PRG bank (0-15)
};

#endif // MAPPER_UXROM_H