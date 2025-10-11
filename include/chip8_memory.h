#pragma once
#include "constants.h"
#include <algorithm>
#include <array>
#include <cstdint>
#include <span>
#include <stdexcept>

namespace chip8 {

class Memory {
public:
  constexpr explicit Memory() noexcept : data_{} {
    std::copy(DEFAULT_CHAR_SET.begin(), DEFAULT_CHAR_SET.end(), data_.begin());
  };

  constexpr void write_byte(uint16_t addr, uint8_t val) {
    check_bounds(addr);
    data_[addr] = val;
  }

  [[nodiscard]] constexpr uint8_t read_byte(uint16_t addr) {
    check_bounds(addr);
    return data_[addr];
  }

  [[nodiscard]] constexpr uint16_t read_two_bytes(uint16_t addr) {
    check_bounds(addr + 1);
    return static_cast<uint16_t>(data_[addr]) << 8 |
           static_cast<uint16_t>(data_[addr + 1]);
  }

  [[nodiscard]] constexpr std::span<const uint8_t, MEMORY_SIZE>
  span() const noexcept {
    return {data_};
  }

private:
  constexpr void check_bounds(uint16_t addr) const {
    if (addr >= MEMORY_SIZE) {
      throw std::out_of_range("Memory access out of bounds\n");
    }
  }
  std::array<uint8_t, MEMORY_SIZE> data_;
};

} // namespace chip8