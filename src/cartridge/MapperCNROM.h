#ifndef MAPPER_CNROM_H
#define MAPPER_CNROM_H

#include "Mapper.h"


class MapperCNROM : public Mapper
{
public:
    MapperCNROM(uint8_t prgBanks, uint8_t chrBanks, MirrorMode mirror);
    virtual ~MapperCNROM() override = default;

    virtual bool CPUMapRead(uint16_t address, uint32_t &mappedAddress) override;
    virtual bool CPUMapWrite(uint16_t address, uint32_t &mappedAddress, uint8_t data) override;

    virtual bool PPUMapRead(uint16_t address, uint32_t &mappedAddress) override;
    virtual bool PPUMapWrite(uint16_t address, uint32_t &mappedAddress) override;

    virtual void Reset() override;
};

#endif // MAPPER_CNROM_H