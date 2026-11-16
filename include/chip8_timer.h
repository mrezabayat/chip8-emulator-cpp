#pragma once
#include <cstdint>
#include <stdexcept>

namespace chip8 {

// Episode 4, step 3: build this class until every test in tests/test_timer.cpp passes.
//   Two bytes, delay and sound. tick_60hz() counts each down by one, never below zero.
//   beep() is true while the sound timer is above zero.
class Timer {
public:
  explicit Timer() = default;

  void set_delay(uint8_t delay) {
    (void)delay;
    throw std::logic_error("Not built yet");
  }
  void set_sound(uint8_t sound) {
    (void)sound;
    throw std::logic_error("Not built yet");
  }

  [[nodiscard]] uint8_t delay() const { throw std::logic_error("Not built yet"); }
  [[nodiscard]] uint8_t sound() const { throw std::logic_error("Not built yet"); }

  void tick_60hz() { throw std::logic_error("Not built yet"); }

  [[nodiscard]] bool beep() const { throw std::logic_error("Not built yet"); }
};

} // namespace chip8
