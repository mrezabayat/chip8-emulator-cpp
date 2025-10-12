#pragma once
#include "chip8_cpu.h"
#include "chip8_display.h"
#include "chip8_keyboard.h"
#include "chip8_memory.h"
#include "chip8_pcg_rand.h"
#include "chip8_timer.h"
#include "constants.h"
#include "emulator_types.h"
#include <cstdint>

namespace chip8 {

class Emulator {
public:
  explicit Emulator(uint32_t cycles_per_frame = 10)
      : rng_{0xC0FFEEu}, cpu_{memory_, display_, Keyboard_, timers_, rng_},
        cycles_per_frame_{cycles_per_frame}, state_{EmulatorState::Stopped} {}

  // Core lifecycle
  void start() noexcept { state_ = EmulatorState::Running; }
  void pause() noexcept { state_ = EmulatorState::Paused; }
  void stop() noexcept {
    state_ = EmulatorState::Stopped;
    reset();
  }

  void reset() noexcept {
    memory_ = Memory{};
    display_.clear();
    Keyboard_ = Keyboard{};
    timers_ = Timer{};
    rng_.reseed(0xC0FFEEu);
    state_ = EmulatorState::Stopped;
  }

  void set_cycles_per_frame(uint32_t cycles) noexcept {
    cycles_per_frame_ = cycles;
  }

  // Executes a single frame worth of cycles
  RunFrameResult run_frame(uint32_t cycles_override = 0) {
    if (state_ != EmulatorState::Running)
      return {};

    uint32_t cycles_to_run =
        cycles_override > 0 ? cycles_override : cycles_per_frame_;

    for (uint32_t i = 0; i < cycles_to_run; ++i)
      cpu_.execute();

    // Tick timers once per frame (typically 60 Hz)
    tick_timers(1);

    return {
        .frame_complete = true,
        .sound_active = timers_.beep(),
    };
  }

  void tick_timers(uint32_t ticks = 1) noexcept {
    for (uint32_t i = 0; i < ticks; ++i)
      timers_.tick_60hz();
  }

  [[nodiscard]] constexpr EmulatorState state() const noexcept {
    return state_;
  }

  [[nodiscard]] constexpr Display const &display() const noexcept {
    return display_;
  }

  [[nodiscard]] constexpr Keyboard &Keyboard() noexcept { return Keyboard_; }

private:
  Memory memory_;
  Display display_;
  Keyboard Keyboard_;
  Timer timers_;
  PcgRandom rng_;
  Cpu cpu_;

  EmulatorState state_;
  uint32_t cycles_per_frame_;
};

} // namespace chip8
