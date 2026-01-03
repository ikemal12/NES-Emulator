#include "ppu/registers/addr.h"

AddrRegister::AddrRegister() 
    : value(0, 0)
    , hi_ptr(true)
{}

void AddrRegister::set(uint16_t data) {
    value.first = (data >> 8) & 0xFF;  
    value.second = data & 0xFF;       
}

void AddrRegister::update(uint8_t data) {
    if (hi_ptr) {
        value.first = data;
    } else {
        value.second = data;
    }
    if (get() > 0x3FFF) {
        set(get() & 0b11111111111111);  
    }
    hi_ptr = !hi_ptr;  
}

void AddrRegister::increment(uint8_t inc) {
    uint8_t lo = value.second;
    value.second = value.second + inc;  
    if (lo > value.second) {
        value.first = value.first + 1;  
    }
    if (get() > 0x3FFF) {
        set(get() & 0b11111111111111);
    }
}

void AddrRegister::reset_latch() {
    hi_ptr = true;
}

uint16_t AddrRegister::get() const {
    return (static_cast<uint16_t>(value.first) << 8) | value.second;
}