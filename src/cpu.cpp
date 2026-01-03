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

void CPU::lda(const AddressingMode& mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t value = mem_read(addr);
    register_a = value;
    update_zero_and_negative_flags(register_a);
}

void CPU::ldx(const AddressingMode& mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t value = mem_read(addr);
    register_x = value;
    update_zero_and_negative_flags(register_x);
}

void CPU::ldy(const AddressingMode& mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t value = mem_read(addr);
    register_y = value;
    update_zero_and_negative_flags(register_y);
}

void CPU::sta(const AddressingMode& mode) {
    uint16_t addr = get_operand_address(mode);
    mem_write(addr, register_a);
}

void CPU::stx(const AddressingMode& mode) {
    uint16_t addr = get_operand_address(mode);
    mem_write(addr, register_x);
}

void CPU::sty(const AddressingMode& mode) {
    uint16_t addr = get_operand_address(mode);
    mem_write(addr, register_y);
}

void CPU::lax(const AddressingMode& mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t value = mem_read(addr);
    register_a = value;
    register_x = value;
    update_zero_and_negative_flags(register_a);
}

void CPU::set_register_a(uint8_t value) {
    register_a = value;
    update_zero_and_negative_flags(register_a);
}

void CPU::add_to_register_a(uint8_t data) {
    uint16_t sum = static_cast<uint16_t>(register_a) + 
                   static_cast<uint16_t>(data) + 
                   (status.contains(CpuFlags::CARRY) ? 1 : 0);
    
    if (sum > 0xFF) {
        status.insert(CpuFlags::CARRY);
    } else {
        status.remove(CpuFlags::CARRY);
    }
    
    uint8_t result = static_cast<uint8_t>(sum);
    if ((data ^ result) & (result ^ register_a) & 0x80) {
        status.insert(CpuFlags::OVERFLOW);
    } else {
        status.remove(CpuFlags::OVERFLOW);
    }
    set_register_a(result);
}

void CPU::adc(const AddressingMode& mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t value = mem_read(addr);
    add_to_register_a(value);
}

void CPU::sbc(const AddressingMode& mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t data = mem_read(addr);
    int8_t signed_data = static_cast<int8_t>(data);
    uint8_t inverted = static_cast<uint8_t>(-signed_data - 1);
    add_to_register_a(inverted);
}

void CPU::and_op(const AddressingMode& mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t data = mem_read(addr);
    set_register_a(data & register_a);
}

void CPU::ora(const AddressingMode& mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t data = mem_read(addr);
    set_register_a(data | register_a);
}

void CPU::eor(const AddressingMode& mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t data = mem_read(addr);
    set_register_a(data ^ register_a);
}

void CPU::compare(const AddressingMode& mode, uint8_t compare_with) {
    uint16_t addr = get_operand_address(mode);
    uint8_t data = mem_read(addr);
    if (data <= compare_with) {
        status.insert(CpuFlags::CARRY);
    } else {
        status.remove(CpuFlags::CARRY);
    }
    uint8_t result = compare_with - data;  
    update_zero_and_negative_flags(result);
}

void CPU::cmp(const AddressingMode& mode) {
    compare(mode, register_a);
}

void CPU::cpx(const AddressingMode& mode) {
    compare(mode, register_x);
}

void CPU::cpy(const AddressingMode& mode) {
    compare(mode, register_y);
}

void CPU::bit(const AddressingMode& mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t data = mem_read(addr);
    uint8_t result = register_a & data;
    if (result == 0) {
        status.insert(CpuFlags::ZERO);
    } else {
        status.remove(CpuFlags::ZERO);
    }
    if (data & 0b10000000) {
        status.insert(CpuFlags::NEGATIVE);
    } else {
        status.remove(CpuFlags::NEGATIVE);
    }
    if (data & 0b01000000) {
        status.insert(CpuFlags::OVERFLOW);
    } else {
        status.remove(CpuFlags::OVERFLOW);
    }
}