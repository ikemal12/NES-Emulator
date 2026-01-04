#ifndef STATUS_REGISTER_H
#define STATUS_REGISTER_H
#include <cstdint>

class StatusRegister {
public:
    uint8_t bits;
    static const uint8_t NOTUSED          = 0b00000001;
    static const uint8_t NOTUSED2         = 0b00000010;
    static const uint8_t NOTUSED3         = 0b00000100;
    static const uint8_t NOTUSED4         = 0b00001000;
    static const uint8_t NOTUSED5         = 0b00010000;
    static const uint8_t SPRITE_OVERFLOW  = 0b00100000;
    static const uint8_t SPRITE_ZERO_HIT  = 0b01000000;
    static const uint8_t VBLANK_STARTED   = 0b10000000;

    StatusRegister();
    void set_vblank_status(bool status);
    void set_sprite_zero_hit(bool status);
    void set_sprite_overflow(bool status);
    void reset_vblank_status();
    bool is_in_vblank() const;
    uint8_t snapshot();
};

#endif // STATUS_REGISTER_H