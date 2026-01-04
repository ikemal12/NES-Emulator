#include "render/tile.h"
#include <cassert>

namespace TileRenderer {

Frame show_tile(const std::vector<uint8_t>& chr_rom, size_t bank, size_t tile_n) {
    assert(bank <= 1);
    Frame frame;
    size_t bank_offset = bank * 0x1000;
    size_t tile_start = bank_offset + tile_n * 16;
    
    for (size_t y = 0; y < 8; y++) {
        uint8_t lower = chr_rom[tile_start + y];      
        uint8_t upper = chr_rom[tile_start + y + 8];  
        for (int x = 7; x >= 0; x--) {
            uint8_t value = ((upper & 1) << 1) | (lower & 1);
            upper >>= 1;
            lower >>= 1;
            
            // Map 2-bit value to colour from palette
            // Using hardcoded palette indices for now 
            std::array<uint8_t, 3> rgb;
            switch (value) {
                case 0: rgb = Palette::get_color(0x01); break;  
                case 1: rgb = Palette::get_color(0x23); break;  
                case 2: rgb = Palette::get_color(0x27); break;  
                case 3: rgb = Palette::get_color(0x30); break;  
                default: rgb = Palette::get_color(0x00); break; 
            }
            frame.set_pixel(x, y, rgb);
        }
    }
    return frame;
}

void render_tile_at(Frame& frame, const std::vector<uint8_t>& chr_rom, 
                   size_t bank, size_t tile_n, size_t screen_x, size_t screen_y) {
    assert(bank <= 1);
    size_t bank_offset = bank * 0x1000;
    size_t tile_start = bank_offset + tile_n * 16;
    
    for (size_t y = 0; y < 8; y++) {
        uint8_t lower = chr_rom[tile_start + y];
        uint8_t upper = chr_rom[tile_start + y + 8];
        
        for (int x = 7; x >= 0; x--) {
            uint8_t value = ((upper & 1) << 1) | (lower & 1);
            upper >>= 1;
            lower >>= 1;
            std::array<uint8_t, 3> rgb;
            switch (value) {
                case 0: rgb = Palette::get_color(0x01); break;
                case 1: rgb = Palette::get_color(0x23); break;
                case 2: rgb = Palette::get_color(0x27); break;
                case 3: rgb = Palette::get_color(0x30); break;
                default: rgb = Palette::get_color(0x00); break;
            }
            size_t pixel_x = screen_x + x;
            size_t pixel_y = screen_y + y;
            if (pixel_x < Frame::WIDTH && pixel_y < Frame::HEIGHT) {
                frame.set_pixel(pixel_x, pixel_y, rgb);
            }
        }
    }
}

} // namespace TileRenderer
