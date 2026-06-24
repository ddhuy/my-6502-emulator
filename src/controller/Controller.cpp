#include <iostream>

#include "Controller.h"
#include "utils/Logger.h"


void Controller::SetButtons(uint8_t state)
{
    _buttons = state;
}

void Controller::Write(uint8_t data)
{
    _strobe = (data & 0x01);
    if (_strobe)
    {
        _shift = _buttons; // while strob is high, register tracks live state
    }
}


uint8_t Controller::Read()
{
    // While strobe is high, hardware continuously reloads -> always return A
    if (_strobe)
        _shift = _buttons;

    uint8_t bit = (_shift & 0x80) > 0 ? 1 : 0; // Return A button state (bit 7)
    _shift <<= 1; // Shift left to prepare next button for reading
    return bit;
}
