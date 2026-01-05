#include "joypad.h"
#include <cstdint>

Joypad::Joypad()
    : strobe(false)
    , button_index(0)
    , button_status(0)
{}

void Joypad::write(uint8_t data) {
    strobe = (data & 1) != 0;
    if (strobe) {
        button_index = 0;
    }
}

uint8_t Joypad::read() {
    uint8_t value = (button_status & (1 << button_index)) ? 1 : 0;
    if (!strobe && button_index < 8) {
        button_index++;
    }
    return value;
}

void Joypad::set_button_status(JoypadButton button, bool pressed) {
    if (pressed) {
        button_status |= button;
    } else {
        button_status &= ~button;
    }
}