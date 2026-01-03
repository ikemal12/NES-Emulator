#include "ppu.h"
#include <stdexcept>

NesPPU::NesPPU(std::vector<uint8_t> chr_rom, Mirroring mirroring)
    : chr_rom(std::move(chr_rom))
    , mirroring(mirroring)
    , vram{}
    , oam_data{}
    , palette_table{}
    , addr()
    , ctrl()
    , internal_data_buf(0)
{}

uint16_t NesPPU::mirror_vram_addr(uint16_t addr) const {
    uint16_t mirrored_vram = addr & 0b10111111111111; 
    uint16_t vram_index = mirrored_vram - 0x2000;   
    uint16_t name_table = vram_index / 0x400;     

    if (mirroring == Mirroring::Vertical) {
        if (name_table == 2 || name_table == 3) {
            return vram_index - 0x800;
        }
    } else if (mirroring == Mirroring::Horizontal) {
        if (name_table == 1) {
            return vram_index - 0x400;
        } else if (name_table == 2) {
            return vram_index - 0x400;
        } else if (name_table == 3) {
            return vram_index - 0x800;
        }
    }
    return vram_index;
}

void NesPPU::increment_vram_addr() {
    addr.increment(ctrl.vram_addr_increment());
}

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