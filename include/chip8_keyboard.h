#pragma once
#include "constants.h"
#include <array>
#include <cstdint>
#include <optional>
#include <stdexcept>

namespace chip8 {

class Keyboard {
public:
  constexpr explicit Keyboard() noexcept : keys_{} {}

  constexpr void set_key_state(uint8_t key, bool pressed) {
    check_key_bounds(key);
    keys_[key] = pressed;
    if (pressed) {
      last_pressed_ = key;
    }
  }

  [[nodiscard]] constexpr bool is_pressed(uint8_t key) const {
    check_key_bounds(key);
    return keys_[key];
  }

  [[nodiscard]] constexpr std::optional<uint8_t> last_pressed() const noexcept {
    return last_pressed_;
  }

  // Clear last_pressed_ after being consumed by Fx instructions
  constexpr void clear_last_pressed() noexcept { last_pressed_.reset(); }

private:
  constexpr void check_key_bounds(uint8_t key) const {
    if (key >= NUM_KEYS) {
      throw std::out_of_range("Keyboard key out of range\n");
    }
  }

  std::array<bool, NUM_KEYS> keys_{};
  std::optional<uint8_t> last_pressed_{};
};

} // namespace chip8