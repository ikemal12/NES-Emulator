#ifndef MEM_H
#define MEM_H
#include <cstdint>

class Mem {
public:
    virtual ~Mem() = default;
    virtual uint8_t mem_read(uint16_t addr) const = 0;
    virtual void mem_write(uint16_t addr, uint8_t data) = 0;
    
    virtual uint16_t mem_read_u16(uint16_t pos) const {
        uint16_t lo = mem_read(pos);
        uint16_t hi = mem_read(pos + 1);
        return (hi << 8) | lo;
    }
    
    virtual void mem_write_u16(uint16_t pos, uint16_t data) {
        uint8_t hi = (data >> 8) & 0xff;
        uint8_t lo = data & 0xff;
        mem_write(pos, lo);
        mem_write(pos + 1, hi);
    }
};

#endif // MEM_H