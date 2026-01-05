#include "cartridge.h"
#include "bus.h"
#include "cpu.h"
#include "trace.h"
#include <iostream>
#include <fstream>
#include <vector>

std::vector<uint8_t> read_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw std::runtime_error("Could not read file: " + filename);
    }
    return buffer;
}

int main() {
    std::cout << "Hello from C++ NES Emulator!" << std::endl;
    try {
        std::cout << "Loading nestest.nes...\n";
        std::vector<uint8_t> rom_data = read_file("../test/roms/nestest.nes");
        Rom rom = Rom::create(rom_data);
        std::cout << "ROM loaded successfully!\n";
        std::cout << "  PRG-ROM: " << rom.prg_rom.size() << " bytes\n";
        std::cout << "  CHR-ROM: " << rom.chr_rom.size() << " bytes\n";
        std::cout << "  Mapper: " << static_cast<int>(rom.mapper) << "\n\n";

        std::cout << "Creating Bus and PPU...\n";
        Bus bus(std::move(rom), [](const NesPPU&, Joypad&) {});
        std::cout << "Bus created successfully!\n\n";
        
        std::cout << "Creating CPU...\n";
        CPU cpu(std::move(bus));
        std::cout << "CPU created successfully!\n\n";
        
        std::cout << "Resetting CPU...\n";
        cpu.reset();
        cpu.program_counter = 0xC000;  
        std::cout << "CPU reset complete!\n";
        std::cout << "  Program Counter: 0x" << std::hex << cpu.program_counter << std::dec << "\n\n";
        
        std::cout << "Running CPU with trace output:\n";
        std::cout << "=================================\n";
        int instruction_count = 0;
        bool printed_vblank = false;
        cpu.run_with_callback([&instruction_count, &printed_vblank](CPU& cpu) {
            if (instruction_count < 50 || (instruction_count >= 200 && instruction_count < 300)) {
                std::cout << trace(cpu) << "\n";
            }
            if (!printed_vblank && cpu.bus.ppu->status.is_in_vblank()) {
                std::cout << "\n*** VBlank Started! Scanline=" << cpu.bus.ppu->scanline << " ***\n\n";
                printed_vblank = true;
            }
            instruction_count++;
            
            if (instruction_count >= 500) {
                std::cout << "\nReached 500 instructions. Final state:\n";
                std::cout << "  Scanline: " << cpu.bus.ppu->scanline << "\n";
                std::cout << "  VBlank: " << (cpu.bus.ppu->status.is_in_vblank() ? "YES" : "NO") << "\n";
                std::cout << "  PC: 0x" << std::hex << cpu.program_counter << std::dec << "\n";
                std::exit(0); 
            }
        });
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}