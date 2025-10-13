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
#include <filesystem>
#include <fstream>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

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
    cpu_.reset();
    state_ = EmulatorState::Stopped;
  }

  void set_cycles_per_frame(uint32_t cycles) noexcept {
    cycles_per_frame_ = cycles;
  }

  void load_rom(std::span<const uint8_t> rom) {
    if (rom.empty()) {
      throw std::invalid_argument("ROM data is empty.");
    }

    if (rom.size() > MEMORY_SIZE - START_ADDRESS) {
      throw std::runtime_error("ROM size exceeds memory capacity.");
    }

    reset();

    for (std::size_t i = 0; i < rom.size(); ++i) {
      auto address =
          static_cast<uint16_t>(START_ADDRESS + static_cast<uint16_t>(i));
      memory_.write_byte(address, rom[i]);
    }

    start();
  }

  void load_rom(const std::filesystem::path &rom_path) {
    std::ifstream rom_file(rom_path, std::ios::binary | std::ios::ate);
    if (!rom_file) {
      throw std::runtime_error("Failed to open ROM file: " + rom_path.string());
    }

    const std::streamsize file_size = rom_file.tellg();
    if (file_size <= 0) {
      throw std::runtime_error("ROM file is empty: " + rom_path.string());
    }

    std::vector<uint8_t> buffer(static_cast<std::size_t>(file_size));
    rom_file.seekg(0, std::ios::beg);
    if (!rom_file.read(reinterpret_cast<char *>(buffer.data()),
                       buffer.size())) {
      throw std::runtime_error("Failed to read ROM file: " + rom_path.string());
    }

    load_rom(std::span<const uint8_t>(buffer));
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

  [[nodiscard]] constexpr Keyboard &keyboard() noexcept { return Keyboard_; }

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
