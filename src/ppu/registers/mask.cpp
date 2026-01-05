#include "ppu/registers/mask.h"

MaskRegister::MaskRegister() 
    : bits(0b00000000)
{}

bool MaskRegister::contains(uint8_t flag) const {
    return (bits & flag) != 0;
}

void MaskRegister::is_grayscale(bool status) {
    if (status) {
        bits |= GREYSCALE;
    } else {
        bits &= ~GREYSCALE;
    }
}

void MaskRegister::leftmost_8pxl_background(bool status) {
    if (status) {
        bits |= LEFTMOST_8PXL_BACKGROUND;
    } else {
        bits &= ~LEFTMOST_8PXL_BACKGROUND;
    }
}

void MaskRegister::leftmost_8pxl_sprite(bool status) {
    if (status) {
        bits |= LEFTMOST_8PXL_SPRITE;
    } else {
        bits &= ~LEFTMOST_8PXL_SPRITE;
    }
}

void MaskRegister::show_background(bool status) {
    if (status) {
        bits |= SHOW_BACKGROUND;
    } else {
        bits &= ~SHOW_BACKGROUND;
    }
}

void MaskRegister::show_sprites(bool status) {
    if (status) {
        bits |= SHOW_SPRITES;
    } else {
        bits &= ~SHOW_SPRITES;
    }
}

std::vector<Colour> MaskRegister::emphasise() {
    std::vector<Colour> colours;
    if (bits & EMPHASISE_RED) {
        colours.push_back(RED);
    }
    if (bits & EMPHASISE_GREEN) {
        colours.push_back(GREEN);
    }
    if (bits & EMPHASISE_BLUE) {
        colours.push_back(BLUE);
    }
    return colours;
}

void MaskRegister::update(uint8_t data) {
    bits = data;
}
