#ifndef RENDERER_H
#define RENDERER_H
#include "ppu.h"
#include "render/frame.h"
#include "render/palette.h"
#include <cstdint>

class Renderer {
private:
    Frame frame;
public:
    Renderer();
    void render(const NesPPU& ppu);
    const Frame& get_frame() const;
private:
    std::array<uint8_t, 3> bg_palette_color(const NesPPU& ppu, uint8_t palette, uint8_t pixel);
    void render_background(const NesPPU& ppu);
    void render_sprites(const NesPPU& ppu);
};

#endif // RENDERER_H
