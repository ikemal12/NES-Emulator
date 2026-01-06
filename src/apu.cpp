#include "apu.h"

PulseChannel::PulseChannel() 
    : duty_cycle(0), length_counter_halt(false), constant_volume(false),
      volume(0), sweep_enabled(false), sweep_period(0), sweep_negate(false),
      sweep_shift(0), timer_period(0), timer_value(0), length_counter(0),
      enabled(false), duty_sequence(0), envelope_divider(0), 
      envelope_counter(0), envelope_start(false) {
}

void PulseChannel::write_register(uint8_t reg, uint8_t value) {
    switch(reg) {
        case 0: 
            duty_cycle = (value >> 6) & 0x03;
            length_counter_halt = (value & 0x20) != 0;
            constant_volume = (value & 0x10) != 0;
            volume = value & 0x0F;
            envelope_start = true;
            break;
        case 1: 
            sweep_enabled = (value & 0x80) != 0;
            sweep_period = (value >> 4) & 0x07;
            sweep_negate = (value & 0x08) != 0;
            sweep_shift = value & 0x07;
            break;
        case 2:
            timer_period = (timer_period & 0x0700) | value;
            break;
        case 3: 
            timer_period = (timer_period & 0x00FF) | ((value & 0x07) << 8);
            static const uint8_t length_table[32] = {
                10, 254, 20, 2, 40, 4, 80, 6, 160, 8, 60, 10, 14, 12, 26, 14,
                12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
            };
            length_counter = length_table[value >> 3];
            duty_sequence = 0;  
            envelope_start = true;
            break;
    }
}

void PulseChannel::clock_timer() {
    if (timer_value == 0) {
        timer_value = timer_period;
        duty_sequence = (duty_sequence + 1) % 8;
    } else {
        timer_value--;
    }
}

float PulseChannel::output() {
    if (!enabled || length_counter == 0) {
        return 0.0f;
    }
    if (timer_period < 8) {
        return 0.0f;
    }
    static const uint8_t duty_table[4] = {
        0b01000000, 
        0b01100000,  
        0b01111000,  
        0b10011111  
    };
    uint8_t pattern = duty_table[duty_cycle];
    bool waveform_high = (pattern >> (7 - duty_sequence)) & 1;
    if (!waveform_high) {
        return 0.0f;
    }
    uint8_t vol = constant_volume ? volume : envelope_counter;
    return vol / 15.0f; 
}

void PulseChannel::clock_envelope() {
    if (envelope_start) {
        envelope_start = false;
        envelope_counter = 15;
        envelope_divider = volume;
    } else if (envelope_divider > 0) {
        envelope_divider--;
    } else {
        envelope_divider = volume;
        if (envelope_counter > 0) {
            envelope_counter--;
        } else if (length_counter_halt) {
            envelope_counter = 15; 
        }
    }
}

void PulseChannel::clock_length_counter() {
    if (length_counter > 0 && !length_counter_halt) {
        length_counter--;
    }
}