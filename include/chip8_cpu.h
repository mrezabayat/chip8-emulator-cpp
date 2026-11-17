#pragma once
#include "chip8_display.h"
#include "chip8_irand_gen.h"
#include "chip8_keyboard.h"
#include "chip8_memory.h"
#include "chip8_timer.h"
#include "constants.h"
#include <array>
#include <cstdint>
#include <span>
#include <stdexcept>

namespace chip8 {

// Episode 5: build this class until every test in tests/test_cpu.cpp passes.
//   Step 1: the CPU's state (V0-VF, I, PC, SP, the stack), references to the parts it
//           drives, and reset(): PC starts at START_ADDRESS (StartsAtTheProgramAddress)
//   Step 2: execute(): fetch two bytes at PC, move PC on, dispatch on the top nibble;
//           first instruction 1nnn, jump (JPInstructionSetsProgramCounterToNNN)
//   Step 3: 00E0, clear the screen (CLSInstrcutionClearsScreen)
//   Step 4: 6xkk, 7xkk and Annn (LDPutsValueKKInRegisterX,
//           ADD_VxByte_AddsImmediateToRegister, LD_I_SetsValueOfRegisterIToNNN)
class Cpu {
public:
  explicit Cpu(Memory &memory, Display &display, Keyboard &keyboard,
               Timer &timer, RandomGenerator &rng) {
    (void)memory;
    (void)display;
    (void)keyboard;
    (void)timer;
    (void)rng;
  }

  void execute();

  [[nodiscard]] uint16_t program_counter() const {
    throw std::logic_error("Not built yet");
  }
  [[nodiscard]] std::span<const uint8_t, NUM_CPU_REGISTERS> registers() const {
    throw std::logic_error("Not built yet");
  }
  [[nodiscard]] uint16_t index_register() const {
    throw std::logic_error("Not built yet");
  }
};

} // namespace chip8
