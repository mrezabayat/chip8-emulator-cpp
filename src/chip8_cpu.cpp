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
  case 0x9000:
    execute_9(opcode);
    break;
  case 0xA000:
    execute_A(opcode);
    break;
  case 0xB000:
    execute_B(opcode);
    break;
  case 0xC000:
    execute_C(opcode);
    break;
  case 0xD000:
    execute_D(opcode);
    break;
  case 0xE000:
    execute_E(opcode);
    break;
  case 0xF000:
    execute_F(opcode);
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
  case 0x00EE:
    --sp_;
    pc_ = stack_[sp_];
    break;
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
  uint16_t temp{};

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
  case 0x03:
    v_[x] ^= v_[y];
    break;
  case 0x04:
    temp = v_[x] + v_[y];
    v_[0x0F] = (temp > 0xFFu) ? 0x01u : 0x00u;
    v_[x] = temp;
    break;
  case 0x05:
    v_[0x0F] = v_[x] < v_[y];
    v_[x] -= v_[y];
    break;
  case 0x06:
    v_[0x0F] = v_[x] & 0b00000001u;
    v_[x] >>= 1;
    break;
  case 0x07:
    v_[0x0F] = v_[x] > v_[y];
    v_[y] -= v_[x];
    break;
  case 0x0E:
    v_[0x0F] = (v_[x] & 0b10000000u) != 0x00u ? 0x01u : 0x00u;
    v_[x] <<= 1;
    break;
  default:
    break;
  }
}

void Cpu::execute_9(uint16_t opcode) noexcept {
  uint8_t x = (opcode >> 8) & 0x0F;
  uint8_t y = (opcode & 0x00F0) >> 4;
  if (v_[x] != v_[y]) {
    pc_ += 2;
  }
}

void Cpu::execute_A(uint16_t opcode) noexcept {
  uint16_t nnn = opcode & 0x0FFF;
  I_ = nnn;
}

void Cpu::execute_B(uint16_t opcode) noexcept {
  uint16_t nnn = opcode & 0x0FFF;
  pc_ = v_[0x00] + nnn;
}

void Cpu::execute_C(uint16_t opcode) noexcept {
  uint8_t x = (opcode >> 8) & 0x0F;
  uint8_t kk = opcode & 0x00FF;
  v_[x] = rng_.get().next(kk);
}

void Cpu::execute_D(uint16_t opcode) noexcept {
  uint8_t x = (opcode >> 8) & 0x0F;
  uint8_t y = (opcode & 0x00F0) >> 4;
  uint8_t n = opcode & 0x000F;

  auto sprite = memory_.get().span().subspan(I_, n);
  v_[0x0F] = display_.get().draw_sprite(v_[x], v_[y], sprite) ? 1 : 0;
}

void Cpu::execute_E(uint16_t opcode) noexcept {
  uint8_t x = (opcode >> 8) & 0x0F;
  uint8_t kk = opcode & 0x00FF;

  switch (kk) {
  case 0x9E:
    if (keyboard_.get().is_pressed(v_[x])) {
      pc_ += 2;
    }
    break;

  case 0xA1:
    if (!keyboard_.get().is_pressed(v_[x])) {
      pc_ += 2;
    }
    break;
  default:
    break;
  }
}

void Cpu::execute_F(uint16_t opcode) noexcept {
  uint8_t x = (opcode >> 8) & 0x0F;
  uint8_t kk = opcode & 0x00FF;

  switch (kk) {
  case 0x07:
    v_[x] = timer_.get().delay();
    break;
  case 0x0A: {
    auto key = keyboard_.get().last_pressed();
    if (key.has_value()) {
      v_[x] = key.value();
      keyboard_.get().clear_last_pressed();
    } else {
      pc_ -= 2;
    }
    break;
  }
  default:
    break;
  }
}

} // namespace chip8