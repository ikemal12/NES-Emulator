cmake_minimum_required(VERSION 3.15)
project(nes_emu)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

file(GLOB_RECURSE LIB_SOURCES 
    "src/*.cpp"
    "src/ppu/*.cpp"
    "src/ppu/registers/*.cpp"
)

list(FILTER LIB_SOURCES EXCLUDE REGEX ".*main\\.cpp$")
add_library(nes-emu-core STATIC ${LIB_SOURCES})

add_executable(nestest-runner
    test/nestest_runner.cpp
    test/trace.cpp
)
target_link_libraries(nestest-runner nes-emu-core)