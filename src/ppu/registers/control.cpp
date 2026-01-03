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

void ControlRegister::update(uint8_t data) {
    bits = data;
}