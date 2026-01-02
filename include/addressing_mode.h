#ifndef ADDRESSING_MODE_H
#define ADDRESSING_MODE_H

enum class AddressingMode {
    Immediate,      
    ZeroPage,       
    ZeroPage_X,     
    ZeroPage_Y,     
    Absolute,       
    Absolute_X,     
    Absolute_Y,    
    Indirect_X,     
    Indirect_Y,     
    NoneAddressing  
};

#endif // ADDRESSING_MODE_H