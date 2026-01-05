#include "render/renderer.h"

Renderer::Renderer() 
    : frame()
{}

const Frame& Renderer::get_frame() const {
    return frame;
}

std::array<uint8_t, 4> Renderer::bg_palette(const NesPPU& ppu, size_t tile_column, size_t tile_row) const {
    size_t attr_table_idx = (tile_row / 4) * 8 + (tile_column / 4);
    uint8_t attr_byte = ppu.vram[0x3C0 + attr_table_idx];  // using first nametable for now
    uint8_t palette_idx;
    switch ((tile_column % 4 / 2) << 1 | (tile_row % 4 / 2)) {
        case 0: 
            palette_idx = attr_byte & 0b11;
            break;
        case 1: 
            palette_idx = (attr_byte >> 2) & 0b11;
            break;
        case 2:
            palette_idx = (attr_byte >> 4) & 0b11;
            break;
        case 3: 
            palette_idx = (attr_byte >> 6) & 0b11;
            break;
        default:
            palette_idx = 0;
    }
    size_t palette_start = 1 + (palette_idx * 4);
    return {
        ppu.palette_table[0],                    
        ppu.palette_table[palette_start],
        ppu.palette_table[palette_start + 1],
        ppu.palette_table[palette_start + 2]
    };
}

std::array<uint8_t, 4> Renderer::sprite_palette(const NesPPU& ppu, uint8_t palette_idx) const {
    size_t start = 0x11 + (palette_idx * 4);
    return {
        0,  
        ppu.palette_table[start],
        ppu.palette_table[start + 1],
        ppu.palette_table[start + 2]
    };
}

void Renderer::render_background(const NesPPU& ppu) {    
    uint16_t bank = ppu.ctrl.background_pattern_addr();
    for (size_t i = 0; i < 0x03C0; i++) {
        uint8_t tile_idx = ppu.vram[i];
        size_t tile_column = i % 32;  
        size_t tile_row = i / 32;
        uint16_t tile_start = bank + (tile_idx * 16);
        const uint8_t* tile = &ppu.chr_rom[tile_start];
        auto palette = bg_palette(ppu, tile_column, tile_row);
        
        for (size_t y = 0; y < 8; y++) {
            uint8_t lower = tile[y];      
            uint8_t upper = tile[y + 8];  
            for (int x = 7; x >= 0; x--) {
                uint8_t value = ((upper & 1) << 1) | (lower & 1);
                upper >>= 1;
                lower >>= 1;
                uint8_t color_idx = palette[value];
                auto rgb = Palette::get_color(color_idx);
                size_t pixel_x = tile_column * 8 + x;
                size_t pixel_y = tile_row * 8 + y;
                frame.set_pixel(pixel_x, pixel_y, rgb);
            }
        }
    }
}

void Renderer::render_sprites(const NesPPU& ppu) {
    uint16_t bank = ppu.ctrl.sprite_pattern_addr();
    for (int i = static_cast<int>(ppu.oam_data.size()) - 4; i >= 0; i -= 4) {
        uint8_t tile_y = ppu.oam_data[i];
        uint8_t tile_idx = ppu.oam_data[i + 1];
        uint8_t attributes = ppu.oam_data[i + 2];
        uint8_t tile_x = ppu.oam_data[i + 3];
        bool flip_vertical = (attributes & 0x80) != 0;
        bool flip_horizontal = (attributes & 0x40) != 0;
        uint8_t palette_idx = attributes & 0x03;
        auto palette = sprite_palette(ppu, palette_idx);
        uint16_t tile_start = bank + (tile_idx * 16);
        const uint8_t* tile = &ppu.chr_rom[tile_start];
        
        for (size_t y = 0; y < 8; y++) {
            uint8_t lower = tile[y];
            uint8_t upper = tile[y + 8];
            for (int x = 7; x >= 0; x--) {
                uint8_t value = ((upper & 1) << 1) | (lower & 1);
                upper >>= 1;
                lower >>= 1;
                if (value == 0) {
                    continue;
                }
                uint8_t color_idx = palette[value];
                auto rgb = Palette::get_color(color_idx);
                size_t pixel_x, pixel_y;
                if (flip_horizontal) {
                    pixel_x = tile_x + (7 - x);
                } else {
                    pixel_x = tile_x + x;
                }
                
                if (flip_vertical) {
                    pixel_y = tile_y + (7 - y);
                } else {
                    pixel_y = tile_y + y;
                }
                if (pixel_x < Frame::WIDTH && pixel_y < Frame::HEIGHT) {
                    frame.set_pixel(pixel_x, pixel_y, rgb);
                }
            }
        }
    }
}

void Renderer::render(const NesPPU& ppu) {
    render_background(ppu);
    render_sprites(ppu);
}