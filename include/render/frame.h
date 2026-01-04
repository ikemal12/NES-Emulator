#ifndef FRAME_H
#define FRAME_H
#include <cstdint>
#include <array>

class Frame {
public:
    static constexpr size_t WIDTH = 256;
    static constexpr size_t HEIGHT = 240;
private:
    std::array<uint8_t, WIDTH * HEIGHT * 3> data;
public:
    Frame();
    void set_pixel(size_t x, size_t y, uint8_t r, uint8_t g, uint8_t b);
    void set_pixel(size_t x, size_t y, const std::array<uint8_t, 3>& rgb);
    const std::array<uint8_t, WIDTH * HEIGHT * 3>& get_data() const;
};

#endif // FRAME_H
