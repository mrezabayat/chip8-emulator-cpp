#pragma once
#include "chip8_display.h"
#include "chip8_keyboard.h"
#include "chip8_memory.h"
#include "chip8_pcg_rand.h"
#include "chip8_timer.h"
#include "constants.h"
#include <array>
#include <cstdint>
#include <functional>

namespace chip8 {

class Cpu {
public:
  explicit Cpu(Memory &memory, Display &display, Keyboard &keyboard,
               Timer &timer, PcgRandom &rng) noexcept
      : memory_(memory), display_(display), keyboard_(keyboard), timer_(timer),
        rng_(rng) {
    reset();
  }

  void execute();

private:
  void reset() noexcept {
    stack_.fill(0);
    v_.fill(0);
    I_ = 0;
    sp_ = 0;
    pc_ = START_ADDRESS;
  }

  void execute_0(uint16_t opcode);

  std::reference_wrapper<Memory> memory_;
  std::reference_wrapper<Display> display_;
  std::reference_wrapper<Keyboard> keyboard_;
  std::reference_wrapper<Timer> timer_;
  std::reference_wrapper<PcgRandom> rng_;

  std::array<uint16_t, NUM_CPU_STACK> stack_{};
  std::array<uint8_t, NUM_CPU_REGISTERS> v_{};
  uint16_t I_{};
  uint8_t sp_{};
  uint16_t pc_{};
};

} // namespace chip8
