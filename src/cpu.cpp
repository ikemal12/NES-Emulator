#include "cpu.h"

CPU::CPU(Bus bus)
    : bus(std::move(bus))
    , register_a(0)
    , register_x(0)
    , register_y(0)
    , status(CpuFlags::from_bits_truncate(0b00100100))
    , stack_pointer(STACK_RESET)
    , program_counter(0)
{}

uint8_t CPU::mem_read(uint16_t addr) const {
    return bus.mem_read(addr);
}

void CPU::mem_write(uint16_t addr, uint8_t data) {
    const_cast<Bus&>(bus).mem_write(addr, data);
}

void CPU::reset() {
    register_a = 0;
    register_x = 0;
    register_y = 0;
    stack_pointer = STACK_RESET;
    status = CpuFlags::from_bits_truncate(0b00100100);
    program_counter = mem_read_u16(0xFFFC);
}