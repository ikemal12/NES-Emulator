#include <iostream>
#include <SDL2/SDL.h>

int main(int argc, char* argv[]) {
    std::cout << "Hello from C++ NES Emulator!" << std::endl;
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL2 initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }
    std::cout << "SDL2 initialized successfully!" << std::endl;
    SDL_Quit();
    return 0;
}