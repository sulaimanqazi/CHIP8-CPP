cmake_minimum_required(VERSION 3.25)
project(CHIP8)

set(CMAKE_CXX_STANDARD 14)

set(SDL2_INCLUDE_DIR C:/Users/sulaiman/Downloads/SDL2-devel-2.0.18-VC/SDL2-2.0.18/include)
set(SDL2_LIB_DIR C:/Users/sulaiman/Downloads/SDL2-devel-2.0.18-VC/SDL2-2.0.18/lib/x64)
include_directories(${SDL2_INCLUDE_DIR})
link_directories(${SDL2_LIB_DIR})

add_executable(CHIP8
        CHIP8.cpp
        CHIP8.hpp
        Platform.hpp)

target_link_libraries(${PROJECT_NAME} SDL2main SDL2)