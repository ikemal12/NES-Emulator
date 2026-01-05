#include "bus.h"
#include <iostream>
#include <stdexcept>

Bus::Bus(Rom rom, std::function<void(const NesPPU&)> callback)
    : cpu_vram{}
    , prg_rom(std::move(rom.prg_rom))
    , ppu(std::make_unique<NesPPU>(std::move(rom.chr_rom), rom.screen_mirroring))
    , gameloop_callback(std::move(callback))
{}

uint8_t Bus::read_prg_rom(uint16_t addr) const {
    addr -= 0x8000;
    if (prg_rom.size() == 0x4000 && addr >= 0x4000) {
        addr = addr % 0x4000;
    }
    return prg_rom[addr];
}

uint8_t Bus::mem_read(uint16_t addr) const {
    if (addr >= RAM && addr <= RAM_MIRRORS_END) {
        uint16_t mirror_down_addr = addr & 0x07FF;
        return cpu_vram[mirror_down_addr];

    } else if (addr == 0x2000 || addr == 0x2001 || addr == 0x2003 || 
               addr == 0x2005 || addr == 0x2006 || addr == 0x4014) {
        // std::cout << "Warning: Read from write-only PPU address 0x" << std::hex << addr << std::dec << std::endl;
        return 0;
        
    } else if (addr == 0x2002) {
        return ppu->read_status();
        
    } else if (addr == 0x2004) {
        return ppu->read_oam_data();
        
    } else if (addr == 0x2007) {
        return ppu->read_data();
        
    } else if (addr >= 0x2008 && addr <= PPU_REGISTERS_MIRRORS_END) {
        uint16_t mirror_down_addr = addr & 0x2007;
        return mem_read(mirror_down_addr);
        
    } else if (addr >= 0x8000 && addr <= 0xFFFF) {
        return read_prg_rom(addr);
        
    } else {
        return 0;
    }
}

void Bus::mem_write(uint16_t addr, uint8_t data) {
    if (addr >= RAM && addr <= RAM_MIRRORS_END) {
        uint16_t mirror_down_addr = addr & 0x07FF;
        cpu_vram[mirror_down_addr] = data;
        
    } else if (addr == 0x2000) {
        ppu->write_to_ctrl(data);
        
    } else if (addr == 0x2001) {
        ppu->write_to_mask(data);
        
    } else if (addr == 0x2002) {
        throw std::runtime_error("Attempt to write to PPU status register");
        
    } else if (addr == 0x2003) {
        ppu->write_to_oam_addr(data);
        
    } else if (addr == 0x2004) {
        ppu->write_to_oam_data(data);
        
    } else if (addr == 0x2005) {
        ppu->write_to_scroll(data);
        
    } else if (addr == 0x2006) {
        ppu->write_to_ppu_addr(data);
        
    } else if (addr == 0x2007) {
        ppu->write_to_data(data);
        
    } else if (addr >= 0x2008 && addr <= PPU_REGISTERS_MIRRORS_END) {
        uint16_t mirror_down_addr = addr & 0x2007;
        mem_write(mirror_down_addr, data);
        
    } else if (addr >= 0x8000 && addr <= 0xFFFF) {
        throw std::runtime_error("Attempt to write to Cartridge ROM space");
    } 
}

void Bus::tick(uint8_t cpu_cycles) {
    bool nmi_before = ppu->nmi_interrupt;
    ppu->tick(cpu_cycles * 3);
    bool nmi_after = ppu->nmi_interrupt;
    if (!nmi_before && nmi_after) {
        gameloop_callback(*ppu);
    }
}