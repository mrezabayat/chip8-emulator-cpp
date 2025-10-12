#pragma once
#include "chip8_irand_gen.h"
#include <cstdint>
#include <random>

namespace chip8 {

class PcgRandom : public RandomGenerator {
public:
  explicit PcgRandom(uint64_t seed = std::random_device{}()) noexcept
      : engine_(seed), dist_(0, 255) {}

  uint8_t next(uint8_t mask) override {
    return static_cast<uint8_t>(dist_(engine_) & mask);
  }

  void reseed(uint64_t seed) { engine_.seed(seed); }

private:
  std::mt19937_64 engine_;
  std::uniform_int_distribution<int> dist_;
};

} // namespace chip8