#include "chip8_cpu.h"

namespace chip8 {

void Cpu::execute() {
  const auto opcode = memory_.get().read_two_bytes(pc_);
  pc_ += 2;

  switch (opcode & 0xF000) {
  case 0x0000:
    execute_0(opcode);
    break;

  default:
    break;
  }
}

void Cpu::execute_0(uint16_t opcode) {
  switch (opcode) {
  case 0x00E0:
    display_.get().clear();
    break;

  default:
    break;
  }
}

} // namespace chip8