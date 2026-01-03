#include "cpu.h"
#include "opcodes.h"
#include <functional>

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

void CPU::asl_accumulator() {
    uint8_t data = register_a;
    if ((data >> 7) == 1) {
        set_carry_flag();
    } else {
        clear_carry_flag();
    }
    data = data << 1;
    set_register_a(data);
}

void CPU::asl(const AddressingMode& mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t data = mem_read(addr);
    if ((data >> 7) == 1) {
        set_carry_flag();
    } else {
        clear_carry_flag();
    }
    data = data << 1;
    mem_write(addr, data);
    update_zero_and_negative_flags(data);
}

void CPU::lsr_accumulator() {
    uint8_t data = register_a;
    if ((data & 1) == 1) {
        set_carry_flag();
    } else {
        clear_carry_flag();
    }
    data = data >> 1;
    set_register_a(data);
}

void CPU::lsr(const AddressingMode& mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t data = mem_read(addr);
    if ((data & 1) == 1) {
        set_carry_flag();
    } else {
        clear_carry_flag();
    }
    data = data >> 1;
    mem_write(addr, data);
    update_zero_and_negative_flags(data);
}

void CPU::rol_accumulator() {
    uint8_t data = register_a;
    bool old_carry = status.contains(CpuFlags::CARRY);
    if ((data >> 7) == 1) {
        set_carry_flag();
    } else {
        clear_carry_flag();
    }
    data = data << 1;
    if (old_carry) {
        data = data | 1;
    }
    set_register_a(data);
}

void CPU::rol(const AddressingMode& mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t data = mem_read(addr);
    bool old_carry = status.contains(CpuFlags::CARRY);
    if ((data >> 7) == 1) {
        set_carry_flag();
    } else {
        clear_carry_flag();
    }
    data = data << 1;
    if (old_carry) {
        data = data | 1;
    }
    mem_write(addr, data);
    update_zero_and_negative_flags(data);
}

void CPU::ror_accumulator() {
    uint8_t data = register_a;
    bool old_carry = status.contains(CpuFlags::CARRY);
    if ((data & 1) == 1) {
        set_carry_flag();
    } else {
        clear_carry_flag();
    }
    data = data >> 1;
    if (old_carry) {
        data = data | 0b10000000;
    }
    set_register_a(data);
}

void CPU::ror(const AddressingMode& mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t data = mem_read(addr);
    bool old_carry = status.contains(CpuFlags::CARRY);
    if ((data & 1) == 1) {
        set_carry_flag();
    } else {
        clear_carry_flag();
    }
    data = data >> 1;
    if (old_carry) {
        data = data | 0b10000000;
    }
    mem_write(addr, data);
    update_zero_and_negative_flags(data);
}

void CPU::inc(const AddressingMode& mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t data = mem_read(addr);
    data = data + 1; 
    mem_write(addr, data);
    update_zero_and_negative_flags(data);
}

void CPU::dec(const AddressingMode& mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t data = mem_read(addr);
    data = data - 1;  
    mem_write(addr, data);
    update_zero_and_negative_flags(data);
}

void CPU::inx() {
    register_x = register_x + 1; 
    update_zero_and_negative_flags(register_x);
}

void CPU::iny() {
    register_y = register_y + 1;  
    update_zero_and_negative_flags(register_y);
}

void CPU::dex() {
    register_x = register_x - 1; 
    update_zero_and_negative_flags(register_x);
}

void CPU::dey() {
    register_y = register_y - 1;  
    update_zero_and_negative_flags(register_y);
}

void CPU::branch(bool condition) {
    if (condition) {
        int8_t jump = static_cast<int8_t>(mem_read(program_counter));
        uint16_t jump_addr = program_counter + 1 + static_cast<uint16_t>(jump);
        program_counter = jump_addr;
    }
}

void CPU::jmp_absolute() {
    uint16_t addr = mem_read_u16(program_counter);
    program_counter = addr;
}

void CPU::jmp_indirect() {
    uint16_t addr = mem_read_u16(program_counter);
    uint16_t indirect_ref;
    if ((addr & 0x00FF) == 0x00FF) {
        uint8_t lo = mem_read(addr);
        uint8_t hi = mem_read(addr & 0xFF00); 
        indirect_ref = (static_cast<uint16_t>(hi) << 8) | lo;
    } else {
        indirect_ref = mem_read_u16(addr);
    }
    program_counter = indirect_ref;
}

void CPU::jsr() {
    stack_push_u16(program_counter + 2 - 1);
    uint16_t target_addr = mem_read_u16(program_counter);
    program_counter = target_addr;
}

void CPU::rts() {
    program_counter = stack_pop_u16() + 1;
}

void CPU::rti() {
    status = CpuFlags::from_bits_truncate(stack_pop());
    status.remove(CpuFlags::BREAK);
    status.remove(CpuFlags::BREAK2);
    program_counter = stack_pop_u16();
}

void CPU::pha() {
    stack_push(register_a);
}

void CPU::pla() {
    uint8_t data = stack_pop();
    set_register_a(data);
}

void CPU::php() {
    CpuFlags flags = status;
    flags.insert(CpuFlags::BREAK);
    flags.insert(CpuFlags::BREAK2);
    stack_push(flags.bits);
}

void CPU::plp() {
    status = CpuFlags::from_bits_truncate(stack_pop());
    status.remove(CpuFlags::BREAK);
    status.remove(CpuFlags::BREAK2);
}

void CPU::tax() {
    register_x = register_a;
    update_zero_and_negative_flags(register_x);
}

void CPU::tay() {
    register_y = register_a;
    update_zero_and_negative_flags(register_y);
}

void CPU::txa() {
    register_a = register_x;
    update_zero_and_negative_flags(register_a);
}

void CPU::tya() {
    register_a = register_y;
    update_zero_and_negative_flags(register_a);
}

void CPU::tsx() {
    register_x = stack_pointer;
    update_zero_and_negative_flags(register_x);
}

void CPU::txs() {
    stack_pointer = register_x;
}

void CPU::clc() {
    status.remove(CpuFlags::CARRY);
}

void CPU::sec() {
    status.insert(CpuFlags::CARRY);
}

void CPU::cli() {
    status.remove(CpuFlags::INTERRUPT_DISABLE);
}

void CPU::sei() {
    status.insert(CpuFlags::INTERRUPT_DISABLE);
}

void CPU::cld() {
    status.remove(CpuFlags::DECIMAL_MODE);
}

void CPU::sed() {
    status.insert(CpuFlags::DECIMAL_MODE);
}

void CPU::clv() {
    status.remove(CpuFlags::OVERFLOW);
}

void CPU::run() {
    run_with_callback([](CPU&) {});
}

template<typename F>
void CPU::run_with_callback(F callback) {
    const auto& opcodes_map = opcodes::get_opcodes_map();
    
    while (true) {
        callback(*this);
        uint8_t code = mem_read(program_counter);
        program_counter += 1;
        uint16_t program_counter_state = program_counter;
        auto it = opcodes_map.find(code);
        if (it == opcodes_map.end()) {
            throw std::runtime_error("Unknown opcode: " + std::to_string(code));
        }
        const OpCode* opcode = it->second;
        
        switch (code) {
            case 0x00:
                return;  
            
            // LDA - Load Accumulator
            case 0xa9: case 0xa5: case 0xb5: case 0xad:
            case 0xbd: case 0xb9: case 0xa1: case 0xb1:
                lda(opcode->mode);
                break;
            
            // LDX - Load X Register
            case 0xa2: case 0xa6: case 0xb6: case 0xae: case 0xbe:
                ldx(opcode->mode);
                break;
            
            // LDY - Load Y Register
            case 0xa0: case 0xa4: case 0xb4: case 0xac: case 0xbc:
                ldy(opcode->mode);
                break;
            
            // STA - Store Accumulator
            case 0x85: case 0x95: case 0x8d: case 0x9d:
            case 0x99: case 0x81: case 0x91:
                sta(opcode->mode);
                break;
            
            // STX - Store X Register
            case 0x86: case 0x96: case 0x8e:
                stx(opcode->mode);
                break;
            
            // STY - Store Y Register
            case 0x84: case 0x94: case 0x8c:
                sty(opcode->mode);
                break;
            
            // ADC - Add with Carry
            case 0x69: case 0x65: case 0x75: case 0x6d:
            case 0x7d: case 0x79: case 0x61: case 0x71:
                adc(opcode->mode);
                break;
            
            // SBC - Subtract with Carry
            case 0xe9: case 0xe5: case 0xf5: case 0xed:
            case 0xfd: case 0xf9: case 0xe1: case 0xf1:
                sbc(opcode->mode);
                break;
            
            // AND - Logical AND
            case 0x29: case 0x25: case 0x35: case 0x2d:
            case 0x3d: case 0x39: case 0x21: case 0x31:
                and_op(opcode->mode);
                break;
            
            // ORA - Logical OR
            case 0x09: case 0x05: case 0x15: case 0x0d:
            case 0x1d: case 0x19: case 0x01: case 0x11:
                ora(opcode->mode);
                break;
            
            // EOR - Exclusive OR
            case 0x49: case 0x45: case 0x55: case 0x4d:
            case 0x5d: case 0x59: case 0x41: case 0x51:
                eor(opcode->mode);
                break;
            
            // ASL - Arithmetic Shift Left
            case 0x0a:
                asl_accumulator();
                break;
            case 0x06: case 0x16: case 0x0e: case 0x1e:
                asl(opcode->mode);
                break;
            
            // LSR - Logical Shift Right
            case 0x4a:
                lsr_accumulator();
                break;
            case 0x46: case 0x56: case 0x4e: case 0x5e:
                lsr(opcode->mode);
                break;
            
            // ROL - Rotate Left
            case 0x2a:
                rol_accumulator();
                break;
            case 0x26: case 0x36: case 0x2e: case 0x3e:
                rol(opcode->mode);
                break;
            
            // ROR - Rotate Right
            case 0x6a:
                ror_accumulator();
                break;
            case 0x66: case 0x76: case 0x6e: case 0x7e:
                ror(opcode->mode);
                break;
            
            // INC - Increment Memory
            case 0xe6: case 0xf6: case 0xee: case 0xfe:
                inc(opcode->mode);
                break;
            
            // DEC - Decrement Memory
            case 0xc6: case 0xd6: case 0xce: case 0xde:
                dec(opcode->mode);
                break;
            
            // CMP - Compare with Accumulator
            case 0xc9: case 0xc5: case 0xd5: case 0xcd:
            case 0xdd: case 0xd9: case 0xc1: case 0xd1:
                cmp(opcode->mode);
                break;
            
            // CPX - Compare with X
            case 0xe0: case 0xe4: case 0xec:
                cpx(opcode->mode);
                break;
            
            // CPY - Compare with Y
            case 0xc0: case 0xc4: case 0xcc:
                cpy(opcode->mode);
                break;
            
            // BIT - Bit Test
            case 0x24: case 0x2c:
                bit(opcode->mode);
                break;
            
            // Branch Instructions
            case 0xf0: // BEQ - Branch if Equal (Zero set)
                branch(status.contains(CpuFlags::ZERO));
                break;
            case 0xd0: // BNE - Branch if Not Equal (Zero clear)
                branch(!status.contains(CpuFlags::ZERO));
                break;
            case 0x90: // BCC - Branch if Carry Clear
                branch(!status.contains(CpuFlags::CARRY));
                break;
            case 0xb0: // BCS - Branch if Carry Set
                branch(status.contains(CpuFlags::CARRY));
                break;
            case 0x30: // BMI - Branch if Minus (Negative set)
                branch(status.contains(CpuFlags::NEGATIVE));
                break;
            case 0x10: // BPL - Branch if Plus (Negative clear)
                branch(!status.contains(CpuFlags::NEGATIVE));
                break;
            case 0x50: // BVC - Branch if Overflow Clear
                branch(!status.contains(CpuFlags::OVERFLOW));
                break;
            case 0x70: // BVS - Branch if Overflow Set
                branch(status.contains(CpuFlags::OVERFLOW));
                break;
            
            // Jump and Subroutine
            case 0x4c: // JMP Absolute
                jmp_absolute();
                break;
            case 0x6c: // JMP Indirect
                jmp_indirect();
                break;
            case 0x20: // JSR
                jsr();
                break;
            case 0x60: // RTS
                rts();
                break;
            case 0x40: // RTI
                rti();
                break;
            
            // Stack Operations
            case 0x48: // PHA
                pha();
                break;
            case 0x68: // PLA
                pla();
                break;
            case 0x08: // PHP
                php();
                break;
            case 0x28: // PLP
                plp();
                break;
            
            // Register Transfers
            case 0xaa: // TAX
                tax();
                break;
            case 0xa8: // TAY
                tay();
                break;
            case 0x8a: // TXA
                txa();
                break;
            case 0x98: // TYA
                tya();
                break;
            case 0xba: // TSX
                tsx();
                break;
            case 0x9a: // TXS
                txs();
                break;
            
            // Increment/Decrement Registers
            case 0xe8: // INX
                inx();
                break;
            case 0xc8: // INY
                iny();
                break;
            case 0xca: // DEX
                dex();
                break;
            case 0x88: // DEY
                dey();
                break;
            
            // Flag Instructions
            case 0x18: // CLC
                clc();
                break;
            case 0x38: // SEC
                sec();
                break;
            case 0x58: // CLI
                cli();
                break;
            case 0x78: // SEI
                sei();
                break;
            case 0xd8: // CLD
                cld();
                break;
            case 0xf8: // SED
                sed();
                break;
            case 0xb8: // CLV
                clv();
                break;
            
            // NOP - No Operation
            case 0xea:
                break;
            
            // Unofficial NOPs (various addressing modes)
            case 0x04: case 0x44: case 0x64: case 0x0c:
            case 0x14: case 0x34: case 0x54: case 0x74:
            case 0xd4: case 0xf4: case 0x1a: case 0x3a:
            case 0x5a: case 0x7a: case 0xda: case 0xfa:
            case 0x80: case 0x1c: case 0x3c: case 0x5c:
            case 0x7c: case 0xdc: case 0xfc:
                break; 
            
            // LAX - Unofficial: Load A and X
            case 0xa3: case 0xa7: case 0xaf: case 0xb3:
            case 0xb7: case 0xbf:
                lax(opcode->mode);
                break;
            
            default:
                throw std::runtime_error("Unimplemented opcode: " + std::to_string(code));
        }
        if (program_counter_state == program_counter) {
            program_counter += (opcode->len - 1);
        }
    }
}

template void CPU::run_with_callback<std::function<void(CPU&)>>(std::function<void(CPU&)>);
template void CPU::run_with_callback<void(*)(CPU&)>(void(*)(CPU&));