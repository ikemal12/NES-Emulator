#ifndef MASK_REGISTER_H
#define MASK_REGISTER_H
#include <cstdint>
#include <vector>

enum Colour {
    RED,
    GREEN,
    BLUE
};

class MaskRegister {
public:
    uint8_t bits;
    static const uint8_t GREYSCALE                 = 0b00000001;
    static const uint8_t LEFTMOST_8PXL_BACKGROUND  = 0b00000010;
    static const uint8_t LEFTMOST_8PXL_SPRITE      = 0b00000100;
    static const uint8_t SHOW_BACKGROUND           = 0b00001000;
    static const uint8_t SHOW_SPRITES              = 0b00010000;
    static const uint8_t EMPHASISE_RED             = 0b00100000;
    static const uint8_t EMPHASISE_GREEN           = 0b01000000;
    static const uint8_t EMPHASISE_BLUE            = 0b10000000;

    MaskRegister();
    bool contains(uint8_t flag) const;
    void is_grayscale(bool status);
    void leftmost_8pxl_background(bool status);
    void leftmost_8pxl_sprite(bool status);
    void show_background(bool status);
    void show_sprites(bool status);
    std::vector<Colour> emphasise();
    void update(uint8_t data);
};

#endif // MASK_REGISTER_H