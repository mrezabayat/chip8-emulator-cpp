#include "chip8_irand_gen.h"
#include "gmock/gmock.h"

class MockRandom : public chip8::RandomGenerator {
public:
  MOCK_METHOD(uint8_t, next, (uint8_t mask), (override));
};
