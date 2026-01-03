#include "ppu.h"
#include <stdexcept>

NesPPU::NesPPU(std::vector<uint8_t> chr_rom, Mirroring mirroring)
    : chr_rom(std::move(chr_rom))
    , vram{}
    , oam_data{}
    , palette_table{}
    , mirroring(mirroring)
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
    uint16_t address = addr.get();
    increment_vram_addr();

    if (address >= 0 && address <= 0x1FFF) {
        uint8_t result = internal_data_buf;
        internal_data_buf = chr_rom[address];
        return result;
    } else if (address >= 0x2000 && address <= 0x2FFF) {
        uint8_t result = internal_data_buf;
        internal_data_buf = vram[mirror_vram_addr(address)];
        return result;
    } else if (address >= 0x3000 && address <= 0x3EFF) {
        throw std::runtime_error("addr space 0x3000..0x3EFF is not expected to be used, requested = " + std::to_string(address));
    } else if (address >= 0x3F00 && address <= 0x3FFF) {
        return palette_table[(address - 0x3F00) % 32];
    } else {
        throw std::runtime_error("unexpected access to mirrored space " + std::to_string(address));
    }
}

void NesPPU::write_to_ctrl(uint8_t value) {
    ctrl.update(value);
}
void NesPPU::write_to_mask(uint8_t value) {}
void NesPPU::write_to_oam_addr(uint8_t value) {}
void NesPPU::write_to_oam_data(uint8_t value) {}
void NesPPU::write_to_scroll(uint8_t value) {}
void NesPPU::write_to_ppu_addr(uint8_t value) {
    addr.update(value);
}
void NesPPU::write_to_data(uint8_t value) {}