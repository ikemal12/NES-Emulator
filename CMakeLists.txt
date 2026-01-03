cmake_minimum_required(VERSION 3.15)
project(nes_emu)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/ppu/*.cpp"
    "src/ppu/registers/*.cpp"
)

add_executable(nes_emu ${SOURCES})