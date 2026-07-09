#include <iostream>

#include "Display.h"
#include "controller/Controller.h"
#include "utils/Logger.h"


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
    _gamepads.fill(nullptr);
}

Display::~Display()
{
    for (auto& controller : _gamepads)
    {
        if (controller)
        {
            SDL_GameControllerClose(controller);
            controller = nullptr;
        }
    }
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
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)
    {
        LOG_ERROR("SDL initialization failed: %s", SDL_GetError());
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
        LOG_ERROR("Window creation failed: %s", SDL_GetError());
        return false;
    }

    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!_renderer)
    {
        LOG_ERROR("Renderer creation failed: %s", SDL_GetError());
        return false;
    }

    _texture = SDL_CreateTexture(_renderer,
                                 SDL_PIXELFORMAT_ARGB8888,
                                 SDL_TEXTUREACCESS_STREAMING,
                                 NES_WIDTH,
                                 NES_HEIGHT);
    if (!_texture)
    {
        LOG_ERROR("Texture creation failed: %s", SDL_GetError());
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
        LOG_ERROR("Failed to lock texture: %s", SDL_GetError());
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

            case SDL_CONTROLLERDEVICEADDED:
                OpenGamepad(event.cdevice.which);
                break;

            case SDL_CONTROLLERDEVICEREMOVED:
                CloseGamepad(event.cdevice.which);
                break;

            case SDL_CONTROLLERBUTTONDOWN:
                LOG_DEBUG("Controller button pressed: %d", event.cbutton.button);
                break;

            case SDL_JOYAXISMOTION:
                LOG_DEBUG("Controller axis motion: axis=%d value=%d", event.jaxis.axis, event.jaxis.value);
                break;

            case SDL_JOYHATMOTION:
                LOG_DEBUG("Controller hat motion: hat=%d value=%d", event.jhat.hat, event.jhat.value);
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

uint8_t Display::GetControllerState(int deviceIndex) const
{
    if (deviceIndex < 0 || deviceIndex >= MAX_PLAYERS)
    {
        LOG_WARN("Invalid controller index: %d", deviceIndex);
        return 0x00;
    }

    // Keyboard mapping per player, indexed to match the button masks below
    // Player 1: X(A), C(B), RSHIFT(SELECT), RETURN(START), ARROWS(DPAD)
    // Player 2: J(A), K(B), U(SELECT), I(START), WASD(DPAD)
    static const uint8_t buttonMasks[8] = {
        Controller::Button::A,
        Controller::Button::B,
        Controller::Button::SELECT,
        Controller::Button::START,
        Controller::Button::UP,
        Controller::Button::DOWN,
        Controller::Button::LEFT,
        Controller::Button::RIGHT
    };

    static const SDL_Scancode keymap[MAX_PLAYERS][8] = {
        {
            SDL_SCANCODE_X,      // A
            SDL_SCANCODE_C,      // B
            SDL_SCANCODE_RSHIFT, // SELECT
            SDL_SCANCODE_RETURN, // START
            SDL_SCANCODE_UP,     // UP
            SDL_SCANCODE_DOWN,   // DOWN
            SDL_SCANCODE_LEFT,   // LEFT
            SDL_SCANCODE_RIGHT   // RIGHT
        },
        { 
            SDL_SCANCODE_J,      // A
            SDL_SCANCODE_K,      // B
            SDL_SCANCODE_U,      // SELECT
            SDL_SCANCODE_I,      // START
            SDL_SCANCODE_W,      // UP
            SDL_SCANCODE_S,      // DOWN
            SDL_SCANCODE_A,      // LEFT
            SDL_SCANCODE_D       // RIGHT
        }
    };

    uint8_t state = 0x00;
    const uint8_t* keys = SDL_GetKeyboardState(nullptr);

    for (int i = 0; i < 8; ++i)
    {
        if (keys[keymap[deviceIndex][i]])
        {
            state |= buttonMasks[i];
        }
    }

    SDL_GameController* controller = _gamepads[deviceIndex];
    if (controller)
    {
        // Map SDL controller buttons to NES controller buttons
        auto btn = [controller](SDL_GameControllerButton b) {
            return SDL_GameControllerGetButton(controller, b) != 0;
        };
    
        if (btn(SDL_CONTROLLER_BUTTON_A) || btn(SDL_CONTROLLER_BUTTON_B))
            state |= Controller::Button::A;
        if (btn(SDL_CONTROLLER_BUTTON_X) || btn(SDL_CONTROLLER_BUTTON_Y))
            state |= Controller::Button::B;
        if (btn(SDL_CONTROLLER_BUTTON_BACK))       state |= Controller::Button::SELECT;
        if (btn(SDL_CONTROLLER_BUTTON_START))      state |= Controller::Button::START;
        if (btn(SDL_CONTROLLER_BUTTON_DPAD_UP))    state |= Controller::Button::UP;
        if (btn(SDL_CONTROLLER_BUTTON_DPAD_DOWN))  state |= Controller::Button::DOWN;
        if (btn(SDL_CONTROLLER_BUTTON_DPAD_LEFT))  state |= Controller::Button::LEFT;
        if (btn(SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) state |= Controller::Button::RIGHT;

        // Left stick as d-pad fallback
        const int16_t DEADZONE = 8000;

        int16_t lx = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
        int16_t ly = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);

        if (lx < -DEADZONE) state |= Controller::Button::LEFT;
        if (lx >  DEADZONE) state |= Controller::Button::RIGHT;
        if (ly < -DEADZONE) state |= Controller::Button::UP;
        if (ly >  DEADZONE) state |= Controller::Button::DOWN;
    }

    return state;
}

void Display::OpenGamepad(int deviceIndex)
{
    if (!SDL_IsGameController(deviceIndex))
    {
        LOG_WARN("Device at index %d is not a game controller", deviceIndex);
        return;
    }

    // Assign the new device to the first available slot
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        if (_gamepads[i])
            continue;

        _gamepads[i] = SDL_GameControllerOpen(deviceIndex);
        if (_gamepads[i])
        {
            LOG_INFO("Gamepad %d connected: %s", i, SDL_GameControllerName(_gamepads[i]));
        }
        else
        {
            LOG_ERROR("Failed to open game controller at index %d: %s", deviceIndex, SDL_GetError());
        }
        return;
    }

    LOG_WARN("Ignoring gamepad at index %d: all player slots are in use", deviceIndex);
}

void Display::CloseGamepad(int deviceIndex)
{
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        SDL_GameController* controller = _gamepads[i];
        if (controller && SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller)) == deviceIndex)
        {
            LOG_INFO("Gamepad %d disconnected: %s", i, SDL_GameControllerName(controller));
            SDL_GameControllerClose(controller);
            _gamepads[i] = nullptr;
            return;
        }
    }
}