#ifndef CONTROL_REGISTER_H
#define CONTROL_REGISTER_H
#include <cstdint>

class ControlRegister {
public:
    uint8_t bits;
    static const uint8_t NAMETABLE1             = 0b00000001;
    static const uint8_t NAMETABLE2             = 0b00000010;
    static const uint8_t VRAM_ADD_INCREMENT     = 0b00000100;
    static const uint8_t SPRITE_PATTERN_ADDR    = 0b00001000;
    static const uint8_t BACKGROUND_PATTERN_ADDR = 0b00010000;
    static const uint8_t SPRITE_SIZE            = 0b00100000;
    static const uint8_t MASTER_SLAVE_SELECT    = 0b01000000;
    static const uint8_t GENERATE_NMI           = 0b10000000;
    
    ControlRegister();
    bool contains(uint8_t flag) const;
    uint8_t vram_addr_increment() const;
    uint16_t background_pattern_addr() const;
    uint16_t sprite_pattern_addr() const;
    void update(uint8_t data);
};

#endif // CONTROL_REGISTER_H