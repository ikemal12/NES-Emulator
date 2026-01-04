#include "render/renderer.h"

Renderer::Renderer() 
    : frame()
{}

void Renderer::render(const NesPPU& ppu) {
    // TODO:  For now, just clear to the background color (palette index 0)
    auto bg_color = Palette::get_color(ppu.palette_table[0]);
    for (size_t y = 0; y < Frame::HEIGHT; y++) {
        for (size_t x = 0; x < Frame::WIDTH; x++) {
            frame.set_pixel(x, y, bg_color);
        }
    }
    // render_background(ppu);
    // render_sprites(ppu);
}

const Frame& Renderer::get_frame() const {
    return frame;
}

std::array<uint8_t, 3> Renderer::bg_palette_color(const NesPPU& ppu, uint8_t palette, uint8_t pixel) {
    uint16_t palette_start = 0x3F00 + (palette * 4);
    uint8_t color_index;
    if (pixel == 0) {
        color_index = ppu.palette_table[0];
    } else {
        color_index = ppu.palette_table[(palette_start - 0x3F00) + pixel];
    }
    return Palette::get_color(color_index);
}

void Renderer::render_background(const NesPPU& ppu) {
    // TODO: This will read nametables and CHR data to draw tiles
}

void Renderer::render_sprites(const NesPPU& ppu) {
    // TODO: This will read OAM data and draw sprites
}
