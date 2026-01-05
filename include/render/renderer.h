#ifndef RENDERER_H
#define RENDERER_H
#include "ppu.h"
#include "render/frame.h"
#include "render/palette.h"
#include <cstdint>
#include <array>

class Renderer {
private:
    Frame frame;
public:
    Renderer();
    void render(const NesPPU& ppu);
    const Frame& get_frame() const;
private:
    std::array<uint8_t, 4> bg_palette(const NesPPU& ppu, size_t tile_column, size_t tile_row) const;
    std::array<uint8_t, 4> sprite_palette(const NesPPU& ppu, uint8_t palette_idx) const;
    void render_background(const NesPPU& ppu);
    void render_sprites(const NesPPU& ppu);
};

#endif // RENDERER_H
