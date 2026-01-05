#ifndef SCROLL_REGISTER_H
#define SCROLL_REGISTER_H
#include <cstdint>

class ScrollRegister {
private:
    uint8_t scroll_x;
    uint8_t scroll_y;
    bool latch;
public:
    ScrollRegister();
    void write(uint8_t data);
    void reset_latch();
    uint8_t get_x() const { return scroll_x; }
    uint8_t get_y() const { return scroll_y; }
};

#endif // SCROLL_REGISTER_H