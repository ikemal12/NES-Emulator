#ifndef CARTRIDGE_H
#define CARTRIDGE_H
#include <cstdint>
#include <vector>
#include <string>
#include <stdexcept>

const uint8_t NES_TAG[4] = {0x4E, 0x45, 0x53, 0x1A};
const size_t PRG_ROM_PAGE_SIZE = 16384;
const size_t CHR_ROM_PAGE_SIZE = 8192;

enum class Mirroring {
    Vertical,
    Horizontal,
    FourScreen
};

class RomParseError: public std::runtime_error {
public:
    explicit RomParseError(const std::string& message)
        : std::runtime_error(message) {}
};

class Rom {
public:
    std::vector<uint8_t> prg_rom;
    std::vector<uint8_t> chr_rom;
    uint8_t mapper;
    Mirroring screen_mirroring;
    static Rom create(const std::vector<uint8_t>& raw);
    
private:
    Rom(std::vector<uint8_t> prg, std::vector<uint8_t> chr, 
        uint8_t map, Mirroring mirror);
};

#endif // CARTRIDGE_H