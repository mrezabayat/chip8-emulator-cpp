#include "chip8_cpu.h"
#include <iostream>
#include <print>
namespace chip8 {

void Cpu::execute() {
  const auto opcode = memory_.get().read_two_bytes(pc_);
  pc_ += 2;

  switch (opcode & 0xF000) {
  case 0x0000:
    execute_0(opcode);
    break;
  case 0x1000:
    execute_1(opcode);
    break;
  case 0x2000:
    execute_2(opcode);
    break;
  case 0x3000:
    execute_3(opcode);
    break;
  case 0x4000:
    execute_4(opcode);
    break;
  case 0x5000:
    execute_5(opcode);
    break;
  case 0x6000:
    execute_6(opcode);
    break;
  case 0x7000:
    execute_7(opcode);
    break;
  case 0x8000:
    execute_8(opcode);
    break;
  default:
    break;
  }
}

void Cpu::reset() noexcept {
  stack_.fill(0);
  v_.fill(0);
  I_ = 0;
  sp_ = 0;
  pc_ = START_ADDRESS;
}

void Cpu::execute_0(uint16_t opcode) noexcept {
  switch (opcode) {
  case 0x00E0:
    display_.get().clear();
    break;

  case 0x00EE: {
    --sp_;
    pc_ = stack_[sp_];
  } break;
  default:
    std::print(std::cerr, "[Warning] 0nnn - SYS addr is ignored.\n");
    break;
  }
}

void Cpu::execute_1(uint16_t opcode) noexcept { pc_ = opcode & 0x0FFF; }

void Cpu::execute_2(uint16_t opcode) noexcept {
  uint16_t nnn = opcode & 0x0FFF;
  stack_[sp_] = pc_;
  ++sp_;
  pc_ = nnn;
}

void Cpu::execute_3(uint16_t opcode) noexcept {
  uint8_t x = (opcode >> 8) & 0x0F;
  uint8_t kk = opcode & 0x00FF;
  if (v_[x] == kk) {
    pc_ += 2;
  }
}

void Cpu::execute_4(uint16_t opcode) noexcept {
  uint8_t x = (opcode >> 8) & 0x0F;
  uint8_t kk = opcode & 0x00FF;
  if (v_[x] != kk) {
    pc_ += 2;
  }
}

void Cpu::execute_5(uint16_t opcode) noexcept {
  uint8_t x = (opcode >> 8) & 0x0F;
  uint8_t y = (opcode & 0x00F0) >> 4;
  if (v_[x] == v_[y]) {
    pc_ += 2;
  }
}

void Cpu::execute_6(uint16_t opcode) noexcept {
  uint8_t x = (opcode >> 8) & 0x0F;
  uint8_t kk = opcode & 0x00FF;
  v_[x] = kk;
}

void Cpu::execute_7(uint16_t opcode) noexcept {
  uint8_t x = (opcode >> 8) & 0x0F;
  uint8_t kk = opcode & 0x00FF;
  v_[x] += kk;
}

void Cpu::execute_8(uint16_t opcode) noexcept {
  uint8_t x = (opcode >> 8) & 0x0F;
  uint8_t y = (opcode & 0x00F0) >> 4;
  uint8_t n = opcode & 0x000F;

  switch (n) {
  case 0x00:
    v_[x] = v_[y];
    break;
  case 0x01:
    v_[x] |= v_[y];
    break;
  case 0x02:
    v_[x] &= v_[y];
    break;
  default:
    break;
  }
}

} // namespace chip8