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
};

#endif // SCROLL_REGISTER_H