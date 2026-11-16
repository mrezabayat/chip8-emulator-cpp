#pragma once
#include "constants.h"
#include <array>
#include <cstdint>
#include <optional>
#include <stdexcept>

namespace chip8 {

// Episode 4: build this class until every test in tests/test_keyboard.cpp passes.
//   Step 1: sixteen keys you can press, release and check; a key past F throws std::out_of_range
//           (InitializedWithNoKeyPressed, PressKeysAndCheckState, OutOfRangeKeyThrows)
//   Step 2: remember the last key that went down, forget it when that key goes up
//           (LastPressedClearsWhenKeyReleased)
class Keyboard {
public:
  explicit Keyboard() = default;

  void set_key_state(uint8_t key, bool pressed) {
    (void)key;
    (void)pressed;
    throw std::logic_error("Not built yet");
  }

  [[nodiscard]] bool is_pressed(uint8_t key) const {
    (void)key;
    throw std::logic_error("Not built yet");
  }

  [[nodiscard]] std::optional<uint8_t> last_pressed() const {
    throw std::logic_error("Not built yet");
  }

  // The CPU calls this once it has used the last key (episode 10).
  void clear_last_pressed() { throw std::logic_error("Not built yet"); }
};

} // namespace chip8
