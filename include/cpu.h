#ifndef CPU_H
#define CPU_H
#include "mem.h"
#include "bus.h"
#include "addressing_mode.h"
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
    static const uint8_t OVERFLOW          = 0b01000000;  
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