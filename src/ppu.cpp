#include "ppu.h"

NesPPU::NesPPU(std::vector<uint8_t> chr_rom, Mirroring mirroring)
    : chr_rom(std::move(chr_rom))
    , mirroring(mirroring)
    , vram{}
    , oam_data{}
    , palette_table{}
{}

uint8_t NesPPU::read_status() {
    return 0;
}

uint8_t NesPPU::read_oam_data() {
    return 0;
}

uint8_t NesPPU::read_data() {
    return 0;
}

void NesPPU::write_to_ctrl(uint8_t value) {}
void NesPPU::write_to_mask(uint8_t value) {}
void NesPPU::write_to_oam_addr(uint8_t value) {}
void NesPPU::write_to_oam_data(uint8_t value) {}
void NesPPU::write_to_scroll(uint8_t value) {}
void NesPPU::write_to_ppu_addr(uint8_t value) {}
void NesPPU::write_to_data(uint8_t value) {}