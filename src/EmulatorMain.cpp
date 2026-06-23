#include <array>
#include <memory>
#include <iostream>

#include "bus/Bus.h"
#include "ppu/Display.h"
#include "utils/Logger.h"


int main(int argc, char **argv)
{
    LOG_INFO("+===========================+");
    LOG_INFO("|      My NES Emulator      |");
    LOG_INFO("+===========================+");

    std::unique_ptr<Bus>    bus = std::make_unique<Bus>();
    std::unique_ptr<Memory> memory = std::make_unique<Memory>();
    std::unique_ptr<CPU>    cpu = std::make_unique<CPU>();
    std::unique_ptr<PPU>    ppu = std::make_unique<PPU>();
    std::unique_ptr<Cartridge> cartridge = std::make_unique<Cartridge>();

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
        LOG_ERROR("Failed to initialize display");
        return -1;
    }
    LOG_INFO("Display initialized successfully!");

    // Load ROM file here
    bool romLoaded = false;
    if (argc >= 2)
    {
        if (cartridge->LoadFromFile(argv[1]))
        {
            bus->InsertCartridge(cartridge.get());
            romLoaded = true;
        }
        else
        {
            LOG_ERROR("Failed to load ROM, running test pattern instead");
        }
    }
    else
    {
        LOG_INFO("No ROM specified, running test pattern");
    }

    // Reset system
    bus->Reset();
    LOG_INFO("CPU initialized successfully!");

    if (!romLoaded)
    {
		LOG_INFO("Creating test pattern...");

        // Create some simple pattern data (2 tiles: solid and checkered)
        // Tile 0: Solid tile
        for (int i = 0; i < 8; i++)
        {
            ppu->PPUWrite(0x0000 + i, 0xFF);      // Low bitplane
            ppu->PPUWrite(0x0008 + i, 0xFF);      // High bitplane
        }
        
        // Tile 1: Checkered tile
        for (int i = 0; i < 8; i++) {
            ppu->PPUWrite(0x0010 + i, 0xAA);      // Low bitplane (10101010)
            ppu->PPUWrite(0x0018 + i, 0x55);      // High bitplane (01010101)
        }

        // This writes to the first nametable to create a checkerboard pattern
        for (uint16_t i = 0; i < 960; i++)
        {
            uint8_t tileId = ((i / 32) + (i % 32)) % 2;
            ppu->PPUWrite(0x2000 + i, tileId);
        }

        // Fill attribute table (set palette for each 2x2 tile region)
        for (uint16_t i = 0; i < 64; i++)
        {
            // Alternate between palette 0 and palette 1
            uint8_t palette = (i % 2) ? 0x55 : 0x00; // 0x55 = palette 1 for all quadrants
            ppu->PPUWrite(0x23C0 + i, palette);
        }

        // Set up color palettes
        // Background palette 0
        ppu->PPUWrite(0x3F00, 0x0F); // Universal background color (black)
        ppu->PPUWrite(0x3F01, 0x00); // Color 1 (dark gray)
        ppu->PPUWrite(0x3F02, 0x10); // Color 2 (light gray)
        ppu->PPUWrite(0x3F03, 0x30); // Color 3 (white)
        
        // Background palette 1
        ppu->PPUWrite(0x3F04, 0x0F); // Universal background (not used but set anyway)
        ppu->PPUWrite(0x3F05, 0x02); // Color 1 (blue)
        ppu->PPUWrite(0x3F06, 0x16); // Color 2 (red)
        ppu->PPUWrite(0x3F07, 0x2A); // Color 3 (green)
        
        // CRITICAL: Enable rendering via PPUMASK register
        // Bit 3 = show background
        // Bit 4 = show sprites
        // Bit 1 = show background in leftmost 8 pixels
        // Bit 2 = show sprites in leftmost 8 pixels
        ppu->CPUWrite(0x2001, 0x1E); // Binary: 00011110
                                     // Enable: bg, sprites, bg left, sprites left
    }
    else
    {
        LOG_INFO("ROM loaded, starting emulation...");
    }

    // Main emulation loop
    LOG_INFO("Entering main loop... (Press ESC to exit)");

    uint64_t frameCount = 0;
    
    while (display->IsRunning())
    {
        // Handle input events
        display->HandleEvents();

        // Poll pad 1 and hand it to the bus before clocking the frame
        uint8_t s = display->GetController1State();
        if (s) std::cerr << "pad: " << std::hex << (int)s << "\n";
        
        bus->SetControllerState(0, s);

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

        // Print FPS
        frameCount++;
        if (frameCount % 60 == 0)
            LOG_INFO("Frame: %ld", frameCount);
    }

    LOG_INFO("Total frames rendered: %ld", frameCount);
    LOG_INFO("Emulator shutting down...");
    // display->Shutdown();

    return 0;
}