#pragma once
#include <cstdint>
#include <string>
#include <map>

class Bus;
class CPU6502;


class Disassembler
{
public:
    static std::map<uint16_t, std::string>
    disassemble(Bus& bus, uint16_t start, uint16_t end);
};
