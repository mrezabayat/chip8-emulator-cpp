#pragma once
#include <cstdint>
#include <stdexcept>

namespace chip8 {

// Episode 4, step 4: build this class until every test in tests/test_pcg_random.cpp passes.
//   First write the interface it implements: a RandomGenerator class in include/chip8_irand_gen.h
//   with one pure virtual function, uint8_t next(uint8_t mask).
//   Then PcgRandom: seeded in its constructor, reseed() changes the seed later, and next(mask)
//   returns a random byte ANDed with the mask. The standard <random> library will help.
class PcgRandom {
public:
  explicit PcgRandom(uint64_t seed = 0) { (void)seed; }

  uint8_t next(uint8_t mask) {
    (void)mask;
    throw std::logic_error("Not built yet");
  }

  void reseed(uint64_t seed) {
    (void)seed;
    throw std::logic_error("Not built yet");
  }
};

} // namespace chip8
