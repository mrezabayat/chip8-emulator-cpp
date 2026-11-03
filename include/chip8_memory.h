#pragma once
#include "constants.h"
#include <cstdint>

namespace chip8 {

// Episode 2: build this class until every test in tests/test_memory.cpp passes.
//   Step 1: memory that boots with DEFAULT_CHAR_SET at the start  (CtorCreatesClearMemory)
//   Step 2: write_byte / read_byte, throwing past the end        (WriteAndReadBytes)
//   Step 3: read_two_bytes, high byte first                      (ReadTwoBytes, ReadTwoBytesAtLastAddressThrows)
// OutOfBoundAccessThrows passes once steps 2 and 3 are done.
class Memory {
public:
  constexpr explicit Memory() noexcept {}

  constexpr void write_byte(uint16_t addr, uint8_t val) {
    (void)addr;
    (void)val;
  }

  [[nodiscard]] constexpr uint8_t read_byte(uint16_t addr) {
    (void)addr;
    return 0;
  }

  [[nodiscard]] constexpr uint16_t read_two_bytes(uint16_t addr) {
    (void)addr;
    return 0;
  }
};

} // namespace chip8
