#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>


// Forward declarations
class Mapper;


// Mirroring modes
enum MirrorMode
{
    MIRROR_MODE_HORIZONTAL,
    MIRROR_MODE_VERTICAL,
    MIRROR_MODE_FOUR_SCREEN,
    MIRROR_MODE_ONE_SCREEN_LOWER,
    MIRROR_MODE_ONE_SCREEN_UPPER
};


class Cartridge
{
public:
    Cartridge();
    ~Cartridge();

    // Load ROM from file
    bool LoadFromFile(const std::string &filename);

    // CPU memory access (PRG-ROM/RAM)
    uint8_t CPURead(uint16_t address);
    void CPUWrite(uint16_t address, uint8_t data);

    // PPU memory access (CHR-ROM/RAM)
    uint8_t PPURead(uint16_t address);
    void PPUWrite(uint16_t address, uint8_t data);

    // Get mirroring mode
    MirrorMode GetMirrorMode() const { return _mirrorMode; }

    // Check if cartridge is loaded
    bool IsLoaded() const { return _loaded; }

    // Get ROM info
    std::string GetRomInfo() const;
    uint8_t GetMapperNumber() const { return _mapperNumber; }

    // Reset Cartridge state
    void Reset();

private:
    // Parse iNES header
    bool ParseHeader(const std::vector<uint8_t> &romData);

    // ROM data
    std::vector<uint8_t> _prgROM; // PRG-ROM (program ROM)
    std::vector<uint8_t> _chrROM; // CHR-ROM (character/pattern ROM)
    std::vector<uint8_t> _prgRAM; // PRG-RAM (save RAM)

    // ROM info
    uint8_t _prgRomBanks;   // Number of 16KB PRG-ROM banks
    uint8_t _chrRomBanks;   // Number of 8KB CHR-ROM banks
    uint8_t _mapperNumber;  // Mapper number
    MirrorMode _mirrorMode; // Nametable mirroring
    bool _hasBattery;       // Battery-backed RAM
    bool _hasTrainer;       // 512-byte trainer

    // Mapper
    std::unique_ptr<Mapper> _mapper;

    // Status
    bool _loaded;
    std::string _filename;

    // iNES header structure
    struct INESHeader
    {
        char name[4];       // "NES" followed by MS-DOS EOF
        uint8_t prgRomSize; // Size of PRG ROM in 16KB units
        uint8_t chrRomSize; // Size of CHR ROM in 8KB units (0 means CHR-RAM)
        uint8_t flags6;     // Mapper, mirroring, battery, trainer
        uint8_t flags7;     // Mapper, VS/Playchoice, NES 2.0
        uint8_t flags8;     // PRG-RAM size (rarely used)
        uint8_t flags9;     // TV system (rarely used)
        uint8_t flags10;    // TV system, PRG-RAM presence (unofficial)
        uint8_t unused[5];  // Unused padding
    };

    static_assert(sizeof(INESHeader) == 16, "iNES header must be 16 bytes");
};

#endif // CARTRIDGE_H