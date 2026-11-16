#pragma once
#include <cstdint>

namespace chip8 {

class RandomGenerator {
public:
  virtual ~RandomGenerator() = default;
  virtual uint8_t next(uint8_t mask) = 0;
};

} // namespace chip8