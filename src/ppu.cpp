#include "ppu.h"
#include <stdexcept>
#include <iostream>

NesPPU::NesPPU(std::vector<uint8_t> chr_rom_data, Mirroring mirroring)
    : chr_rom(chr_rom_data.empty() ? std::vector<uint8_t>(8192, 0) : std::move(chr_rom_data))
    , vram{}
    , oam_data{}
    , palette_table{}
    , mirroring(mirroring)
    , addr()
    , ctrl()
    , status()
    , mask()
    , scroll()
    , internal_data_buf(0)
    , oam_addr(0)
    , cycles(0)
    , scanline(0)
    , nmi_interrupt(false)
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
    uint8_t data = status.snapshot();
    addr.reset_latch();
    scroll.reset_latch();
    return data;
}

uint8_t NesPPU::read_oam_data() {
    return oam_data[oam_addr];
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
        uint8_t result = internal_data_buf;
        uint16_t mirrored = address - 0x1000; 
        internal_data_buf = vram[mirror_vram_addr(mirrored)];
        return result;
    } else if (address >= 0x3F00 && address <= 0x3FFF) {
        return palette_table[(address - 0x3F00) % 32];
    } else {
        throw std::runtime_error("unexpected access to mirrored space " + std::to_string(address));
    }
}

void NesPPU::write_to_ctrl(uint8_t value) {
    bool nmi_before = ctrl.contains(ControlRegister::GENERATE_NMI);
    ctrl.update(value);
    bool nmi_after = ctrl.contains(ControlRegister::GENERATE_NMI);
    if (!nmi_before && nmi_after && status.is_in_vblank()) {
        nmi_interrupt = true;
    }
}

void NesPPU::write_to_mask(uint8_t value) {
    mask.update(value);
}

void NesPPU::write_to_oam_addr(uint8_t value) {
    oam_addr = value;
}

void NesPPU::write_to_oam_data(uint8_t value) {
    oam_data[oam_addr] = value;
    oam_addr = oam_addr + 1;
}

void NesPPU::write_to_scroll(uint8_t value) {
    scroll.write(value);
}

void NesPPU::write_to_ppu_addr(uint8_t value) {
    addr.update(value);
}

void NesPPU::write_to_data(uint8_t value) {
    uint16_t address = addr.get();
    if (address >= 0 && address <= 0x1FFF) {
        chr_rom[address] = value;
    } else if (address >= 0x2000 && address <= 0x2FFF) {
        vram[mirror_vram_addr(address)] = value;
    } else if (address >= 0x3000 && address <= 0x3EFF) {
        uint16_t mirrored = address - 0x1000; 
        vram[mirror_vram_addr(mirrored)] = value;
    } else if (address >= 0x3F00 && address <= 0x3FFF) {
        palette_table[(address - 0x3F00) % 32] = value;
    } else {
        throw std::runtime_error("unexpected access to mirrored space " + std::to_string(address));
    }
    increment_vram_addr();
}

bool NesPPU::tick(uint8_t cycles) {
    bool vblank_entered = false;
    this->cycles += cycles;
    if (this->cycles >= 341) {
        this->cycles -= 341;
        scanline += 1;

        if (scanline == 241) {
            static bool first_vblank = true;
            if (first_vblank) {
                std::cout << "PPU: Reached scanline 241 (VBlank), CTRL NMI bit: " 
                          << (ctrl.contains(ControlRegister::GENERATE_NMI) ? "ON" : "OFF") << std::endl;
                first_vblank = false;
            }
            status.set_vblank_status(true);
            if (ctrl.contains(ControlRegister::GENERATE_NMI)) {
                nmi_interrupt = true;
            }
            vblank_entered = true;
        }
        if (scanline >= 262) {
            scanline = 0;
            status.set_vblank_status(false);
            status.set_sprite_zero_hit(false);
            nmi_interrupt = false;
        }
    }
    bool show_background = mask.contains(MaskRegister::SHOW_BACKGROUND);
    bool show_sprites = mask.contains(MaskRegister::SHOW_SPRITES);
    if (show_background && show_sprites) {
        uint8_t sprite_0_y = oam_data[0];
        uint8_t sprite_0_x = oam_data[3];
        uint8_t sprite_height = ctrl.sprite_size(); 
        if (scanline >= sprite_0_y + 1 && scanline <= sprite_0_y + sprite_height) {
             if (this->cycles >= sprite_0_x && this->cycles < sprite_0_x + 8 + 20) { 
                 status.set_sprite_zero_hit(true);
             }
        }
    }
    return vblank_entered;
}

bool NesPPU::poll_nmi_interrupt() {
    bool result = nmi_interrupt;
    nmi_interrupt = false;
    return result;
}