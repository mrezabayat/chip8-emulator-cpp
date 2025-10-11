#include "chip8_memory.h"
#include "constants.h"
#include <gtest/gtest.h>
#include <stdexcept>

TEST(MemoryTest, CtorCreatesClearMemory) {
  chip8::Memory memory;
  for (uint16_t index = 0; index < chip8::DEFAULT_CHAR_SET.size(); ++index) {
    EXPECT_EQ(memory.read_byte(index), chip8::DEFAULT_CHAR_SET[index]);
  }
  for (uint16_t index = chip8::DEFAULT_CHAR_SET.size();
       index < chip8::MEMORY_SIZE; ++index) {
    EXPECT_EQ(memory.read_byte(index), 0);
  }
}

TEST(MemoryTest, WriteAndReadBytes) {
  chip8::Memory memory;
  uint16_t addr = 599u;
  uint8_t val = 0xABu;
  memory.write_byte(addr, val);
  EXPECT_EQ(memory.read_byte(addr), val);
}

TEST(MemoryTest, ReadTwoBytes) {
  chip8::Memory memory;
  uint16_t addr = 1024u;
  memory.write_byte(addr, 0x0Eu);
  memory.write_byte(addr + 1, 0x9Fu);
  EXPECT_EQ(memory.read_two_bytes(addr), 0x0E9Fu);
}

TEST(MemoryTest, OutOfBoundAccessThrows) {
  chip8::Memory memory;
  EXPECT_THROW(
      { auto _ = memory.read_byte(chip8::MEMORY_SIZE); }, std::out_of_range);
  EXPECT_THROW(
      { auto _ = memory.read_two_bytes(chip8::MEMORY_SIZE); },
      std::out_of_range);
  EXPECT_THROW(memory.write_byte(chip8::MEMORY_SIZE, 0xABu), std::out_of_range);
}