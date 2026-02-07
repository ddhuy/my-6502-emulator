#include <iostream>
#include <iomanip>
#include <memory>
#include "cartridge/Cartridge.h"

// ROM Diagnostic Tool
// Usage: ./rom_diagnostic donkeykong.nes

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <rom_file.nes>" << std::endl;
        return 1;
    }

    std::cout << "========================================" << std::endl;
    std::cout << "     NES ROM Diagnostic Tool" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    auto cartridge = std::make_unique<Cartridge>();
    
    if (!cartridge->LoadFromFile(argv[1]))
    {
        std::cerr << "Failed to load ROM!" << std::endl;
        return 1;
    }

    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "VECTOR TABLE" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Read interrupt vectors
    uint8_t nmiLo = cartridge->CPURead(0xFFFA);
    uint8_t nmiHi = cartridge->CPURead(0xFFFB);
    uint16_t nmiVector = (nmiHi << 8) | nmiLo;
    
    uint8_t resetLo = cartridge->CPURead(0xFFFC);
    uint8_t resetHi = cartridge->CPURead(0xFFFD);
    uint16_t resetVector = (resetHi << 8) | resetLo;
    
    uint8_t irqLo = cartridge->CPURead(0xFFFE);
    uint8_t irqHi = cartridge->CPURead(0xFFFF);
    uint16_t irqVector = (irqHi << 8) | irqLo;
    
    std::cout << "NMI   Vector ($FFFA-$FFFB): $" << std::hex << std::uppercase 
              << std::setw(4) << std::setfill('0') << nmiVector << std::endl;
    std::cout << "RESET Vector ($FFFC-$FFFD): $" << std::hex << std::uppercase 
              << std::setw(4) << std::setfill('0') << resetVector << std::endl;
    std::cout << "IRQ   Vector ($FFFE-$FFFF): $" << std::hex << std::uppercase 
              << std::setw(4) << std::setfill('0') << irqVector << std::endl;
    
    std::cout << std::endl;
    
    // Check if vectors are in valid range
    bool vectorsOk = true;
    if (resetVector < 0x8000)
    {
        std::cout << "⚠️  WARNING: Reset vector is outside PRG-ROM range!" << std::endl;
        vectorsOk = false;
    }
    if (nmiVector < 0x8000)
    {
        std::cout << "⚠️  WARNING: NMI vector is outside PRG-ROM range!" << std::endl;
        vectorsOk = false;
    }
    if (irqVector < 0x8000)
    {
        std::cout << "⚠️  WARNING: IRQ vector is outside PRG-ROM range!" << std::endl;
        vectorsOk = false;
    }
    
    if (vectorsOk)
        std::cout << "✅ All vectors point to valid PRG-ROM addresses" << std::endl;
    
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "PRG-ROM DUMP" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Dump code at reset vector
    std::cout << "Code at RESET vector ($" << std::hex << std::uppercase 
              << std::setw(4) << std::setfill('0') << resetVector << "):" << std::endl;
    std::cout << "  ";
    for (int i = 0; i < 32; i++)
    {
        if (i > 0 && i % 16 == 0)
            std::cout << std::endl << "  ";
        std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') 
                  << (int)cartridge->CPURead(resetVector + i) << " ";
    }
    std::cout << std::endl << std::endl;
    
    // Dump code at NMI vector
    std::cout << "Code at NMI vector ($" << std::hex << std::uppercase 
              << std::setw(4) << std::setfill('0') << nmiVector << "):" << std::endl;
    std::cout << "  ";
    for (int i = 0; i < 32; i++)
    {
        if (i > 0 && i % 16 == 0)
            std::cout << std::endl << "  ";
        std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') 
                  << (int)cartridge->CPURead(nmiVector + i) << " ";
    }
    std::cout << std::endl << std::endl;
    
    std::cout << std::dec;
    std::cout << "========================================" << std::endl;
    std::cout << "CHR-ROM CHECK" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Check if CHR-ROM has data
    bool chrEmpty = true;
    for (int i = 0; i < 256; i++)
    {
        if (cartridge->PPURead(i) != 0)
        {
            chrEmpty = false;
            break;
        }
    }
    
    if (chrEmpty)
    {
        std::cout << "⚠️  WARNING: CHR-ROM appears to be empty!" << std::endl;
    }
    else
    {
        std::cout << "✅ CHR-ROM contains data" << std::endl;
        std::cout << std::endl;
        std::cout << "First 128 bytes of CHR-ROM:" << std::endl;
        for (int row = 0; row < 8; row++)
        {
            std::cout << "  ";
            for (int col = 0; col < 16; col++)
            {
                std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') 
                          << (int)cartridge->PPURead(row * 16 + col) << " ";
            }
            std::cout << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << std::dec;
    std::cout << "========================================" << std::endl;
    std::cout << "EXPECTED BEHAVIOR" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "When emulator runs:" << std::endl;
    std::cout << "1. CPU should reset and jump to $" << std::hex << std::uppercase 
              << std::setw(4) << std::setfill('0') << resetVector << std::endl;
    std::cout << "2. Game initializes PPU and enables NMI" << std::endl;
    std::cout << "3. At VBlank, PPU triggers NMI" << std::endl;
    std::cout << "4. CPU jumps to $" << std::hex << std::uppercase 
              << std::setw(4) << std::setfill('0') << nmiVector << std::endl;
    std::cout << "5. Game renders frame in NMI handler" << std::endl;
    std::cout << "6. Repeat step 3-5 at 60 FPS" << std::endl;
    std::cout << std::dec << std::endl;
    
    std::cout << "========================================" << std::endl;
    std::cout << "DIAGNOSTIC COMPLETE" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}