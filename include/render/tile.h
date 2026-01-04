#ifndef TILE_H
#define TILE_H
#include "render/frame.h"
#include "render/palette.h"
#include <cstdint>
#include <vector>

namespace TileRenderer {
    Frame show_tile(const std::vector<uint8_t>& chr_rom, size_t bank, size_t tile_n);
    void render_tile_at(Frame& frame, const std::vector<uint8_t>& chr_rom, 
                       size_t bank, size_t tile_n, size_t screen_x, size_t screen_y);
}

#endif // TILE_H
