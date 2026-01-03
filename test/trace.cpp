#include "trace.h"
#include "opcodes.h"
#include <sstream>
#include <iomanip>
#include <vector>

std::string trace(const CPU& cpu) {
    const auto& opcodes_map = opcodes::get_opcodes_map();
    uint8_t code = cpu.mem_read(cpu.program_counter);
    uint16_t begin = cpu.program_counter;
    auto it = opcodes_map.find(code);
    if (it == opcodes_map.end()) {
        std::ostringstream oss;
        oss << std::uppercase << std::setfill('0') << std::hex
            << std::setw(4) << begin << "  "
            << std::setw(2) << static_cast<int>(code) << "        "
            << "*UNKNOWN                        "
            << "A:" << std::setw(2) << static_cast<int>(cpu.register_a) << " "
            << "X:" << std::setw(2) << static_cast<int>(cpu.register_x) << " "
            << "Y:" << std::setw(2) << static_cast<int>(cpu.register_y) << " "
            << "P:" << std::setw(2) << static_cast<int>(cpu.status.bits) << " "
            << "SP:" << std::setw(2) << static_cast<int>(cpu.stack_pointer);
        return oss.str();
    }
    
    const OpCode* opcode = it->second;
    std::vector<uint8_t> hex_dump;
    hex_dump.push_back(code);
    uint16_t mem_addr = 0;
    uint8_t stored_value = 0;
    
    if (opcode->mode != AddressingMode::NoneAddressing && 
        opcode->mode != AddressingMode::Immediate) {
        
        switch (opcode->mode) {
            case AddressingMode::ZeroPage:
                mem_addr = cpu.mem_read(begin + 1);
                stored_value = cpu.mem_read(mem_addr);
                break;
                
            case AddressingMode::ZeroPage_X: {
                uint8_t pos = cpu.mem_read(begin + 1);
                mem_addr = static_cast<uint8_t>(pos + cpu.register_x);
                stored_value = cpu.mem_read(mem_addr);
                break;
            }
            
            case AddressingMode::ZeroPage_Y: {
                uint8_t pos = cpu.mem_read(begin + 1);
                mem_addr = static_cast<uint8_t>(pos + cpu.register_y);
                stored_value = cpu.mem_read(mem_addr);
                break;
            }
            
            case AddressingMode::Absolute:
                mem_addr = cpu.mem_read_u16(begin + 1);
                stored_value = cpu.mem_read(mem_addr);
                break;
                
            case AddressingMode::Absolute_X:
                mem_addr = cpu.mem_read_u16(begin + 1) + cpu.register_x;
                stored_value = cpu.mem_read(mem_addr);
                break;
                
            case AddressingMode::Absolute_Y:
                mem_addr = cpu.mem_read_u16(begin + 1) + cpu.register_y;
                stored_value = cpu.mem_read(mem_addr);
                break;
                
            case AddressingMode::Indirect_X: {
                uint8_t base = cpu.mem_read(begin + 1);
                uint8_t ptr = base + cpu.register_x;
                uint8_t lo = cpu.mem_read(ptr);
                uint8_t hi = cpu.mem_read(static_cast<uint8_t>(ptr + 1));
                mem_addr = (static_cast<uint16_t>(hi) << 8) | lo;
                stored_value = cpu.mem_read(mem_addr);
                break;
            }
            
            case AddressingMode::Indirect_Y: {
                uint8_t base = cpu.mem_read(begin + 1);
                uint8_t lo = cpu.mem_read(base);
                uint8_t hi = cpu.mem_read(static_cast<uint8_t>(base + 1));
                uint16_t deref_base = (static_cast<uint16_t>(hi) << 8) | lo;
                mem_addr = deref_base + cpu.register_y;
                stored_value = cpu.mem_read(mem_addr);
                break;
            }
            default:
                break;
        }
    }
    
    std::string tmp;
    if (opcode->len == 1) {
        if (code == 0x0a || code == 0x4a || code == 0x2a || code == 0x6a) {
            tmp = "A ";  
        } else {
            tmp = "";
        }
    } else if (opcode->len == 2) {
        uint8_t address = cpu.mem_read(begin + 1);
        hex_dump.push_back(address);
        std::ostringstream oss;
        oss << std::uppercase << std::setfill('0') << std::hex;
        
        switch (opcode->mode) {
            case AddressingMode::Immediate:
                oss << "#$" << std::setw(2) << static_cast<int>(address);
                break;
                
            case AddressingMode::ZeroPage:
                oss << "$" << std::setw(2) << static_cast<int>(mem_addr) 
                    << " = " << std::setw(2) << static_cast<int>(stored_value);
                break;
                
            case AddressingMode::ZeroPage_X:
                oss << "$" << std::setw(2) << static_cast<int>(address) << ",X @ "
                    << std::setw(2) << static_cast<int>(mem_addr) << " = "
                    << std::setw(2) << static_cast<int>(stored_value);
                break;
                
            case AddressingMode::ZeroPage_Y:
                oss << "$" << std::setw(2) << static_cast<int>(address) << ",Y @ "
                    << std::setw(2) << static_cast<int>(mem_addr) << " = "
                    << std::setw(2) << static_cast<int>(stored_value);
                break;
                
            case AddressingMode::Indirect_X:
                oss << "($" << std::setw(2) << static_cast<int>(address) << ",X) @ "
                    << std::setw(2) << static_cast<int>(static_cast<uint8_t>(address + cpu.register_x))
                    << " = " << std::setw(4) << mem_addr << " = "
                    << std::setw(2) << static_cast<int>(stored_value);
                break;
                
            case AddressingMode::Indirect_Y:
                oss << "($" << std::setw(2) << static_cast<int>(address) << "),Y = "
                    << std::setw(4) << (mem_addr - cpu.register_y) << " @ "
                    << std::setw(4) << mem_addr << " = "
                    << std::setw(2) << static_cast<int>(stored_value);
                break;
                
            case AddressingMode::NoneAddressing: {
                oss << "$" << std::setw(4) << static_cast<int>(address);
                break;
            }
            default:
                break;
        }
        tmp = oss.str();
        
    } else if (opcode->len == 3) {
        uint8_t address_lo = cpu.mem_read(begin + 1);
        uint8_t address_hi = cpu.mem_read(begin + 2);
        hex_dump.push_back(address_lo);
        hex_dump.push_back(address_hi);
        uint16_t address = cpu.mem_read_u16(begin + 1);
        std::ostringstream oss;
        oss << std::uppercase << std::setfill('0') << std::hex;
        
        switch (opcode->mode) {
            case AddressingMode::NoneAddressing:
                if (code == 0x6c) {
                    oss << "($" << std::setw(4) << address << ")";
                } else {
                    oss << "$" << std::setw(4) << address;
                }
                break;
                
            case AddressingMode::Absolute:
                oss << "$" << std::setw(4) << mem_addr << " = "
                    << std::setw(2) << static_cast<int>(stored_value);
                break;
                
            case AddressingMode::Absolute_X:
                oss << "$" << std::setw(4) << address << ",X @ "
                    << std::setw(4) << mem_addr << " = "
                    << std::setw(2) << static_cast<int>(stored_value);
                break;
                
            case AddressingMode::Absolute_Y:
                oss << "$" << std::setw(4) << address << ",Y @ "
                    << std::setw(4) << mem_addr << " = "
                    << std::setw(2) << static_cast<int>(stored_value);
                break;

            default:
                break;
        }
        tmp = oss.str();
    }
    
    std::ostringstream hex_str;
    hex_str << std::uppercase << std::setfill('0') << std::hex;
    for (size_t i = 0; i < hex_dump.size(); i++) {
        if (i > 0) hex_str << " ";
        hex_str << std::setw(2) << static_cast<int>(hex_dump[i]);
    }
    
    std::ostringstream asm_str;
    asm_str << std::uppercase << std::setfill(' ') << std::hex
            << std::setw(4) << std::setfill('0') << begin << "  "
            << std::setw(8) << std::setfill(' ') << std::left << hex_str.str() << " "
            << std::setw(4) << opcode->mnemonic << " "
            << tmp;
    
    std::string asm_str_trimmed = asm_str.str();
    while (!asm_str_trimmed.empty() && asm_str_trimmed.back() == ' ') {
        asm_str_trimmed.pop_back();
    }
    
    std::ostringstream result;
    result << std::setfill(' ') << std::left << std::setw(47) << asm_str_trimmed << " "
           << std::uppercase << std::setfill('0') << std::hex << std::right
           << "A:" << std::setw(2) << static_cast<int>(cpu.register_a) << " "
           << "X:" << std::setw(2) << static_cast<int>(cpu.register_x) << " "
           << "Y:" << std::setw(2) << static_cast<int>(cpu.register_y) << " "
           << "P:" << std::setw(2) << static_cast<int>(cpu.status.bits) << " "
           << "SP:" << std::setw(2) << static_cast<int>(cpu.stack_pointer);
    
    return result.str();
}