#ifndef ADDR_REGISTER_H
#define ADDR_REGISTER_H
#include <cstdint>
#include <utility>

class AddrRegister {
private:
    std::pair<uint8_t, uint8_t> value; // (high_byte, low_byte)
    bool hi_ptr;
    void set(uint16_t data);

public:
    AddrRegister();
    void update(uint8_t data);
    void increment(uint8_t inc);
    void reset_latch();
    uint16_t get() const;
};

#endif // ADDR_REGISTER_H