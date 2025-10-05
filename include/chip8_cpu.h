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
#include <span>

namespace chip8 {

class Cpu {
public:
  explicit constexpr Cpu(Memory &memory, Display &display, Keyboard &keyboard,
                         Timer &timer, PcgRandom &rng) noexcept
      : memory_(memory), display_(display), keyboard_(keyboard), timer_(timer),
        rng_(rng) {
    reset();
  }

  void execute();

  [[nodiscard]] constexpr uint16_t program_counter() const noexcept {
    return pc_;
  }
  [[nodiscard]] constexpr std::span<const uint8_t, NUM_CPU_REGISTERS>
  registers() const noexcept {
    return v_;
  }
  [[nodiscard]] constexpr uint16_t index_register() const noexcept {
    return I_;
  }

private:
  void reset() noexcept;

  void execute_0(uint16_t opcode) noexcept;
  void execute_1(uint16_t opcode) noexcept;
  void execute_2(uint16_t opcode) noexcept;
  void execute_3(uint16_t opcode) noexcept;
  void execute_4(uint16_t opcode) noexcept;
  void execute_5(uint16_t opcode) noexcept;
  void execute_6(uint16_t opcode) noexcept;
  void execute_7(uint16_t opcode) noexcept;
  void execute_8(uint16_t opcode) noexcept;
  void execute_9(uint16_t opcode) noexcept;

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
