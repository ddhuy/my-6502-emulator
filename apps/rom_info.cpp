#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <cstring>
#include <cstdint>


struct INESHeader
{
    char name[4];
    uint8_t prgRomSize;
    uint8_t chrRomSize;
    uint8_t flags6;
    uint8_t flags7;
    uint8_t flags8;
    uint8_t flags9;
    uint8_t flags10;
    uint8_t unused[5];
};


void printRomInfo(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open: " << filename << std::endl;
        return;
    }
    
    INESHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    
    if (header.name[0] != 'N' || header.name[1] != 'E' || 
        header.name[2] != 'S' || header.name[3] != 0x1A)
    {
        std::cerr << "Not a valid iNES file!" << std::endl;
        return;
    }
    
    std::cout << "========================================\n";
    std::cout << "NES ROM Information\n";
    std::cout << "========================================\n";
    std::cout << "File: " << filename << "\n\n";
    
    // Basic info
    std::cout << "PRG-ROM: " << (int)header.prgRomSize << " x 16KB = " 
              << (header.prgRomSize * 16) << " KB\n";
    std::cout << "CHR-ROM: " << (int)header.chrRomSize << " x 8KB = "
              << (header.chrRomSize * 8) << " KB";
    if (header.chrRomSize == 0)
    {
        std::cout << " (Uses CHR-RAM)";
    }
    std::cout << "\n\n";
    
    // Mapper
    uint8_t mapperNumber = ((header.flags7 & 0xF0) | (header.flags6 >> 4));
    std::cout << "Mapper: " << (int)mapperNumber;
    switch (mapperNumber)
    {
        case 0: std::cout << " (NROM)"; break;
        case 1: std::cout << " (MMC1)"; break;
        case 2: std::cout << " (UxROM)"; break;
        case 3: std::cout << " (CNROM)"; break;
        case 4: std::cout << " (MMC3)"; break;
        case 7: std::cout << " (AxROM)"; break;
        default: std::cout << " (Unknown)"; break;
    }
    std::cout << "\n\n";
    
    // Mirroring
    std::cout << "Mirroring: ";
    if (header.flags6 & 0x08)
    {
        std::cout << "Four-screen\n";
    }
    else if (header.flags6 & 0x01)
    {
        std::cout << "Vertical\n";
    }
    else
    {
        std::cout << "Horizontal\n";
    }
    
    // Flags
    std::cout << "Battery: " << ((header.flags6 & 0x02) ? "Yes" : "No") << "\n";
    std::cout << "Trainer: " << ((header.flags6 & 0x04) ? "Yes (512 bytes)" : "No") << "\n";
    
    // Format
    if ((header.flags7 & 0x0C) == 0x08)
    {
        std::cout << "Format: NES 2.0\n";
    }
    else
    {
        std::cout << "Format: iNES\n";
    }
    
    // Calculate file size
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    size_t expectedSize = 16; // Header
    if (header.flags6 & 0x04) expectedSize += 512; // Trainer
    expectedSize += header.prgRomSize * 16384; // PRG-ROM
    expectedSize += header.chrRomSize * 8192;  // CHR-ROM
    
    std::cout << "\nFile Size: " << fileSize << " bytes\n";
    std::cout << "Expected: " << expectedSize << " bytes\n";
    if (fileSize != expectedSize)
    {
        std::cout << "⚠️  Size mismatch! File may be corrupted or have extra data.\n";
    }
    
    std::cout << "\n";
    std::cout << "Flags 6: 0x" << std::hex << std::setw(2) << std::setfill('0') 
              << (int)header.flags6 << " (";
    for (int i = 7; i >= 0; i--)
    {
        std::cout << ((header.flags6 & (1 << i)) ? '1' : '0');
    }
    std::cout << "b)\n" << std::dec;
    
    std::cout << "Flags 7: 0x" << std::hex << std::setw(2) << std::setfill('0')
              << (int)header.flags7 << " (";
    for (int i = 7; i >= 0; i--)
    {
        std::cout << ((header.flags7 & (1 << i)) ? '1' : '0');
    }
    std::cout << "b)\n" << std::dec;
    
    std::cout << "========================================\n";
}

int main(int argc, char* argv[])
{
    std::cout << "NES ROM Info Utility\n\n";
    
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <rom_file.nes>\n";
        return 1;
    }
    
    for (int i = 1; i < argc; i++)
    {
        printRomInfo(argv[i]);
        if (i < argc - 1)
        {
            std::cout << "\n";
        }
    }
    
    return 0;
}