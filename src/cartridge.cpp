#include "cartridge.h"
#include <cstring>  

Rom::Rom(std::vector<uint8_t> prg, std::vector<uint8_t> chr, 
         uint8_t map, Mirroring mirror)
    : prg_rom(std::move(prg))
    , chr_rom(std::move(chr))
    , mapper(map)
    , screen_mirroring(mirror)
{}

std::optional<Rom> Rom::create(const std::vector<uint8_t>& raw) {
    if (raw.size() < 16) {
        return std::nullopt;  
    }
    if (std::memcmp(&raw[0], NES_TAG, 4) != 0) {
        return std::nullopt; 
    }
    
    uint8_t mapper = (raw[7] & 0b11110000) | (raw[6] >> 4);
    uint8_t ines_ver = (raw[7] >> 2) & 0b11;
    if (ines_ver != 0) {
        return std::nullopt;  
    }
    
    bool four_screen = (raw[6] & 0b00001000) != 0;
    bool vertical_mirroring = (raw[6] & 0b00000001) != 0;
    Mirroring screen_mirroring;
    if (four_screen) {
        screen_mirroring = Mirroring::FourScreen;
    } else if (vertical_mirroring) {
        screen_mirroring = Mirroring::Vertical;
    } else {
        screen_mirroring = Mirroring::Horizontal;
    }
    
    size_t prg_rom_size = raw[4] * PRG_ROM_PAGE_SIZE;
    size_t chr_rom_size = raw[5] * CHR_ROM_PAGE_SIZE; 
    bool skip_trainer = (raw[6] & 0b00000100) != 0;
    size_t prg_rom_start = 16 + (skip_trainer ? 512 : 0);
    size_t chr_rom_start = prg_rom_start + prg_rom_size;
    
    if (raw.size() < chr_rom_start + chr_rom_size) {
        return std::nullopt; 
    }
    std::vector<uint8_t> prg_rom(
        raw.begin() + prg_rom_start,
        raw.begin() + prg_rom_start + prg_rom_size
    );
    
    std::vector<uint8_t> chr_rom(
        raw.begin() + chr_rom_start,
        raw.begin() + chr_rom_start + chr_rom_size
    );

    return Rom(
        std::move(prg_rom),
        std::move(chr_rom),
        mapper,
        screen_mirroring
    );
}