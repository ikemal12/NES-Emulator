cmake_minimum_required(VERSION 3.15)
project(nes_emu VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

find_package(SDL2 REQUIRED)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${SDL2_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCES 
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

if(SOURCES)
    add_executable(nes_emu ${SOURCES})
    target_link_libraries(nes_emu ${SDL2_LIBRARIES})
endif()