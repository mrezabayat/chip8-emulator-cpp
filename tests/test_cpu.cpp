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

TEST_F(CpuTest, RETInstructionReturnsToCaller) {
  // Arrange: CALL 0x300 pushes the return address, RET pops it
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

TEST_F(CpuTest, SNE_VxByte_SkipsNextInstructionWhenNotEqual) {
  // Arrange: set v[0x09] = 0x56
  memory.write_byte(0x200, 0x69u);
  memory.write_byte(0x201, 0x56u);
  cpu.execute();

  // Next instruction
  memory.write_byte(0x202, 0x49u);
  memory.write_byte(0x203, 0x50u);
  uint16_t before = cpu.program_counter();
  cpu.execute(); // should skip

  EXPECT_EQ(cpu.program_counter(), before + 4);
}

TEST_F(CpuTest, SE_VxVy_SkipsNextInstructionWhenEqual) {
  // Arrange: set v[0x03] = v[0x0A] = 0x85
  memory.write_byte(0x200, 0x63u);
  memory.write_byte(0x201, 0x85u);
  cpu.execute();

  memory.write_byte(0x202, 0x6Au);
  memory.write_byte(0x203, 0x85u);
  cpu.execute();

  // Act
  memory.write_byte(0x204, 0x53u);
  memory.write_byte(0x205, 0xA0u);
  uint16_t before = cpu.program_counter();
  cpu.execute();

  // Assert
  EXPECT_EQ(cpu.program_counter(), before + 4);
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

TEST_F(CpuTest, SNE_VxVy_SkipsNextInstructionIfVxNEVy) {
  // Arrange: set v[0x03] = 0x64 and v[0x0A] = 0x80
  memory.write_byte(0x200, 0x63u);
  memory.write_byte(0x201, 0x64u);
  cpu.execute();
  EXPECT_EQ(cpu.registers()[0x03], 0x64u);

  memory.write_byte(0x202, 0x6Au);
  memory.write_byte(0x203, 0x80u);
  cpu.execute();
  EXPECT_EQ(cpu.registers()[0x0A], 0x80u);

  // Act
  memory.write_byte(0x204, 0x93u);
  memory.write_byte(0x205, 0xA0u);
  uint16_t before = cpu.program_counter();
  cpu.execute();

  // Assert
  EXPECT_EQ(cpu.program_counter(), before + 4);
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

TEST_F(CpuTest, JP_V0_JumpsToNNNPlusV0) {
  // Arrange: set v[0x00] = 0xAB
  memory.write_byte(0x200, 0x60u);
  memory.write_byte(0x201, 0xABu);
  cpu.execute();
  EXPECT_EQ(cpu.registers()[0x00], 0xABu);

  // Act
  memory.write_byte(0x202, 0xB8u);
  memory.write_byte(0x203, 0x79u);
  cpu.execute();

  // Assert
  EXPECT_EQ(cpu.program_counter(), 0x00ABu + 0x0879u);
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

TEST_F(CpuTest, RET_WithEmptyStackThrows) {
  memory.write_byte(0x200, 0x00u);
  memory.write_byte(0x201, 0xEEu);

  EXPECT_THROW(cpu.execute(), std::runtime_error);
}

TEST_F(CpuTest, CALL_WhenStackIsFullThrows) {
  // CALL 0x200 recursively until the stack is full
  memory.write_byte(0x200, 0x22u);
  memory.write_byte(0x201, 0x00u);
  for (int i = 0; i < chip8::NUM_CPU_STACK; ++i)
    cpu.execute();

  EXPECT_THROW(cpu.execute(), std::runtime_error);
}
