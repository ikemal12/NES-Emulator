#include <SDL2/SDL.h>
#include "cartridge.h"
#include "render/renderer.h"
#include "cpu.h"
#include "bus.h"
#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_Window* window = SDL_CreateWindow(
        "NES Emulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        256 * 3,  
        240 * 3,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    SDL_Renderer* sdl_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!sdl_renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_RenderSetScale(sdl_renderer, 3.0f, 3.0f);
    SDL_Texture* texture = SDL_CreateTexture(
        sdl_renderer,
        SDL_PIXELFORMAT_RGB24, 
        SDL_TEXTUREACCESS_STREAMING,  
        256,  
        240   
    );
    if (!texture) {
        std::cerr << "Texture creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(sdl_renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    std::ifstream file("test/roms/nestest.nes", std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open nestest.nes" << std::endl;
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(sdl_renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    std::vector<uint8_t> rom_data((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());
    file.close();
    Rom cartridge = Rom::create(rom_data);
    std::cout << "Loaded ROM - PRG: " << cartridge.prg_rom.size() << " bytes, CHR:" << cartridge.chr_rom.size() << " bytes" << std::endl;
    Renderer nes_renderer;
    SDL_Event event;

    Bus bus(std::move(cartridge), [&](const NesPPU& ppu) {
        nes_renderer.render(ppu);
        const Frame& frame = nes_renderer.get_frame();
        SDL_UpdateTexture(texture, nullptr, frame.get_data(), 256 * 3);
        SDL_RenderClear(sdl_renderer);
        SDL_RenderCopy(sdl_renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(sdl_renderer);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                std::exit(0);
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                std::exit(0);
            }
        }
    });
    
    CPU cpu(std::move(bus));
    cpu.reset();
    cpu.run();
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
