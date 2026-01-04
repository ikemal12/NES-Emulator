#ifndef PPU_H
#define PPU_H
#include "cartridge.h"
#include "ppu/registers/addr.h"
#include "ppu/registers/control.h"
#include "ppu/registers/status.h"
#include "ppu/registers/mask.h"
#include "ppu/registers/scroll.h"
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
    AddrRegister addr;
    ControlRegister ctrl;
    StatusRegister status;
    MaskRegister mask;
    ScrollRegister scroll;
    uint8_t internal_data_buf;
    uint8_t oam_addr;
    uint16_t cycles;
    uint16_t scanline;
    bool nmi_interrupt;

    NesPPU(std::vector<uint8_t> chr_rom, Mirroring mirroring);
    uint16_t mirror_vram_addr(uint16_t addr) const;
    void increment_vram_addr();
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
    bool tick(uint8_t cycles);
    bool poll_nmi_interrupt();
};

#endif // PPU_H