cmake_minimum_required(VERSION 3.18)
project(CHIP8-EMU VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest CONFIG REQUIRED)

# --- CHIP8 Core lib ---
# Header-only for now; the CPU's source file joins it in episode 5.
add_library(chip8_core INTERFACE)
target_include_directories(chip8_core INTERFACE include)

# --- Unit tests ---
# Each episode adds its test file here.
enable_testing()

add_executable(chip8_tests
    tests/test_memory.cpp
)
target_link_libraries(chip8_tests PRIVATE chip8_core GTest::gtest_main GTest::gmock)

include(GoogleTest)
gtest_discover_tests(chip8_tests)
