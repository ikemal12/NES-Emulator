#ifndef PPU_H
#define PPU_H
#include "cartridge.h"
#include <cstdint>
#include <vector>
#include <array>

class NesPPU {
public:
    std::vector<uint8_t> chr_rom;
    std::array<uint8_t, 2048> vram;
    std::array<uint8_t, 256> oam_data;
    std::array<uint8_t, 32> palette_table;
    Mirroring mirroring;
    NesPPU(std::vector<uint8_t> chr_rom, Mirroring mirroring);
    
    uint8_t read_status();
    uint8_t read_oam_data();
    uint8_t read_data();
    void write_to_ctrl(uint8_t value);
    void write_to_mask(uint8_t value);
    void write_to_oam_addr(uint8_t value);
    void write_to_oam_data(uint8_t value);
    void write_to_scroll(uint8_t value);
    void write_to_ppu_addr(uint8_t value);
    void write_to_data(uint8_t value);
};

#endif // PPU_H