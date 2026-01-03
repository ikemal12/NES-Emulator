#ifndef OPCODES_H
#define OPCODES_H
#include "addressing_mode.h"
#include <cstdint>
#include <vector>
#include <unordered_map>

struct OpCode {
    uint8_t code;              
    const char* mnemonic;      
    uint8_t len;              
    uint8_t cycles;            
    AddressingMode mode;      
    
    OpCode(uint8_t code, const char* mnemonic, uint8_t len, 
           uint8_t cycles, AddressingMode mode);
};

namespace opcodes {
    const std::vector<OpCode>& get_cpu_opcodes();
    const std::unordered_map<uint8_t, const OpCode*>& get_opcodes_map();
}

#endif // OPCODES_H