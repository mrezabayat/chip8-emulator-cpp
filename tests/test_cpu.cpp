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

TEST_F(CpuTest, LD_VxVy_StoresValueOfVyInVx) {
  // Arrange: set v[0x03] = 0x85
  memory.write_byte(0x200, 0x63u);
  memory.write_byte(0x201, 0x85u);
  cpu.execute();
  EXPECT_EQ(cpu.registers()[0x03], 0x85u);

  // Act: LD v[0x0E] v[0x03]
  memory.write_byte(0x202, 0x8Eu);
  memory.write_byte(0x203, 0x30u);
  EXPECT_EQ(cpu.registers()[0x0E], 0x00u);
  cpu.execute();

  // Assert
  EXPECT_EQ(cpu.registers()[0x0E], 0x85u);
}

TEST_F(CpuTest, OR_VxVy_BitwiseOrOfVxAndVyIsStoredInVx) {
  // Arrange: set v[0x03] = 0x23 and v[A] = 0x45
  memory.write_byte(0x200, 0x63u);
  memory.write_byte(0x201, 0x23u);
  cpu.execute();
  EXPECT_EQ(cpu.registers()[0x03], 0x23u);

  memory.write_byte(0x202, 0x6Au);
  memory.write_byte(0x203, 0x45u);
  cpu.execute();
  EXPECT_EQ(cpu.registers()[0x0A], 0x45u);

  // Act: bitwise or operation
  memory.write_byte(0x204, 0x83u);
  memory.write_byte(0x205, 0xA1u);
  cpu.execute();

  // Assert
  EXPECT_EQ(cpu.registers()[0x03], 0x67u);
}

TEST_F(CpuTest, AND_VxVy_BitwiseAndfOfVxAndVyIsStoredInVx) {
  // Arrange: set v[0x03] = 0x23 and v[A] = 0x45
  memory.write_byte(0x200, 0x63u);
  memory.write_byte(0x201, 0x23u);
  cpu.execute();
  EXPECT_EQ(cpu.registers()[0x03], 0x23u);

  memory.write_byte(0x202, 0x6Au);
  memory.write_byte(0x203, 0x45u);
  cpu.execute();
  EXPECT_EQ(cpu.registers()[0x0A], 0x45u);

  // Act: bitwise and operation
  memory.write_byte(0x204, 0x83u);
  memory.write_byte(0x205, 0xA2u);
  cpu.execute();

  // Assert
  EXPECT_EQ(cpu.registers()[0x03], 0x01u);
}

TEST_F(CpuTest, XOR_VxVy_BitwiseXorfOfVxAndVyIsStoredInVx) {
  // Arrange: set v[0x03] = 0x23 and v[A] = 0x45
  memory.write_byte(0x200, 0x63u);
  memory.write_byte(0x201, 0x23u);
  cpu.execute();
  EXPECT_EQ(cpu.registers()[0x03], 0x23u);

  memory.write_byte(0x202, 0x6Au);
  memory.write_byte(0x203, 0x45u);
  cpu.execute();
  EXPECT_EQ(cpu.registers()[0x0A], 0x45u);

  // Act: bitwise xor operation
  memory.write_byte(0x204, 0x83u);
  memory.write_byte(0x205, 0xA3u);
  cpu.execute();

  // Assert
  EXPECT_EQ(cpu.registers()[0x03], 0x66u);
}

TEST_F(CpuTest, ADD_VxVy_AddTwoRegistersAndStoreCarryInVf) {
  // Arrange: set v[0x03] = 0x80 and v[A] = 0x64
  memory.write_byte(0x200, 0x63u);
  memory.write_byte(0x201, 0x80u);
  cpu.execute();
  EXPECT_EQ(cpu.registers()[0x03], 0x80u);

  memory.write_byte(0x202, 0x6Au);
  memory.write_byte(0x203, 0x64u);
  cpu.execute();
  EXPECT_EQ(cpu.registers()[0x0A], 0x64u);

  // Act: v[0x03] += v[0x0A]
  memory.write_byte(0x204, 0x83u);
  memory.write_byte(0x205, 0xA4u);
  cpu.execute();

  // Assert
  EXPECT_EQ(cpu.registers()[0x03], 0xE4u);
  EXPECT_EQ(cpu.registers()[0x0F], 0x00u);

  // Act one more time to check the carry
  memory.write_byte(0x206, 0x83u);
  memory.write_byte(0x207, 0xA4u);
  cpu.execute();

  // Assert
  EXPECT_EQ(cpu.registers()[0x03], 0x48u);
  EXPECT_EQ(cpu.registers()[0x0F], 0x01u);
}