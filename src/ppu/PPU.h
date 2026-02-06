#ifndef PPU_H
#define PPU_H

#include <array>
#include <cstdint>

// Forward declarationc
class Bus;

class PPU
{
public:
    PPU();
    ~PPU() = default;

    // Reset PPU to initial state
    void Reset();
    
    // Connect to bus for CPU communication
    void ConnectBus(Bus* b) { _bus = b; }

    // Clock the PPU (called 3 times per CPU cycle)
    void Clock();

    // CPU reads from PPU registers ($2000-$2007)
    uint8_t CPURead(uint16_t address);

    // CPU writes to PPU registers ($2000-$2007)
    void CPUWrite(uint16_t address, uint8_t value);

    // PPU reads from its own memory space
    uint8_t Read(uint16_t address);
    
    // PPU writes to its own memory space
    void Write(uint16_t address, uint8_t value);

    // Check if frame is complete (ready to render)
    bool IsFrameComplete() const { return _frameComplete; }
    void ClearFrameComplete() { _frameComplete = false; }

    // Check if NMI should be triggered
    bool NMIOccurred() const { return _nmiOutput; };
    void ClearNMI() { _nmiOutput = false; };

    // Get screen buffer (256x240 pixels, NES color palette indices 0-63)
    const uint8_t* GetScreenBuffer() const { return _screen.data(); };

    static constexpr int SCREEN_WIDTH  = 256;
    static constexpr int SCREEN_HEIGHT = 240;

private:
    // PPU registers (mapped to $2000-$2007)
    union PPUCtrl
    {
        struct
        {
            uint8_t nametableX : 1;    // Bit 0: Nametable X
            uint8_t nametableY : 1;    // Bit 1: Nametable Y
            uint8_t incrementMode : 1; // Bit 2: VRAM increment (0: +1, 1: +32)
            uint8_t spritePattern : 1; // Bit 3: Sprite pattern table
            uint8_t bgPattern  : 1;    // Bit 4: Background pattern table
            uint8_t spriteSize : 1;    // Bit 5: Sprite size (0: 8x8, 1: 8x16)
            uint8_t slaveMode  : 1;    // Bit 6: PPU Master/Slave
            uint8_t enableNMI  : 1;    // Bit 7: Generate NMI at V-Blank
        };

        uint8_t reg;

    } _ctrl;

    union PPUMask
    {
        struct
        {
            uint8_t grayscale : 1;          // Bit 0: Grayscale
            uint8_t showBgLeft : 1;         // Bit 1: Show background in leftmost 8 pixels
            uint8_t showSpritesLeft : 1;    // Bit 2: Show sprites in leftmost 8 pixels
            uint8_t showBg : 1;             // Bit 3: Show background
            uint8_t showSprites : 1;        // Bit 4: Show sprites
            uint8_t emphasizeRed : 1;       // Bit 5: Emphasize red
            uint8_t emphasizeGreen : 1;     // Bit 6: Emphasize green
            uint8_t emphasizeBlue : 1;      // Bit 7: Emphasize blue
        };

        uint8_t reg;

    } _mask;

    union PPUStatus
    {
        struct
        {
            uint8_t unused : 5;             // Bits 0-4: Unused
            uint8_t spriteOverflow : 1;     // Bit 5: Sprite overflow
            uint8_t sprite0Hit : 1;         // Bit 6: Sprite 0 hit
            uint8_t vblank : 1;             // Bit 7: V-Blank flag
        };

        uint8_t reg;

    } _status;

    // Internal registers
    uint8_t _oamAddress;     // OAM address register ($2003)
    uint8_t _ppuDataBuffer;  // Data buffer for $2007 reads

    // Scrolling registers (internal)
    union LoopyRegister
    {
        struct
        {
            uint16_t coarseX : 5;      // Bits 0-4
            uint16_t coarseY : 5;      // Bits 5-9
            uint16_t nametableX : 1;   // Bit 10
            uint16_t nametableY : 1;   // Bit 11
            uint16_t fineY : 3;        // Bits 12-14
            uint16_t unused : 1;       // Bit 15
        };
        
        uint16_t reg;
    
    };

    LoopyRegister _vramAddr; // Current VRAM address (15 bits)
    LoopyRegister _tramAddr; // Temporaty VRAM address (15 bits)
    uint8_t _fineX;          // Fine X scroll (3 bits)
    bool _addressLatch;      // First or second write toggle

    // Rendering counters
    int16_t _scanline;  // Current scanline (-1 to 260)
    int16_t _cycle;     // Current cycle (0 to 340)

    // Frame tracking
    bool _frameComplete;
    bool _nmiOutput;
    uint64_t _frameCount;

    // PPU Memory
    std::array<uint8_t, 2048> _nametable;    // 2KB nametable RAM
    std::array<uint8_t, 32>   _palette;      // 32 bytes palette RAM
    std::array<uint8_t, 256>  _oam;          // 256 bytes Object Attribute Memory (sprites)
    std::array<uint8_t, 32>   _secondaryOam; // 32 bytes secondary OAM for current scanline

    // Pattern table memory (CHR-ROM/RAM) - handled by cartridge
    // For now, we'll use dummy pattern tables
    std::array<uint8_t, 8192> _patternTable; // 8KB pattern tables (temoprar)

    // Screen buffer - stores palette indices for each pixel
    std::array<uint8_t, SCREEN_WIDTH * SCREEN_HEIGHT> _screen;

    // Rendering pipeline data
    struct BackgroundShifters
    {
        uint16_t patternLo;
        uint16_t patternHi;
        uint16_t attributeLo;
        uint16_t attributeHi;
    } _bgShifters;

    uint8_t _bgNextTileId;
    uint8_t _bgNextTileAttrib;
    uint8_t _bgNextTileLsb;
    uint8_t _bgNextTileMsb;

    // Sprite rendering data
    struct SpriteData
    {
        uint8_t y;
        uint8_t tileId;
        uint8_t attribute;
        uint8_t x;
    };
    std::array<SpriteData, 8> _spriteScanline;
    std::array<uint8_t, 8> _spriteShifterPatternLo;
    std::array<uint8_t, 8> _spriteShifterPatternHi;
    uint8_t _spriteCount;
    bool _sprite0HitPossible;
    bool _sprite0Rendering;

    // Helper functions
    void IncrementScrollX();
    void IncrementScrollY();
    void TransferAddressX();
    void TransferAddressY();
    void LoadBackgroundShifters();
    void UpdateShifters();
    
    // Sprite evaluation
    void EvaluateSprites();
    
    // Rendering
    uint8_t GetBackgroundPixel();
    uint8_t GetSpritePixel(bool& spritePriority);

    // Bus connection
    Bus* _bus;
};

#endif // PPU_H