#ifndef APU_H
#define APU_H

#include <cstdint>
#include <array>

class PulseChannel {
public:
    uint8_t duty_cycle;        
    bool length_counter_halt;  
    bool constant_volume;
    uint8_t volume;           
    bool sweep_enabled;
    uint8_t sweep_period;
    bool sweep_negate;
    uint8_t sweep_shift;
    uint16_t timer_period;    
    uint16_t timer_value;
    uint8_t length_counter;
    bool enabled;
    uint8_t duty_sequence;    
    uint8_t envelope_divider;
    uint8_t envelope_counter;
    bool envelope_start;
    
    PulseChannel();
    void write_register(uint8_t reg, uint8_t value); 
    float output();  
    void clock_timer();
    void clock_length_counter();
    void clock_envelope();
    void clock_sweep(bool is_pulse1); 
};

class APU {
public:
    PulseChannel pulse1;
    PulseChannel pulse2;
    uint64_t cycles;  
    
    APU();
    void write_register(uint16_t addr, uint8_t value);
    uint8_t read_register(uint16_t addr);
    void clock();  
    float output();  
};

#endif // APU_H