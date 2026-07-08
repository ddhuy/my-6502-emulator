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
    void CloseGamepad(int deviceIndex);

    // Read keyboard + gamepad for one player (0 or 1) and pack it
    // into an NES controller byte.
    uint8_t GetControllerState(int deviceIndex) const;

    static constexpr int NES_WIDTH = 256;
    static constexpr int NES_HEIGHT = 240;
    static constexpr int MAX_PLAYERS = 2;

private:
    SDL_Window* _window;
    SDL_Renderer* _renderer;
    SDL_Texture* _texture;

    // One gamepad slot per player; slot 0 = player 1, slot 1 = player 2
    std::array<SDL_GameController*, MAX_PLAYERS> _gamepads;

    const char* _title;
    int _windowWidth;
    int _windowHeight;
    int _scale;
    bool _running;

    // NES Color Palette (64 colors)
    static const std::array<uint32_t, 64> _nesPalette;
};

#endif // DISPLAY_H