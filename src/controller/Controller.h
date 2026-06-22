#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <cstdint>

class Controller
{
public:
    enum Button
    {
        A      = 0x80,
        B      = 0x40,
        SELECT = 0x20,
        START  = 0x10,
        UP     = 0x08,
        DOWN   = 0x04,
        LEFT   = 0x02,
        RIGHT  = 0x01
    };

    // Live button state, refreshed each frame by the input layer
    void SetButtons(uint8_t state);

    // $4016 write: strobe signal to latch button states
    void Write(uint8_t data);

    // $4016/$4017 read: shift out button states
    uint8_t Read();


private:
    uint8_t _buttons = 0x00; // live state from the keyboard
    uint8_t _shift   = 0x00; // shift register
    bool    _strobe = false; // strobe state
};

#endif // CONTROLLER_H