#include "chip8_pcg_rand.h"
#include "gtest/gtest.h"

TEST(PcgRandomTest, NextAlwaysWithingMask) {
  chip8::PcgRandom rng{1234};
  uint8_t mask = 0x0F;
  for (int i = 0; i < 100; ++i) {
    EXPECT_LE(rng.next(mask), mask);
  }
}

TEST(PcgRandomTest, ReSeedProducesDifferentResults) {
  chip8::PcgRandom rng1{1234};
  chip8::PcgRandom rng2{4321};
  uint8_t mask = 0xFF;

  EXPECT_NE(rng1.next(mask), rng2.next(mask));
}

TEST(PcgRandomTest, DeterministicResultsWithSameSeed) {
  chip8::PcgRandom rng1{567};
  chip8::PcgRandom rng2{567};
  uint8_t mask = 0xFF;

  for (int i = 0; i < 100; ++i) {
    EXPECT_EQ(rng1.next(mask), rng2.next(mask));
  }
}
