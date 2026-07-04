#ifndef DISPLAY_H
#define DISPLAY_H

#include <array>
#include <cstdint>

#include <SDL2/SDL.h>


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
    void HandleEvents();
    void Shutdown();

    void OpenGamepad(int deviceIndex);
    void CloseGamepad();

    // Read the keyboard and pack it into an NES controller byte.
    uint8_t GetController1State() const;

    static constexpr int NES_WIDTH = 256;
    static constexpr int NES_HEIGHT = 240;

private:
    SDL_Window* _window;
    SDL_Renderer* _renderer;
    SDL_Texture* _texture;
    SDL_GameController* _controller1;

    const char* _title;
    int _windowWidth;
    int _windowHeight;
    int _scale;
    bool _running;

    // NES Color Palette (64 colors)
    static const std::array<uint32_t, 64> _nesPalette;
};

#endif // DISPLAY_H