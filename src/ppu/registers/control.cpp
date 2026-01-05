#include "ppu/registers/control.h"

ControlRegister::ControlRegister() 
    : bits(0b00000000)
{}

bool ControlRegister::contains(uint8_t flag) const {
    return (bits & flag) != 0;
}

uint8_t ControlRegister::vram_addr_increment() const {
    if (!contains(VRAM_ADD_INCREMENT)) {
        return 1;
    } else {
        return 32;
    }
}

uint16_t ControlRegister::background_pattern_addr() const {
    if (!contains(BACKGROUND_PATTERN_ADDR)) {
        return 0x0000;
    } else {
        return 0x1000;
    }
}

uint16_t ControlRegister::sprite_pattern_addr() const {
    if (!contains(SPRITE_PATTERN_ADDR)) {
        return 0x0000;
    } else {
        return 0x1000;
    }
}

void ControlRegister::update(uint8_t data) {
    bits = data;
}