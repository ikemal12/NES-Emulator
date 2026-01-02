#include <SDL2/SDL.h>
#include "cartridge.h"
#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char* argv[]) {
    std::cout << "Hello from C++ NES Emulator!" << std::endl;
    
    try {
        std::ifstream file("nestest.nes", std::ios::binary | std::ios::ate);
        if (!file) {
            std::cerr << "Error: Failed to open nestest.nes" << std::endl;
            return 1;
        }
        
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::vector<uint8_t> buffer(size);
        if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
            std::cerr << "Error: Failed to read file" << std::endl;
            return 1;
        }
        
        std::cout << "File size: " << size << " bytes" << std::endl;
        Rom rom = Rom::create(buffer);
        std::cout << "\n✓ ROM loaded successfully!" << std::endl;
        std::cout << "  PRG-ROM size: " << rom.prg_rom.size() << " bytes" << std::endl;
        std::cout << "  CHR-ROM size: " << rom.chr_rom.size() << " bytes" << std::endl;
        std::cout << "  Mapper: " << static_cast<int>(rom.mapper) << std::endl;
        std::cout << "  Mirroring: ";
        
        switch (rom.screen_mirroring) {
            case Mirroring::Horizontal: 
                std::cout << "Horizontal" << std::endl; 
                break;
            case Mirroring::Vertical: 
                std::cout << "Vertical" << std::endl; 
                break;
            case Mirroring::FourScreen: 
                std::cout << "FourScreen" << std::endl; 
                break;
        }
        
    } catch (const RomParseError& e) {
        std::cerr << "\nROM Parse Error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "\nUnexpected error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}