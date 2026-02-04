#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <array>
#include <cstdint>


class Display
{
public:
    Display(const char* title, int width, int height, int scale);
    ~Display();

    bool Init();
    void Clear();
    void Render(const uint8_t* screenBuffer);
    void Present();
    bool IsRunning() const { return _running; }
    void HandlerEvents();

    static constexpr int NES_WIDTH = 256;
    static constexpr int NES_HEIGHT = 240;

private:
    SDL_Window* _window;
    SDL_Renderer* _renderer;
    SDL_Texture* _texture;

    const char* _title;
    int _windowWidth;
    int _windowHeight;
    int _scale;
    bool _running;

    // NES Color Palette (64 colors)
    static const std::array<uint32_t, 64> _nesPalette;
};

#endif // DISPLAY_H