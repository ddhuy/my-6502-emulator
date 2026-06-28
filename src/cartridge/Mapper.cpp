#include "Mapper.h"


Mapper::Mapper(uint8_t prgBanks, uint8_t chrBanks, MirrorMode mirror)
    : _prgBanks(prgBanks), _chrBanks(chrBanks), _mirrorMode(mirror)
{}
