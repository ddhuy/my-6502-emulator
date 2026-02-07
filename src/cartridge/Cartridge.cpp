#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Cartridge.h"
#include "Mapper.h"
#include "MapperNROM.h"


Cartridge::Cartridge()
    : _prgRomBanks(0),
      _chrRomBanks(0),
      _mapperNumber(0),
      _mirrorMode(MirrorMode::MIRROR_MODE_HORIZONTAL),
      _hasBattery(false),
      _hasTrainer(false),
      _loaded(false)
{
}

Cartridge::~Cartridge()
{
}

bool Cartridge::LoadFromFile(const std::string &filename)
{
    _filename = filename;
    _loaded = false;

    // Open file
    std::ifstream f(_filename, std::ios::binary);
    if (!f.is_open())
    {
        std::cerr << "Failed to open ROM file: " << _filename << std::endl;
        return false;
    }

    // Read entire file into memory
    std::vector<uint8_t> romData((std::istreambuf_iterator<char>(f)),
                                 std::istreambuf_iterator<char>());
    f.close();

    if (romData.size() < sizeof(INESHeader))
    {
        std::cerr << "ROM file is too small!" << std::endl;
        return false;
    }

    // Parse header
    if (!ParseHeader(romData))
    {
        return false;
    }

    // Calculate data positions
    size_t offset = sizeof(INESHeader);

    // Skip trainer if present
    if (_hasTrainer)
        offset += 512;

    // Read PRG-ROM
    size_t prgSize = _prgRomBanks * 16 * 1024; // 16KB per bank
    if (offset + prgSize > romData.size())
    {
        std::cerr << "Invalid PRG-ROM size" << std::endl;
        return false;
    }

    _prgROM.resize(prgSize);
    std::memcpy(_prgROM.data(), romData.data() + offset, prgSize);
    offset += prgSize;

    // Read CHR-ROM (or allocate CHR-RAM)
    if (_chrRomBanks > 0)
    {
        size_t chrSize = _chrRomBanks * 8192; // 8KB per bank
        if (offset + chrSize > romData.size())
        {
            std::cerr << "Invalid CHR-ROM size" << std::endl;
            return false;
        }
        _chrROM.resize(chrSize);
        std::memcpy(_chrROM.data(), romData.data() + offset, chrSize);
    }
    else
    {
        // No CHR-ROM, so we have CHR-RAM
        _chrROM.resize(8192); // 8KB CHR-RAM
        std::fill(_chrROM.begin(), _chrROM.end(), 0);
    }

    // Allocate PRG-RAM (8KB)
    _prgRAM.resize(8 * 1024);
    std::fill(_prgRAM.begin(), _prgRAM.end(), 0);

    // Create Mapper
    switch (_mapperNumber)
    {
        case 0:
            _mapper = std::make_unique<MapperNROM>(_prgRomBanks, _chrRomBanks);
            break;
        // TODO: Add more mappers here        
        default:
            std::cerr << "Unsupported mapper: " << (int) _mapperNumber << std::endl;
            return false;
    }

    _loaded = true;
    std::cout << "ROM loaded successfully: " << _filename << std::endl;
    std::cout << GetRomInfo() << std::endl;
    
    return true;
}

bool Cartridge::ParseHeader(const std::vector<uint8_t> &romData)
{
    const INESHeader *header = reinterpret_cast<const INESHeader *>(romData.data());

    // Check magic number "NES\x1A"
    if (header->name[0] != 'N' || header->name[1] != 'E'
        || header->name[2] != 'S' || header->name[3] != 0x1A)
    {
        std::cerr << "Invalid iNES header" << std::endl;
        return false;
    }

    // Parse basic info
    _prgRomBanks = header->prgRomSize;
    _chrRomBanks = header->chrRomSize;

    // Parse Flags 6
    _mirrorMode = (header->flags6 & 0x01) ? MirrorMode::MIRROR_MODE_VERTICAL : MirrorMode::MIRROR_MODE_HORIZONTAL;
    _hasBattery = (header->flags6 & 0x02) ? true: false;
    _hasTrainer = (header->flags6 & 0x04) ? true: false;
    if (header->flags6 & 0x08)
        _mirrorMode = MirrorMode::MIRROR_MODE_FOUR_SCREEN;

    // Parse mapper number
    _mapperNumber = ((header->flags7 & 0xF0) | (header->flags6 >> 4));

    // Check for NES 2.0 format
    if ((header->flags7 & 0x0C) == 0x08)
    {
        std::cout << "NES 2.0 format detected!" << std::endl;
        // For now, treat it as iNES;
    }

    return true;
}

uint8_t Cartridge::CPURead(uint16_t address)
{
    uint32_t mappedAddress = 0;

    // Check PRG-RAM range ($6000-$7FFF)
    if (0x6000 <= address && address < 0x8000)
        return _prgRAM[address - 0x6000];

    // Check PRG-ROM range via mapper
    if (_mapper->CPUMapRead(address, mappedAddress))
    {
        if (mappedAddress < _prgROM.size())
            return _prgROM[mappedAddress];
    }

    return 0x00;
}

void Cartridge::CPUWrite(uint16_t address, uint8_t data)
{
    uint32_t mappedAddress = 0;

    // Check PRG-RAM range ($6000-$7FFF)
    if (0x6000 <= address && address < 0x8000)
    {
        _prgRAM[address - 0x6000] = data;
        return;
    }

    // Some mappers use writes to PRG-ROM area for bank switching
    if (_mapper->CPUMapWrite(address, mappedAddress, data))
    {
        // Most mappers dont actually write to ROM, but we support it for CHR-RAM
        if (mappedAddress < _prgROM.size())
            _prgROM[mappedAddress] = data;
    }
}

uint8_t Cartridge::PPURead(uint16_t address)
{
    uint32_t mappedAddress = 0;

    if (_mapper->PPUMapRead(address, mappedAddress) && (mappedAddress < _chrROM.size()))
        return _chrROM[mappedAddress];

    return 0x00;
}

void Cartridge::PPUWrite(uint16_t address, uint8_t data)
{
    uint32_t mappedAddress = 0;

    if (_mapper->PPUMapWrite(address, mappedAddress) && (mappedAddress < _chrROM.size()))
        _chrROM[mappedAddress] = data;
}

void Cartridge::Reset()
{
    _mapper->Reset();
}

std::string Cartridge::GetRomInfo() const
{
    std::ostringstream oss;

    oss << "ROM Information:" << std::endl;
    oss << "  File    : " << _filename << std::endl;
    oss << "  Mapper  : " << (int) _mapperNumber << std::endl;
    oss << "  PRG-ROM : " << (int) _prgRomBanks  << " x 16KB" << std::endl;
    oss << "  CHR-ROM : " << (int) _chrRomBanks  << " x 8KB";
    if (_chrRomBanks == 0)
        oss << " (CHR-RAM)";
    oss << std::endl;

    oss << "  Mirroring : ";    
    switch (_mirrorMode)
    {
        case MirrorMode::MIRROR_MODE_HORIZONTAL : oss << "Horizontal"; break;
        case MirrorMode::MIRROR_MODE_VERTICAL   : oss << "Vertical";   break;
        case MirrorMode::MIRROR_MODE_FOUR_SCREEN: oss << "Four-screen"; break;
        default: oss << "Unknown"; break;
    }
    oss << std::endl;
    
    oss << "  Battery: " << (_hasBattery ? "Yes" : "No") << std::endl;
    oss << "  Trainer: " << (_hasTrainer ? "Yes" : "No") << std::endl;

    return oss.str();
}