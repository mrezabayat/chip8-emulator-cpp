#include "chip8_cpu.h"
#include "chip8_display.h"
#include "chip8_keyboard.h"
#include "chip8_memory.h"
#include "chip8_timer.h"
#include "constants.h"
#include "mock_rng.h"
#include "gtest/gtest.h"

class CpuTest : public ::testing::Test {
protected:
  chip8::Memory memory;
  chip8::Display display;
  chip8::Keyboard keyboard;
  chip8::Timer timer;
  MockRandom rng;
  chip8::Cpu cpu{memory, display, keyboard, timer, rng};
};

TEST_F(CpuTest, StartsAtTheProgramAddress) {
  EXPECT_EQ(cpu.program_counter(), chip8::START_ADDRESS);
  EXPECT_EQ(cpu.index_register(), 0x0000u);
  for (auto v : cpu.registers()) {
    EXPECT_EQ(v, 0x00u);
  }
}

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

TEST_F(CpuTest, JPInstructionSetsProgramCounterToNNN) {
  memory.write_byte(0x200, 0x12u);
  memory.write_byte(0x201, 0x34u);

  cpu.execute();

  EXPECT_EQ(cpu.program_counter(), 0x0234u);
}

TEST_F(CpuTest, LDPutsValueKKInRegisterX) {
  memory.write_byte(0x200, 0x65u);
  memory.write_byte(0x201, 0xFAu);

  cpu.execute();

  EXPECT_EQ(cpu.registers()[0x05], 0xFAu);
}

TEST_F(CpuTest, ADD_VxByte_AddsImmediateToRegister) {
  // Arrange: set v[0x03] = 0x23
  uint8_t first = 0x23;
  uint8_t second = 0x45;
  memory.write_byte(0x200, 0x63u);
  memory.write_byte(0x201, first);
  cpu.execute();

  // Act: add 0x45 to v[0x03]
  memory.write_byte(0x202, 0x73);
  memory.write_byte(0x203, second);
  cpu.execute();

  // Assert
  EXPECT_EQ(cpu.registers()[0x03], first + second);
}

TEST_F(CpuTest, LD_I_SetsValueOfRegisterIToNNN) {
  // Arrange
  memory.write_byte(0x200, 0xAF);
  memory.write_byte(0x201, 0xED);

  // Act
  cpu.execute();

  // Assert
  EXPECT_EQ(cpu.index_register(), 0x0FEDu);
}
