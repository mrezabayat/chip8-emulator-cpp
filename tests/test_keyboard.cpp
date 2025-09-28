#include "chip8_keyboard.h"
#include "constants.h"
#include "gtest/gtest.h"
#include <cstdint>
#include <stdexcept>

TEST(KeyboardTest, InitializedWithNoKeyPressed) {
  chip8::Keyboard keyboard;
  for (uint8_t key = 0; key < chip8::NUM_KEYS; ++key) {
    EXPECT_FALSE(keyboard.is_pressed(key));
  }
}

TEST(KeyboardTest, PressKeysAndCheckState) {
  chip8::Keyboard keyboard;
  for (uint8_t key = 0; key < chip8::NUM_KEYS; ++key) {
    keyboard.set_key_state(key, true);
    EXPECT_TRUE(keyboard.is_pressed(key));

    keyboard.set_key_state(key, false);
    EXPECT_FALSE(keyboard.is_pressed(key));
  }
}

TEST(KeyboardTest, OutOfRangeKeyThrows) {
  chip8::Keyboard keyboard;
  EXPECT_THROW(
      { auto _ = keyboard.is_pressed(chip8::NUM_KEYS); }, std::out_of_range);
  EXPECT_THROW(keyboard.set_key_state(chip8::NUM_KEYS, false),
               std::out_of_range);
}