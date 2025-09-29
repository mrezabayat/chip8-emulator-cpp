#pragma once
#include <cstdint>

namespace chip8 {

class Timer {
public:
  constexpr Timer() noexcept {}

  constexpr void set_delay(uint8_t delay) { delay_ = delay; }
  constexpr void set_sound(uint8_t sound) { sound_ = sound; }

  constexpr uint8_t delay() const { return delay_; }
  constexpr uint8_t sound() const { return sound_; }

  constexpr void tick_60hz() {
    if (delay_ > 0)
      --delay_;
    if (sound_ > 0)
      --sound_;
  }

  constexpr bool beep() const { return sound_ > 0; }

private:
  uint8_t delay_{};
  uint8_t sound_{};
};

} // namespace chip8