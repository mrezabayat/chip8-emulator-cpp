#include "chip8_cpu.h"
#include "chip8_display.h"
#include "chip8_keyboard.h"
#include "chip8_memory.h"
#include "chip8_pcg_rand.h"
#include "chip8_timer.h"
#include "constants.h"
#include "gtest/gtest.h"

class CpuTest : public ::testing::Test {
protected:
  chip8::Memory memory;
  chip8::Display display;
  chip8::Keyboard keyboard;
  chip8::Timer timer;
  chip8::PcgRandom rng;
  chip8::Cpu cpu{memory, display, keyboard, timer, rng};
};

TEST_F(CpuTest, CLSInstrcutionClearsScreen) {
  for (uint8_t i = 0; i < chip8::SCREEN_WIDTH; ++i) {
    for (uint8_t j = 0; j < chip8::SCREEN_HEIGHT; ++j) {
      display.set_pixel(i, j, true);
    }
  }

  memory.write_byte(chip8::START_ADDRESS, 0x00u);
  memory.write_byte(chip8::START_ADDRESS + 1, 0xE0u);

  cpu.execute();

  for (int i = 0; i < chip8::SCREEN_WIDTH; ++i) {
    for (int j = 0; j < chip8::SCREEN_HEIGHT; ++j) {
      EXPECT_FALSE(display.is_pixel_set(i, j));
    }
  }
}