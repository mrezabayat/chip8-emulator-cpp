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

TEST_F(CpuTest, DRW_DrawSpriteWithoutCollision) {
  // Arrange: simple one byte sprite
  memory.write_byte(0x300, 0b11110101u);

  // set I = 0x300
  memory.write_byte(0x200, 0xA3u);
  memory.write_byte(0x201, 0x00u);
  cpu.execute();

  // Act
  EXPECT_EQ(cpu.registers()[0], 0);
  EXPECT_EQ(cpu.registers()[1], 0);

  memory.write_byte(0x202, 0xD0u);
  memory.write_byte(0x203, 0x11u);
  cpu.execute();

  // Assert
  EXPECT_TRUE(display.is_pixel_set(0, 0));
  EXPECT_TRUE(display.is_pixel_set(1, 0));
  EXPECT_TRUE(display.is_pixel_set(2, 0));
  EXPECT_TRUE(display.is_pixel_set(3, 0));
  EXPECT_TRUE(!display.is_pixel_set(4, 0));
  EXPECT_TRUE(display.is_pixel_set(5, 0));
  EXPECT_TRUE(!display.is_pixel_set(6, 0));
  EXPECT_TRUE(display.is_pixel_set(7, 0));
}

TEST_F(CpuTest, DRW_DrawSpriteWithCollision) {
  // Arrange: simple one byte sprite
  memory.write_byte(0x300, 0b11110101u);

  // set I = 0x300
  memory.write_byte(0x200, 0xA3u);
  memory.write_byte(0x201, 0x00u);
  cpu.execute();

  // Act
  EXPECT_EQ(cpu.registers()[0], 0);
  EXPECT_EQ(cpu.registers()[1], 0);

  // draw for the first time
  memory.write_byte(0x202, 0xD0u);
  memory.write_byte(0x203, 0x11u);
  cpu.execute();

  // second time draw should toggle on pixels off
  memory.write_byte(0x204, 0xD0u);
  memory.write_byte(0x205, 0x11u);
  cpu.execute();

  // Assert
  EXPECT_EQ(cpu.registers()[0x0F], 1);
  for (int dx = 0; dx < 8; ++dx) {
    EXPECT_FALSE(display.is_pixel_set(dx, 0));
  }
}

TEST_F(CpuTest, DRW_DrawSpriteNearEdgeWrapsAround) {
  // Arrange: simple two byte sprite
  memory.write_byte(0x300, 0xFFu);
  memory.write_byte(0x301, 0xFFu);

  // set I = 0x300
  memory.write_byte(0x200, 0xA3u);
  memory.write_byte(0x201, 0x00u);
  cpu.execute();

  // set V[0] = 60, V[1] = 30
  memory.write_byte(0x202, 0x60u);
  memory.write_byte(0x203, 0x3Cu);
  cpu.execute();
  EXPECT_EQ(cpu.registers()[0], 0x3Cu);
  memory.write_byte(0x204, 0x61u);
  memory.write_byte(0x205, 0x1Fu);
  cpu.execute();
  EXPECT_EQ(cpu.registers()[1], 0x1Fu);

  // Act
  memory.write_byte(0x206, 0xD0u);
  memory.write_byte(0x207, 0x12u);
  cpu.execute();

  // Assert
  for (int dx = 0; dx < 8; ++dx) {
    EXPECT_TRUE(display.is_pixel_set((60 + dx) % 64, 31));
    EXPECT_TRUE(display.is_pixel_set((60 + dx) % 64, 0));
  }
}
