#include "ppu/registers/scroll.h"

ScrollRegister::ScrollRegister() 
    : scroll_x(0)
    , scroll_y(0)
    , latch(true)
{}

void ScrollRegister::write(uint8_t data) {
    if (latch) {
        scroll_x = data;
        latch = false;
    } else {
        scroll_y = data;
        latch = true;
    }
}

void ScrollRegister::reset_latch() {
    latch = true;
}