#include "chip8_cpu.h"
#include <algorithm>
#include <iostream>
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
  case 0x6000:
    execute_6(opcode);
    break;
  case 0x7000:
    execute_7(opcode);
    break;
  case 0xA000:
    execute_A(opcode);
    break;
  case 0xD000:
    execute_D(opcode);
    break;
  default:
    // The other families arrive in episodes 7 to 10; until then they do nothing.
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
  default:
    std::cerr << "[Warning] 0nnn - SYS addr is ignored." << std::endl;
    break;
  }
}

void Cpu::execute_1(uint16_t opcode) noexcept { pc_ = opcode & 0x0FFF; }

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

void Cpu::execute_A(uint16_t opcode) noexcept {
  uint16_t nnn = opcode & 0x0FFF;
  I_ = nnn;
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
