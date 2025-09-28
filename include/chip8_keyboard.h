#pragma once
#include "constants.h"
#include <array>
#include <cstdint>
#include <stdexcept>

namespace chip8 {

class Keyboard {
public:
  constexpr Keyboard() noexcept : keys_{} {}

  constexpr void set_key_state(uint8_t key, bool pressed) {
    check_key_bounds(key);
    keys_[key] = pressed;
  }

  [[nodiscard]] constexpr bool is_pressed(uint8_t key) const {
    check_key_bounds(key);
    return keys_[key];
  }

private:
  constexpr void check_key_bounds(uint8_t key) const {
    if (key >= NUM_KEYS) {
      throw std::out_of_range("Keyboard key out of range\n");
    }
  }

  std::array<bool, NUM_KEYS> keys_{};
};

} // namespace chip8