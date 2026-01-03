#ifndef BUS_H
#define BUS_H
#include "mem.h"
#include "cartridge.h"
#include "ppu.h"
#include <cstdint>
#include <vector>
#include <array>
#include <memory>

const uint16_t RAM = 0x0000;
const uint16_t RAM_MIRRORS_END = 0x1FFF;
const uint16_t PPU_REGISTERS = 0x2000;
const uint16_t PPU_REGISTERS_MIRRORS_END = 0x3FFF;

class Bus : public Mem {
public:
    std::array<uint8_t, 2048> cpu_vram;  
    std::vector<uint8_t> prg_rom;          
    std::unique_ptr<NesPPU> ppu;            
    explicit Bus(Rom rom);
    uint8_t mem_read(uint16_t addr) const override;
    void mem_write(uint16_t addr, uint8_t data) override;
    
private:
    uint8_t read_prg_rom(uint16_t addr) const;
};

#endif // BUS_H