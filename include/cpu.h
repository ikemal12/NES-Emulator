#ifndef CPU_H
#define CPU_H
#include "mem.h"
#include "bus.h"
#include "addressing_mode.h"
#include "opcodes.h"
#include <cstdint>

const uint16_t STACK = 0x0100;
const uint8_t STACK_RESET = 0xfd;

class CpuFlags {
public:
    uint8_t bits;
    static const uint8_t CARRY             = 0b00000001;  
    static const uint8_t ZERO              = 0b00000010;
    static const uint8_t INTERRUPT_DISABLE = 0b00000100;  
    static const uint8_t DECIMAL_MODE      = 0b00001000;  // not used on NES
    static const uint8_t BREAK             = 0b00010000;  
    static const uint8_t BREAK2            = 0b00100000;  // Unused (always 1)
    static const uint8_t OVERFLOW_FLAG     = 0b01000000;  
    static const uint8_t NEGATIVE          = 0b10000000;  
    
    CpuFlags() : bits(0) {}
    explicit CpuFlags(uint8_t value) : bits(value) {}
    
    bool contains(uint8_t flag) const {
        return (bits & flag) != 0;
    }
    
    void insert(uint8_t flag) {
        bits |= flag;
    }
    
    void remove(uint8_t flag) {
        bits &= ~flag;
    }
    
    static CpuFlags from_bits_truncate(uint8_t value) {
        return CpuFlags(value);
    }
};

class CPU : public Mem {
public:
    uint8_t register_a;       
    uint8_t register_x;       
    uint8_t register_y;       
    CpuFlags status;          
    uint8_t stack_pointer;    // (points into 0x0100-0x01FF)
    uint16_t program_counter; 
    Bus bus;                 
    
    explicit CPU(Bus bus);
    uint8_t mem_read(uint16_t addr) const override;
    void mem_write(uint16_t addr, uint8_t data) override;
    void reset();

    void run();
    template<typename F>
    void run_with_callback(F callback) {
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
                    branch(!status.contains(CpuFlags::OVERFLOW_FLAG));
                    break;
                case 0x70: // BVS - Branch if Overflow Set
                    branch(status.contains(CpuFlags::OVERFLOW_FLAG));
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
private:
    void stack_push(uint8_t data);
    uint8_t stack_pop();
    void stack_push_u16(uint16_t data);
    uint16_t stack_pop_u16();
    
    void set_carry_flag();
    void clear_carry_flag();
    void update_zero_and_negative_flags(uint8_t result);
    uint16_t get_operand_address(const AddressingMode& mode);

    void lda(const AddressingMode& mode);  
    void ldx(const AddressingMode& mode); 
    void ldy(const AddressingMode& mode);  
    void sta(const AddressingMode& mode);  
    void stx(const AddressingMode& mode);  
    void sty(const AddressingMode& mode);  
    void lax(const AddressingMode& mode); 
    void set_register_a(uint8_t value);

    void add_to_register_a(uint8_t data); 
    void adc(const AddressingMode& mode);  
    void sbc(const AddressingMode& mode);  
    
    void and_op(const AddressingMode& mode); 
    void ora(const AddressingMode& mode);     
    void eor(const AddressingMode& mode);   
    
    void compare(const AddressingMode& mode, uint8_t compare_with);
    void cmp(const AddressingMode& mode);  
    void cpx(const AddressingMode& mode);  
    void cpy(const AddressingMode& mode);  
    void bit(const AddressingMode& mode);

    void asl_accumulator(); 
    void asl(const AddressingMode& mode);  
    void lsr_accumulator();  
    void lsr(const AddressingMode& mode);  
    void rol_accumulator();  
    void rol(const AddressingMode& mode); 
    void ror_accumulator();  
    void ror(const AddressingMode& mode);  
    
    void inc(const AddressingMode& mode); 
    void dec(const AddressingMode& mode);  
    void inx();  
    void iny();  
    void dex();  
    void dey(); 

    void branch(bool condition);
    void jmp_absolute();
    void jmp_indirect();
    void jsr();  
    void rts();  
    void rti();  
    
    void pha();  
    void pla();  
    void php();  
    void plp();  
    
    void tax();  
    void tay(); 
    void txa();  
    void tya(); 
    void tsx();  
    void txs();  
    
    void clc();  
    void sec();  
    void cli();  
    void sei(); 
    void cld();  
    void sed();  
    void clv();  
};

#endif // CPU_H