#pragma once
#include <cstdint>

namespace chip8 {
inline constexpr uint16_t MEMORY_SIZE = 4096;
inline constexpr uint8_t NUM_KEYS = 16;
inline constexpr uint8_t SCREEN_WIDTH = 64;
inline constexpr uint8_t SCREEN_HEIGHT = 32;
inline constexpr uint16_t START_ADDRESS = 0x200;
inline constexpr uint8_t NUM_CPU_STACK = 16;
inline constexpr uint8_t NUM_CPU_REGISTERS = 16;
} // namespace chip8
