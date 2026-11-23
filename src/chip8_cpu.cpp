#include "chip8_cpu.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>
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
  case 0x9000:
    execute_9(opcode);
    break;
  case 0xA000:
    execute_A(opcode);
    break;
  case 0xB000:
    execute_B(opcode);
    break;
  case 0xD000:
    execute_D(opcode);
    break;
  default:
    // The other families arrive in episodes 8 to 10; until then they do nothing.
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

void Cpu::execute_0(uint16_t opcode) {
  switch (opcode) {
  case 0x00E0:
    display_.get().clear();
    break;
  case 0x00EE:
    if (sp_ == 0) {
      throw std::runtime_error("Stack underflow");
    }
    --sp_;
    pc_ = stack_[sp_];
    break;
  default:
    std::cerr << "[Warning] 0nnn - SYS addr is ignored." << std::endl;
    break;
  }
}

void Cpu::execute_1(uint16_t opcode) noexcept { pc_ = opcode & 0x0FFF; }

void Cpu::execute_2(uint16_t opcode) {
  uint16_t nnn = opcode & 0x0FFF;
  if (sp_ >= NUM_CPU_STACK) {
    throw std::runtime_error("Stack overflow");
  }
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

void Cpu::execute_D(uint16_t opcode) noexcept {
  uint8_t x = (opcode >> 8) & 0x0F;
  uint8_t y = (opcode & 0x00F0) >> 4;
  uint8_t n = opcode & 0x000F;

  auto memory_span = memory_.get().span();
  if (I_ >= memory_span.size()) {
    v_[0x0F] = 0;
    return;
  }

  const auto available = std::min<std::size_t>(n, memory_span.size() - I_);
  if (available == 0) {
    v_[0x0F] = 0;
    return;
  }

  auto sprite = memory_span.subspan(I_, available);
  v_[0x0F] = display_.get().draw_sprite(v_[x], v_[y], sprite) ? 1 : 0;
}

} // namespace chip8
