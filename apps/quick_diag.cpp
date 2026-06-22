#include <iostream>
#include <iomanip>
#include <memory>
#include "bus/Bus.h"
#include "cpu/CPU.h"
#include "ppu/PPU.h"
#include "memory/Memory.h"
#include "cartridge/Cartridge.h"

// Quick diagnostic program to check what's working and what's not

void PrintSeparator()
{
    std::cout << "========================================" << std::endl;
}

void RunQuickDiagnostics(Bus* bus, CPU* cpu, PPU* ppu, Cartridge* cart)
{
    PrintSeparator();
    std::cout << "  NES EMULATOR QUICK DIAGNOSTICS" << std::endl;
    PrintSeparator();
    std::cout << std::endl;
    
    // ========================================
    // 1. CARTRIDGE CHECK
    // ========================================
    std::cout << "1. CARTRIDGE CHECK:" << std::endl;
    if (cart && cart->IsLoaded())
    {
        std::cout << "   ✅ Cartridge loaded" << std::endl;
        std::cout << "   Mapper: " << (int)cart->GetMapperNumber() << std::endl;
        
        // Check reset vector
        uint8_t lo = cart->CPURead(0xFFFC);
        uint8_t hi = cart->CPURead(0xFFFD);
        uint16_t resetVector = (hi << 8) | lo;
        
        std::cout << "   Reset Vector: $" << std::hex << std::uppercase 
                  << std::setw(4) << std::setfill('0') << resetVector << std::dec;
        
        if (resetVector >= 0x8000 && resetVector <= 0xFFFF)
        {
            std::cout << " ✅ VALID" << std::endl;
        }
        else
        {
            std::cout << " ❌ INVALID! (should be $8000-$FFFF)" << std::endl;
        }
        
        // Check NMI vector
        lo = cart->CPURead(0xFFFA);
        hi = cart->CPURead(0xFFFB);
        uint16_t nmiVector = (hi << 8) | lo;
        std::cout << "   NMI Vector: $" << std::hex << std::uppercase 
                  << std::setw(4) << std::setfill('0') << nmiVector << std::dec << std::endl;
        
        // Check first bytes of ROM
        std::cout << "   First 8 bytes at reset: ";
        for (int i = 0; i < 8; i++)
        {
            std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
                      << (int)cart->CPURead(resetVector + i) << " ";
        }
        std::cout << std::dec << std::endl;
    }
    else
    {
        std::cout << "   ❌ Cartridge not loaded" << std::endl;
    }
    
    std::cout << std::endl;
    
    // ========================================
    // 2. CPU CHECK
    // ========================================
    std::cout << "2. CPU CHECK:" << std::endl;
    if (cpu)
    {
        uint16_t pc = cpu->PC;
        std::cout << "   PC: $" << std::hex << std::uppercase 
                  << std::setw(4) << std::setfill('0') << pc << std::dec;
        
        if (pc >= 0x8000 && pc <= 0xFFFF)
        {
            std::cout << " ✅ VALID" << std::endl;
        }
        else
        {
            std::cout << " ❌ INVALID! (should be $8000-$FFFF)" << std::endl;
        }
        
        std::cout << "   A: $" << std::hex << std::uppercase << std::setw(2) 
                  << (int)cpu->A << std::dec << std::endl;
        std::cout << "   X: $" << std::hex << std::uppercase << std::setw(2) 
                  << (int)cpu->X << std::dec << std::endl;
        std::cout << "   Y: $" << std::hex << std::uppercase << std::setw(2) 
                  << (int)cpu->Y << std::dec << std::endl;
        std::cout << "   SP: $" << std::hex << std::uppercase << std::setw(2) 
                  << (int)cpu->SP << std::dec << std::endl;
        std::cout << "   P: $" << std::hex << std::uppercase << std::setw(2) 
                  << (int)cpu->P << std::dec << std::endl;
    }
    else
    {
        std::cout << "   ❌ CPU is null" << std::endl;
    }
    
    std::cout << std::endl;
    
    // ========================================
    // 3. CPU NMI CHECK
    // ========================================
    std::cout << "3. CPU NMI CHECK:" << std::endl;
    std::cout << "   ⚠️  CRITICAL: Check these manually:" << std::endl;
    std::cout << "   - Does CPU.h have 'void NMI();' declared?" << std::endl;
    std::cout << "   - Does CPU.cpp have NMI() implemented?" << std::endl;
    std::cout << "   - Is '_cpu->NMI();' called in Bus::Clock()?" << std::endl;
    std::cout << "   - Is that line NOT commented out?" << std::endl;
    
    std::cout << std::endl;
    
    // ========================================
    // 4. EXECUTION TEST
    // ========================================
    std::cout << "4. EXECUTION TEST (10 instructions):" << std::endl;
    
    uint16_t startPC = cpu->PC;
    std::cout << "   Starting PC: $" << std::hex << std::uppercase 
              << std::setw(4) << std::setfill('0') << startPC << std::dec << std::endl;
    
    for (int i = 0; i < 10; i++)
    {
        uint16_t pc = cpu->PC;
        uint8_t opcode = bus->CPURead(pc);
        
        std::cout << "   Instr " << std::setw(2) << i << ": PC=$" 
                  << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << pc
                  << " OP=$" << std::setw(2) << (int)opcode << std::dec << std::endl;
        
        // Execute one CPU instruction (not one clock!)
        // You might need to execute multiple clocks per instruction
        int maxClocks = 10;  // Safety limit
        int startCycles = cpu->GetCycles();
        
        while (cpu->GetCycles() == startCycles && maxClocks-- > 0)
        {
            bus->Clock();
        }
    }
    
    uint16_t endPC = cpu->PC;
    std::cout << "   Ending PC: $" << std::hex << std::uppercase 
              << std::setw(4) << std::setfill('0') << endPC << std::dec << std::endl;
    
    if (endPC != startPC)
    {
        std::cout << "   ✅ PC CHANGED - CPU is executing!" << std::endl;
    }
    else
    {
        std::cout << "   ❌ PC STUCK - CPU not executing or infinite loop!" << std::endl;
    }
    
    std::cout << std::endl;
    
    // ========================================
    // 5. PPU CHECK
    // ========================================
    std::cout << "5. PPU CHECK:" << std::endl;
    if (ppu)
    {
        std::cout << "   Scanline: " << ppu->GetScanline() << std::endl;
        std::cout << "   Cycle: " << ppu->GetCycle() << std::endl;
        std::cout << "   Frame: " << ppu->GetFrameCount() << std::endl;
        
        // Enable rendering
        ppu->CPUWrite(0x2001, 0x1E);
        std::cout << "   ✅ Set PPUMASK = $1E (rendering enabled)" << std::endl;
        
        // Enable NMI
        ppu->CPUWrite(0x2000, 0x80);
        std::cout << "   ✅ Set PPUCTRL = $80 (NMI enabled)" << std::endl;
    }
    else
    {
        std::cout << "   ❌ PPU is null" << std::endl;
    }
    
    std::cout << std::endl;
    
    // ========================================
    // 6. WAIT FOR NMI TEST
    // ========================================
    std::cout << "6. NMI TRIGGER TEST:" << std::endl;
    std::cout << "   Running for ~1 frame to check for NMI..." << std::endl;
    
    int clocksToRun = 29781 * 3;  // One frame = 29781 CPU cycles * 3 (PPU/CPU ratio)
    bool nmiSeen = false;
    
    for (int i = 0; i < clocksToRun; i++)
    {
        if (ppu->NMIOccurred())
        {
            nmiSeen = true;
            std::cout << "   ✅ NMI FLAG DETECTED at clock " << i << "!" << std::endl;
            break;
        }
        bus->Clock();
    }
    
    if (!nmiSeen)
    {
        std::cout << "   ❌ NO NMI detected after 1 frame!" << std::endl;
        std::cout << "      Current scanline: " << ppu->GetScanline() << std::endl;
        std::cout << "      This is a CRITICAL problem!" << std::endl;
    }
    
    std::cout << std::endl;
    
    // ========================================
    // SUMMARY
    // ========================================
    PrintSeparator();
    std::cout << "  SUMMARY" << std::endl;
    PrintSeparator();
    std::cout << std::endl;
    
    std::cout << "Common issues to check:" << std::endl;
    std::cout << "  1. CPU::NMI() must be implemented" << std::endl;
    std::cout << "  2. _cpu->NMI() must be called in Bus::Clock()" << std::endl;
    std::cout << "  3. That call must NOT be commented out" << std::endl;
    std::cout << "  4. Reset vector must be valid ($8000+)" << std::endl;
    std::cout << "  5. PPU must trigger VBlank at scanline 241" << std::endl;
    std::cout << "  6. PPUCTRL bit 7 must enable NMI generation" << std::endl;
    std::cout << std::endl;
    
    PrintSeparator();
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <rom.nes>" << std::endl;
        return 1;
    }
    
    // Create components
    auto bus = std::make_unique<Bus>();
    auto memory = std::make_unique<Memory>();
    auto cpu = std::make_unique<CPU>();
    auto ppu = std::make_unique<PPU>();
    auto cartridge = std::make_unique<Cartridge>();
    
    // Connect
    bus->ConnectMemory(memory.get());
    bus->ConnectCPU(cpu.get());
    bus->ConnectPPU(ppu.get());
    cpu->ConnectBus(bus.get());
    ppu->ConnectBus(bus.get());
    
    // Load ROM
    if (!cartridge->LoadFromFile(argv[1]))
    {
        std::cerr << "Failed to load ROM: " << argv[1] << std::endl;
        return 1;
    }
    
    bus->InsertCartridge(cartridge.get());
    ppu->ConnectCartridge(cartridge.get());
    
    // Reset
    bus->Reset();
    
    // Run diagnostics
    RunQuickDiagnostics(bus.get(), cpu.get(), ppu.get(), cartridge.get());
    
    return 0;
}