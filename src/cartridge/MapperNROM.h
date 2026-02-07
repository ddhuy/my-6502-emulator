#ifndef MAPPER_NROM_H
#define MAPPER_NROM_H

#include "Mapper.h"


class MapperNROM : public Mapper
{
public:
    MapperNROM(uint8_t prgBanks, uint8_t chrBanks);
    virtual ~MapperNROM() override = default;

    virtual bool CPUMapRead(uint16_t address, uint32_t &mappedAddress) override;
    virtual bool CPUMapWrite(uint16_t address, uint32_t &mappedAddress, uint8_t data) override;

    virtual bool PPUMapRead(uint16_t address, uint32_t &mappedAddress) override;
    virtual bool PPUMapWrite(uint16_t address, uint32_t &mappedAddress) override;

    virtual void Reset() override;
};

#endif // MAPPER_NROM_H