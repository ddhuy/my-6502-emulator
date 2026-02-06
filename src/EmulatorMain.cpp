#include <array>
#include <memory>
#include <iostream>

#include "Bus.h"
#include "CPU.h"
#include "PPU.h"
#include "Memory.h"
#include "Display.h"


int main()
{
    std::cout << "+===========================+" << std::endl;
    std::cout << "|      My NES Emulator      |" << std::endl;
    std::cout << "+===========================+" << std::endl;

    std::unique_ptr<Bus>    bus = std::make_unique<Bus>();
    std::unique_ptr<Memory> memory = std::make_unique<Memory>();
    std::unique_ptr<CPU>    cpu = std::make_unique<CPU>();
    std::unique_ptr<PPU>    ppu = std::make_unique<PPU>();

    std::unique_ptr<Display> display = std::make_unique<Display>("NES Emulator",
                                                                 Display::NES_WIDTH,
                                                                 Display::NES_HEIGHT,
                                                                 3);

    // Connect components
    bus->ConnectMemory(memory.get());
    bus->ConnectCPU(cpu.get());
    bus->ConnectPPU(ppu.get());

    // Initialize display
    if (!display->Init())
    {
        std::cerr << "Failed to initialized display" << std::endl;
        return -1;
    }
    std::cout << "Display initialized successfully!" << std::endl;

    // Reset system
    bus->Reset();
    std::cout << "CPU initialized successfully!" << std::endl;

    // TODO: Load ROM file here

    // Create some simple pattern data (2 tiles: solid and checkered)
    // Tile 0: Solid tile
    for (int i = 0; i < 8; i++)
    {
        ppu->Write(0x0000 + i, 0xFF);      // Low bitplane
        ppu->Write(0x0008 + i, 0xFF);      // High bitplane
    }
    
    // Tile 1: Checkered tile
    for (int i = 0; i < 8; i++) {
        ppu->Write(0x0010 + i, 0xAA);      // Low bitplane (10101010)
        ppu->Write(0x0018 + i, 0x55);      // High bitplane (01010101)
    }
    
    // Fill nametable with checkerboard pattern
    for (uint16_t i = 0; i < 960; i++)
    {
        uint8_t tileId = ((i / 32) + (i % 32)) % 2 ? 0x01 : 0x00;
        ppu->Write(0x2000 + i, tileId);
    }

    // For now, let's write a simple test pattern to PPU memory
    // This writes to the first nametable to create a checkerboard pattern
    for (uint16_t i = 0; i < 960; i++)
    {
        uint8_t tileId = ((i / 32) + (i % 32)) % 2;
        ppu->Write(0x2000 + i, tileId);
    }

    // Fill attribute table (set palette for each 2x2 tile region)
    for (uint16_t i = 0; i < 64; i++)
    {
        // Alternate between palette 0 and palette 1
        uint8_t palette = (i % 2) ? 0x55 : 0x00; // 0x55 = palette 1 for all quadrants
        ppu->Write(0x23C0 + i, palette);
    }
    

    // Set up color palettes
    // Background palette 0
    ppu->Write(0x3F00, 0x0F); // Universal background color (black)
    ppu->Write(0x3F01, 0x00); // Color 1 (dark gray)
    ppu->Write(0x3F02, 0x10); // Color 2 (light gray)
    ppu->Write(0x3F03, 0x30); // Color 3 (white)
    
    // Background palette 1
    ppu->Write(0x3F04, 0x0F); // Universal background (not used but set anyway)
    ppu->Write(0x3F05, 0x02); // Color 1 (blue)
    ppu->Write(0x3F06, 0x16); // Color 2 (red)
    ppu->Write(0x3F07, 0x2A); // Color 3 (green)
    
    // CRITICAL: Enable rendering via PPUMASK register
    // Bit 3 = show background
    // Bit 4 = show sprites
    // Bit 1 = show background in leftmost 8 pixels
    // Bit 2 = show sprites in leftmost 8 pixels
    ppu->CPUWrite(0x2001, 0x1E); // Binary: 00011110
                                 // Enable: bg, sprites, bg left, sprites left
    

    // Main emulation loop
    std::cout << "Entering main loop..." << std::endl;
    
    while (display->IsRunning())
    {
        // Handle input events
        display->HandleEvents();

        // Clock the system until frame is complete
        do
        {
            bus->Clock();
        } while (!ppu->IsFrameComplete());

        // Frame is ready - render it
        ppu->ClearFrameComplete();

        // Clear screen with a background color
        display->Clear();

        // Draw the checkerboard pattern
        display->Render(ppu->GetScreenBuffer());

        // Update the screen
        display->Present();
    }

    std::cout << "Emulator shutting down..." << std::endl;
    display->Shutdown();

    return 0;
}