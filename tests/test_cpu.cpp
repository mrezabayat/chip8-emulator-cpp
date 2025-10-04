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

TEST_F(CpuTest, RETInstructionReturnsToCaller) {
  // Arrange: simulate CALL 0x300 (we'll manually pre-load stack)
  // Program layout:
  // 0x200: 23 00   -> CALL 0x300
  // 0x300: 00 EE   -> RET

  memory.write_byte(0x200, 0x23);
  memory.write_byte(0x201, 0x00);
  memory.write_byte(0x300, 0x00);
  memory.write_byte(0x301, 0xEE);

  // Step 1: CALL 0x300 should jump there
  cpu.execute();
  EXPECT_EQ(cpu.program_counter(), 0x0300u);

  // Step 2: RET should bring us back to next instruction after CALL (0x202)
  cpu.execute();
  EXPECT_EQ(cpu.program_counter(), 0x0202u);

  // If we get here without infinite loop or crash, RET worked
  SUCCEED();
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

TEST_F(CpuTest, SESkipsNextInstructionIfVxIsKK) {
  // Set V[B] = 85
  memory.write_byte(0x200, 0x6Bu);
  memory.write_byte(0x201, 0x85u);
  cpu.execute();
  EXPECT_EQ(cpu.registers()[0x0B], 0x85u);
  EXPECT_EQ(cpu.program_counter(), 0x202);

  memory.write_byte(0x202, 0x3Bu);
  memory.write_byte(0x203, 0x85u);
  cpu.execute();
  EXPECT_EQ(cpu.program_counter(), 0x206);
}