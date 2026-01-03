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

void CPU::stack_push(uint8_t data) {
    mem_write(STACK + stack_pointer, data);
    stack_pointer = stack_pointer - 1;  
}

uint8_t CPU::stack_pop() {
    stack_pointer = stack_pointer + 1; 
    return mem_read(STACK + stack_pointer);
}

void CPU::stack_push_u16(uint16_t data) {
    uint8_t hi = (data >> 8) & 0xff;
    uint8_t lo = data & 0xff;
    stack_push(hi);
    stack_push(lo);
}

uint16_t CPU::stack_pop_u16() {
    uint16_t lo = stack_pop();
    uint16_t hi = stack_pop();
    return (hi << 8) | lo;
}

void CPU::set_carry_flag() {
    status.insert(CpuFlags::CARRY);
}

void CPU::clear_carry_flag() {
    status.remove(CpuFlags::CARRY);
}

void CPU::update_zero_and_negative_flags(uint8_t result) {
    if (result == 0) {
        status.insert(CpuFlags::ZERO);
    } else {
        status.remove(CpuFlags::ZERO);
    }
    
    if ((result & 0b10000000) != 0) {
        status.insert(CpuFlags::NEGATIVE);
    } else {
        status.remove(CpuFlags::NEGATIVE);
    }
}

uint16_t CPU::get_operand_address(const AddressingMode& mode) {
    switch (mode) {
        case AddressingMode::Immediate:
            return program_counter;
            
        case AddressingMode::ZeroPage:
            return mem_read(program_counter);
            
        case AddressingMode::Absolute:
            return mem_read_u16(program_counter);
            
        case AddressingMode::ZeroPage_X: {
            uint8_t pos = mem_read(program_counter);
            uint8_t addr = pos + register_x; 
            return addr;
        }
        
        case AddressingMode::ZeroPage_Y: {
            uint8_t pos = mem_read(program_counter);
            uint8_t addr = pos + register_y; 
            return addr;
        }
        
        case AddressingMode::Absolute_X: {
            uint16_t base = mem_read_u16(program_counter);
            uint16_t addr = base + register_x; 
            return addr;
        }
        
        case AddressingMode::Absolute_Y: {
            uint16_t base = mem_read_u16(program_counter);
            uint16_t addr = base + register_y;  
            return addr;
        }
        
        case AddressingMode::Indirect_X: {
            uint8_t base = mem_read(program_counter);
            uint8_t ptr = base + register_x;  
            uint8_t lo = mem_read(ptr);
            uint8_t hi = mem_read(static_cast<uint8_t>(ptr + 1)); 
            return (static_cast<uint16_t>(hi) << 8) | lo;
        }
        
        case AddressingMode::Indirect_Y: {
            uint8_t base = mem_read(program_counter);
            uint8_t lo = mem_read(base);
            uint8_t hi = mem_read(static_cast<uint8_t>(base + 1));  
            uint16_t deref_base = (static_cast<uint16_t>(hi) << 8) | lo;
            uint16_t addr = deref_base + register_y;  
            return addr;
        }
        
        case AddressingMode::NoneAddressing:
            throw std::runtime_error("get_operand_address called with NoneAddressing mode");
    }
    throw std::runtime_error("Unknown addressing mode");
}