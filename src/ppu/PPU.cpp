#include "PPU.h"
#include "bus/Bus.h"
#include "cartridge/Cartridge.h"


PPU::PPU() 
    : _oamAddress(0),
      _ppuDataBuffer(0),
      _fineX(0),
      _addressLatch(false),
      _scanline(0),
      _cycle(0),
      _frameComplete(false),
      _nmiOutput(false),
      _frameCount(0),
      _spriteCount(0),
      _sprite0HitPossible(false),
      _sprite0Rendering(false),
      _bus(nullptr),
	  _cartridge(nullptr)
{
    
    _ctrl.reg = 0;
    _mask.reg = 0;
    _status.reg = 0;
    _vramAddr.reg = 0;
    _tramAddr.reg = 0;

    _nametable.fill(0);
    _palette.fill(0);
    _oam.fill(0);
    _secondaryOam.fill(0);
    _screen.fill(0);
    _patternTable.fill(0);
    
    _bgShifters.patternLo = 0;
    _bgShifters.patternHi = 0;
    _bgShifters.attributeLo = 0;
    _bgShifters.attributeHi = 0;
}

void PPU::Reset()
{
    _ctrl.reg = 0;
    _mask.reg = 0;
    _status.reg = 0;
    _oamAddress = 0;
    _ppuDataBuffer = 0;
    _fineX = 0;
    _addressLatch = false;
    _vramAddr.reg = 0;
    _tramAddr.reg = 0;
    
    _scanline = 0;
    _cycle = 0;
    _frameComplete = false;
    _nmiOutput = false;
    
    _bgShifters.patternLo = 0;
    _bgShifters.patternHi = 0;
    _bgShifters.attributeLo = 0;
    _bgShifters.attributeHi = 0;
}

uint8_t PPU::CPURead(uint16_t address)
{
    uint8_t data = 0x0;

    address &= 0x0007; // Mirror down to $2000-$2007

    switch (address)
    {
        case 0x0000: // PPUCTRL - write only
            break;

        case 0x0001: // PPUMASK - write only
            break;

        case 0x0002: // PPUSTATUS
        {
            data = (_status.reg & 0xE0) | (_ppuDataBuffer & 0x1F);
            _status.vblank = 0; // Reading clears V-Blank flag
            _addressLatch = false; // Reset address latch
            break;
        }

        case 0x0003: // OAMADDR - write only
            break;

        case 0x0004: // OAMDATA
        {
            data = _oam[_oamAddress];
            break;
        }

        case 0x0005: // PPUSCROLL - write only
            break;

        case 0x0006: // PPUADDR - write only
            break;

        case 0x0007: // PPUDATA
        {
            data = _ppuDataBuffer;
            _ppuDataBuffer = PPURead(_vramAddr.reg);

            // Palette reads are not buffered
            if (_vramAddr.reg >= 0x3F00)
                data = _ppuDataBuffer;

            // Increment VRAM address
            _vramAddr.reg += (_ctrl.incrementMode ? 32 : 1);

            break;
        }
    }

    return data;
}

void PPU::CPUWrite(uint16_t address, uint8_t data)
{
    address &= 0x0007; // Mirror down to $2000-$2007

    switch (address)
    {
        case 0x0000: // PPUCTRL
        {
            _ctrl.reg = data;
            _tramAddr.nametableX = _ctrl.nametableX;
            _tramAddr.nametableY = _ctrl.nametableY;
            break;
        }

        case 0x0001: // PPUMASK
        {
            _mask.reg = data;
            break;
        }

        case 0x0002: // PPUSTATUS - read only
            break;

        case 0x0003: // OAMADDR
        {
            _oamAddress = data;
            break;
        }

        case 0x0004: // OAMDATA
        {
            _oam[_oamAddress++] = data;
            break;
        }

        case 0x0005: // PPUSCROLL
        {
            if (!_addressLatch)
            {
                // First write - X Scroll
                _fineX = data & 0x07;
                _tramAddr.coarseX = data >> 3;
                _addressLatch = true;
            }
            else
            {
                // Second write - y Scroll
                _tramAddr.fineY = data & 0x07;
                _tramAddr.coarseY = data >> 3;
                _addressLatch = false;
            }

            break;
        }

        case 0x0006: // PPUADDR
        {
            if (!_addressLatch)
            {
                // First write - high byte
                _tramAddr.reg = ((data & 0x3F) << 8) | (_tramAddr.reg & 0x00FF);
                _addressLatch = true;
            }
            else
            {
                // Second write - low byte
                _tramAddr.reg = (_tramAddr.reg & 0xFF00) | data;
                _vramAddr.reg = _tramAddr.reg;
                _addressLatch = false;
            }

            break;
        }

        case 0x0007: // PPUDATA
        {
            PPUWrite(_vramAddr.reg, data);
            _vramAddr.reg += (_ctrl.incrementMode ? 32 : 1);
            break;
        }
    }
}

uint8_t PPU::PPURead(uint16_t address)
{
    address &= 0x3FFF; // Mirror down to 14-bit address space

    // Pattern tables (CHR-ROM/RAM) - TODO: Connect to Cartridge
    if (address < 0x2000)
    {
		// Pattern tables (CHR-ROM/RAM) - Use cartridge
		if (_cartridge)
			return _cartridge->PPURead(address);

        return _patternTable[address];
    }
    // nametables
    else if (address < 0x3F00)
    {
        address &= 0x0FFF;

        // Mirror nametables (horizontal mirroring for now)
        // TODO: Proper mirroring based cartridge
        if (address < 0x0400)
            return _nametable[address & 0x03FF];
        else if (address < 0x0800)
            return _nametable[address & 0x03FF];
        else if (address < 0x0C00)
            return _nametable[(address & 0x03FF) + 0x0400];
        else
            return _nametable[(address & 0x03FF) + 0x0400];
    }
    // Palette RAM
    else if (address < 0x4000)
    {
        address &= 0x001F;

        // Mirror background palette indices
        if (address == 0x0010) address = 0x0000;
        if (address == 0x0014) address = 0x0004;
        if (address == 0x0018) address = 0x0008;
        if (address == 0x001C) address = 0x000C;

        return _palette[address] & (_mask.grayscale ? 0x30 : 0x3F);
    }
    
    return 0x00;
}

void PPU::PPUWrite(uint16_t address, uint8_t data)
{
    address &= 0x3FFF; // Mirror down to 14-bit address space

    // Pattern tables - TODO: Some cartridges have CHR-RAM
    if (address < 0x2000)
    {
		// Pattern tables - Use cartridge (for CHR-RAM)
		if (_cartridge)
			_cartridge->PPUWrite(address, data);
		else
	        _patternTable[address] = data;
    }
    // Nametables
    else if (address < 0x3F00)
    {
        address &= 0x0FFF;

        if (address < 0x0400)
            _nametable[address & 0x03FF] = data;
        else if (address < 0x0800)
            _nametable[address & 0x03FF] = data;
        else if (address < 0x0C00)
            _nametable[(address & 0x03FF) + 0x0400] = data;
        else
            _nametable[(address & 0x03FF) + 0x0400] = data;
    }
    // Palette RAM
    else if (address < 0x4000)
    {
        address &= 0x001F;

        // Mirror background palette indices
        if (address == 0x0010) address = 0x0000;
        if (address == 0x0014) address = 0x0004;
        if (address == 0x0018) address = 0x0008;
        if (address == 0x001C) address = 0x000C;

        _palette[address] = data;
    }

}

void PPU::Clock()
{
    // Visible scanlines (0-239) and pre-render scanline (261/-1)
    if (_scanline >= -1 && _scanline < 240)
    {
        // Skip cycle 0 of scanline -1 on odd frames
        if (_scanline == -1 && _cycle == 1)
        {
            _status.vblank = 0;
            _status.sprite0Hit = 0;
            _status.spriteOverflow = 0;
            _sprite0HitPossible = false;
        }
        
        if ((_cycle >= 2 && _cycle < 258) || (_cycle >= 321 && _cycle < 338))
        {
            UpdateShifters();
            
            switch ((_cycle - 1) % 8)
            {
                case 0:
                    LoadBackgroundShifters();
                    _bgNextTileId = PPURead(0x2000 | (_vramAddr.reg & 0x0FFF));
                    break;
                case 2:
                    {
                        uint16_t attribAddr = 0x23C0 | (_vramAddr.nametableY << 11) |
                                            (_vramAddr.nametableX << 10) |
                                            ((_vramAddr.coarseY >> 2) << 3) |
                                            (_vramAddr.coarseX >> 2);
                        _bgNextTileAttrib = PPURead(attribAddr);
                        
                        if (_vramAddr.coarseY & 0x02) _bgNextTileAttrib >>= 4;
                        if (_vramAddr.coarseX & 0x02) _bgNextTileAttrib >>= 2;
                        _bgNextTileAttrib &= 0x03;
                    }
                    break;
                case 4:
                    {
                        uint16_t patternAddr = (_ctrl.bgPattern << 12) +
                                             ((uint16_t)_bgNextTileId << 4) +
                                             _vramAddr.fineY;
                        _bgNextTileLsb = PPURead(patternAddr);
                    }
                    break;
                case 6:
                    {
                        uint16_t patternAddr = (_ctrl.bgPattern << 12) +
                                             ((uint16_t)_bgNextTileId << 4) +
                                             _vramAddr.fineY + 8;
                        _bgNextTileMsb = PPURead(patternAddr);
                    }
                    break;
                case 7:
                    IncrementScrollX();
                    break;
            }
        }
        
        if (_cycle == 256)
        {
            IncrementScrollY();
        }
        
        if (_cycle == 257)
        {
            LoadBackgroundShifters();
            TransferAddressX();
            
            if (_scanline >= 0)
            {
                EvaluateSprites();
            }
        }
        
        if (_scanline == -1 && _cycle >= 280 && _cycle < 305)
        {
            TransferAddressY();
        }
        
        // Sprite fetching
        if (_cycle == 340) {
            // Prepare sprites for next scanline
        }
    }
    
    // V-Blank scanlines (240-260)
    if (_scanline == 241 && _cycle == 1) {
        _status.vblank = 1;
        if (_ctrl.enableNMI) {
            _nmiOutput = true;
        }
    }
    
    // Rendering logic for visible scanlines
    if (_scanline >= 0 && _scanline < 240 && _cycle >= 1 && _cycle < 257)
    {
        uint8_t bgPixel = 0x00;
        uint8_t bgPalette = 0x00;
        
        if (_mask.showBg && (_mask.showBgLeft || _cycle >= 9))
        {
            bgPixel = GetBackgroundPixel();
            
            uint16_t bitMux = 0x8000 >> _fineX;
            uint8_t p0Pixel = (_bgShifters.patternLo & bitMux) > 0;
            uint8_t p1Pixel = (_bgShifters.patternHi & bitMux) > 0;
            bgPixel = (p1Pixel << 1) | p0Pixel;
            
            uint8_t bgPal0 = (_bgShifters.attributeLo & bitMux) > 0;
            uint8_t bgPal1 = (_bgShifters.attributeHi & bitMux) > 0;
            bgPalette = (bgPal1 << 1) | bgPal0;
        }
        
        uint8_t fgPixel = 0x00;
        uint8_t fgPalette = 0x00;
        bool fgPriority = false;
        
        if (_mask.showSprites && (_mask.showSpritesLeft || _cycle >= 9))
        {
            fgPixel = GetSpritePixel(fgPriority);
            
            for (uint8_t i = 0; i < _spriteCount; i++)
            {
                if (_spriteScanline[i].x == 0) {
                    uint8_t fgPixelLo = (_spriteShifterPatternLo[i] & 0x80) > 0;
                    uint8_t fgPixelHi = (_spriteShifterPatternHi[i] & 0x80) > 0;
                    fgPixel = (fgPixelHi << 1) | fgPixelLo;
                    
                    fgPalette = (_spriteScanline[i].attribute & 0x03) + 0x04;
                    fgPriority = (_spriteScanline[i].attribute & 0x20) == 0;
                    
                    if (fgPixel != 0)
                    {
                        if (i == 0)
                        {
                            _sprite0Rendering = true;
                        }
                        break;
                    }
                }
            }
        }
        
        // Combine background and foreground
        uint8_t pixel = 0x00;
        uint8_t paletteIndex = 0x00;
        
        if (bgPixel == 0 && fgPixel == 0) {
            pixel = 0x00;
            paletteIndex = 0x00;
        } else if (bgPixel == 0 && fgPixel > 0) {
            pixel = fgPixel;
            paletteIndex = fgPalette;
        } else if (bgPixel > 0 && fgPixel == 0) {
            pixel = bgPixel;
            paletteIndex = bgPalette;
        } else {
            if (fgPriority)
            {
                pixel = fgPixel;
                paletteIndex = fgPalette;
            }
            else
            {
                pixel = bgPixel;
                paletteIndex = bgPalette;
            }
            
            // Sprite 0 hit detection
            if (_sprite0HitPossible && _sprite0Rendering)
            {
                if (_mask.showBg && _mask.showSprites)
                {
                    if (!(_mask.showBgLeft || _mask.showSpritesLeft))
                    {
                        if (_cycle >= 9 && _cycle < 258)
                            _status.sprite0Hit = 1;
                    } else {
                        if (_cycle >= 1 && _cycle < 258)
                            _status.sprite0Hit = 1;
                    }
                }
            }
        }

        uint8_t colorIndex = PPURead(0x3F00 + (paletteIndex << 2) + pixel);
        _screen[(_scanline * 256) + (_cycle - 1)] = colorIndex & 0x3F;
    }
    
    // Advance renderer
    _cycle++;
    if (_cycle >= 341)
    {
        _cycle = 0;
        _scanline++;
        if (_scanline >= 261)
        {
            _scanline = -1;
            _frameComplete = true;
            _frameCount++;
        }
    }
}

void PPU::IncrementScrollX()
{
    if (!_mask.showBg && !_mask.showSprites)
        return;

    if (_vramAddr.coarseX == 31)
    {
        _vramAddr.coarseX = 0;
        _vramAddr.nametableX = ~_vramAddr.nametableX;
    }
    else
    {
        _vramAddr.coarseX++;
    }
}

void PPU::IncrementScrollY()
{
    if (!_mask.showBg && !_mask.showSprites)
        return;

    if (_vramAddr.fineY < 7)
    {
        _vramAddr.fineY++;
    }
    else
    {
        _vramAddr.fineY = 0;

        if (_vramAddr.coarseY == 29)
        {
            _vramAddr.coarseY = 0;
            _vramAddr.nametableY = ~_vramAddr.nametableY;
        }
        else if (_vramAddr.coarseY == 31)
        {
            _vramAddr.coarseY = 0;
        }
        else
        {
            _vramAddr.coarseY++;
        }
    }
}

void PPU::TransferAddressX()
{
    if (!_mask.showBg && !_mask.showSprites)
        return;

    _vramAddr.nametableX = _tramAddr.nametableX;
    _vramAddr.coarseX = _tramAddr.coarseX;
}

void PPU::TransferAddressY()
{
    if (!_mask.showBg && !_mask.showSprites)
        return;
    
    _vramAddr.fineY = _tramAddr.fineY;
    _vramAddr.nametableY = _tramAddr.nametableY;
    _vramAddr.coarseY = _tramAddr.coarseY;
}

void PPU::LoadBackgroundShifters()
{
    _bgShifters.patternLo = (_bgShifters.patternLo & 0xFF00) | _bgNextTileLsb;
    _bgShifters.patternHi = (_bgShifters.patternHi & 0xFF00) | _bgNextTileMsb;

    _bgShifters.attributeLo = (_bgShifters.attributeLo & 0xFF00) |
                              ((_bgNextTileAttrib & 0x01) ? 0xFF : 0x00);
    _bgShifters.attributeHi = (_bgShifters.attributeHi & 0xFF00) |
                              ((_bgNextTileAttrib & 0x02) ? 0xFF : 0x00);
}

void PPU::UpdateShifters()
{
    if (_mask.showBg)
    {
        _bgShifters.patternLo <<= 1;
        _bgShifters.patternHi <<= 1;
        _bgShifters.attributeLo <<= 1;
        _bgShifters.attributeHi <<= 1;
    }

    if (_mask.showSprites && _cycle >= 1 && _cycle < 258)
    {
        for (int i = 0; i < _spriteCount; ++i)
        {
            if (_spriteScanline[i].x > 0)
            {
                _spriteScanline[i].x--;
            }
            else
            {
                _spriteShifterPatternLo[i] <<= 1;
                _spriteShifterPatternHi[i] <<= 1;
            }
        }
    }
}

void PPU::EvaluateSprites()
{
    _spriteCount = 0;
    _sprite0HitPossible = false;
    
    for (uint8_t i = 0; i < _spriteCount; i++)
    {
        _spriteShifterPatternLo[i] = 0;
        _spriteShifterPatternHi[i] = 0;
    }
    
    uint8_t oamEntry = 0;
    _spriteCount = 0;
    
    while (oamEntry < 64 && _spriteCount < 9)
    {
        int16_t diff = ((int16_t)_scanline - (int16_t)_oam[oamEntry * 4]);
        
        if (diff >= 0 && diff < (_ctrl.spriteSize ? 16 : 8) && _spriteCount < 8)
        {
            if (_spriteCount < 8)
            {
                if (oamEntry == 0)
                {
                    _sprite0HitPossible = true;
                }
                
                _spriteScanline[_spriteCount].y = _oam[oamEntry * 4];
                _spriteScanline[_spriteCount].tileId = _oam[oamEntry * 4 + 1];
                _spriteScanline[_spriteCount].attribute = _oam[oamEntry * 4 + 2];
                _spriteScanline[_spriteCount].x = _oam[oamEntry * 4 + 3];
                
                _spriteCount++;
            }
        }
        oamEntry++;
    }
    
    if (_spriteCount > 8)
    {
        _status.spriteOverflow = 1;
        _spriteCount = 8;
    }
}

uint8_t PPU::GetBackgroundPixel()
{
    return 0x00; // Simplified - actual implementation in Clock()
}

uint8_t PPU::GetSpritePixel(bool& spritePriority)
{
    spritePriority = false;
    return 0x00; // Simplified - actual implementation in Clock()
}