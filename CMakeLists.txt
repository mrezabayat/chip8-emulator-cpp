cmake_minimum_required(VERSION 3.18)
project(CHIP8-EMU VERSION 0.1 LANGUAGES CXX)

set(CXX_STANDARD 23)
set(CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)
find_package(GTEST REQUIRED)

# --- Main executable ---
add_executable(chip8 src/main.cpp)
target_include_directories(chip8 PRIVATE include)

# --- Unit tests ---
enable_testing()

add_executable(chip8_tests 
    tests/test_memory.cpp
    tests/test_keyboard.cpp
    tests/test_sdl_input.cpp
    tests/test_timer.cpp
)

target_include_directories(chip8_tests PRIVATE include)
target_link_libraries(chip8_tests PRIVATE GTest::gtest_main SDL2::SDL2)

include(GoogleTest)
gtest_discover_tests(chip8_tests)