#include "render/frame.h"

Frame::Frame() 
    : data{}  
{}

void Frame::set_pixel(size_t x, size_t y, uint8_t r, uint8_t g, uint8_t b) {
    if (x >= WIDTH || y >= HEIGHT) {
        return;
    }
    size_t base = y * 3 * WIDTH + x * 3;
    data[base] = r;
    data[base + 1] = g;
    data[base + 2] = b;
}

void Frame::set_pixel(size_t x, size_t y, const std::array<uint8_t, 3>& rgb) {
    set_pixel(x, y, rgb[0], rgb[1], rgb[2]);
}
