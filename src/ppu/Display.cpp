#include <iostream>

#include "Display.h"


// NES Color Palette (RGB values for all 64 colors)
const std::array<uint32_t, 64> Display::_nesPalette = {
    0xFF545454, 0xFF001E74, 0xFF081090, 0xFF300088, 0xFF440064, 0xFF5C0030, 0xFF540400, 0xFF3C1800,
    0xFF202A00, 0xFF083A00, 0xFF004000, 0xFF003C00, 0xFF00323C, 0xFF000000, 0xFF000000, 0xFF000000,
    0xFF989698, 0xFF084CC4, 0xFF3032EC, 0xFF5C1EE4, 0xFF8814B0, 0xFFA01464, 0xFF982220, 0xFF783C00,
    0xFF545A00, 0xFF287200, 0xFF087C00, 0xFF007628, 0xFF006678, 0xFF000000, 0xFF000000, 0xFF000000,
    0xFFECEEEC, 0xFF4C9AEC, 0xFF787CEC, 0xFFB062EC, 0xFFE454EC, 0xFFEC58B4, 0xFFEC6A64, 0xFFD48820,
    0xFFA0AA00, 0xFF74C400, 0xFF4CD020, 0xFF38CC6C, 0xFF38B4CC, 0xFF3C3C3C, 0xFF000000, 0xFF000000,
    0xFFECEEEC, 0xFFA8CCEC, 0xFFBCBCEC, 0xFFD4B2EC, 0xFFECAEEC, 0xFFECAED4, 0xFFECB4B0, 0xFFE4C490,
    0xFFCCD278, 0xFFB4DE78, 0xFFA8E290, 0xFF98E2B4, 0xFFA0D6E4, 0xFFA0A2A0, 0xFF000000, 0xFF000000
};

Display::Display(const char* title, int width, int height, int scale)
{
    _title = title;
    _window = nullptr;
    _renderer = nullptr;
    _texture = nullptr;
    _windowWidth = width * scale;
    _windowHeight = height * scale;
    _scale = scale;
    _running = false;
}

Display::~Display()
{
    if (_texture)
        SDL_DestroyTexture(_texture);
    if (_renderer)
        SDL_DestroyRenderer(_renderer);
    if (_window)
        SDL_DestroyWindow(_window);
    SDL_Quit();
}

bool Display::Init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }

    _window = SDL_CreateWindow(_title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        _windowWidth,
        _windowHeight,
        SDL_WINDOW_SHOWN);
    if (!_window)
    {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!_renderer)
    {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    _texture = SDL_CreateTexture(_renderer,
                                 SDL_PIXELFORMAT_ARGB8888,
                                 SDL_TEXTUREACCESS_STREAMING,
                                 NES_WIDTH,
                                 NES_HEIGHT);
    if (!_texture)
    {
        std::cerr << "Texter creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    _running = true;
    return true;
}

void Display::Clear()
{
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
    SDL_RenderClear(_renderer);
}

void Display::Render(const uint8_t* screenBuffer)
{
    uint32_t* pixels;
    int pitch;

    if (SDL_LockTexture(_texture, nullptr, (void**) &pixels, &pitch) < 0)
    {
        std::cerr << "Failed to lock texture: " << SDL_GetError() << std::endl;
        return;
    }

    // Convert NES palette indices to RGB
    for (int y = 0; y < NES_HEIGHT; ++y)
    {
        for (int x = 0; x < NES_WIDTH; ++x)
        {
            uint8_t paletteIndex = screenBuffer[y * NES_WIDTH + x] & 0x3F;
            pixels[y * (pitch / 4) + x] = _nesPalette[paletteIndex];
        }
    }

    SDL_UnlockTexture(_texture);
    SDL_RenderCopy(_renderer, _texture, nullptr, nullptr);
}

void Display::Present()
{
    SDL_RenderPresent(_renderer);
}

void Display::HandleEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                _running = false;
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    _running = false;
                break;
        }
    }
}

void Display::Shutdown()
{
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
    SDL_Quit();
}