cmake_minimum_required(VERSION 3.18)
project(CHIP8-EMU VERSION 0.1 LANGUAGES CXX)

set(CXX_STANDARD 23)
set(CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)
find_package(GTEST REQUIRED)

add_executable(chip8 src/main.cpp)