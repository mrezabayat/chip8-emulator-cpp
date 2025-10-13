cmake_minimum_required(VERSION 3.18)
project(CHIP8-EMU VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

# --- CHIP8 Core lib ---
add_library(chip8_core STATIC 
    src/chip8_cpu.cpp
)
target_include_directories(chip8_core PUBLIC include)

# --- Main executable ---
add_executable(chip8 
    src/main.cpp
)
target_link_libraries(chip8 PRIVATE chip8_core SDL2::SDL2)

# --- Unit tests ---
enable_testing()

add_executable(chip8_tests 
    tests/test_memory.cpp
    tests/test_keyboard.cpp
    tests/test_sdl_input.cpp
    tests/test_timer.cpp
    tests/test_pcg_random.cpp
    tests/test_display.cpp
    tests/test_cpu.cpp
)
target_link_libraries(chip8_tests PRIVATE chip8_core GTest::gtest_main GTest::gmock SDL2::SDL2)

include(GoogleTest)
gtest_discover_tests(chip8_tests)
