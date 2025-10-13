#include "chip8_display.h"
#include "gtest/gtest.h"

TEST(DisplayTest, ScreenStartsCleared) {
  chip8::Display d;
  for (int y = 0; y < chip8::SCREEN_HEIGHT; y++) {
    for (int x = 0; x < chip8::SCREEN_WIDTH; x++) {
      EXPECT_FALSE(d.is_pixel_set(x, y));
    }
  }
}

TEST(DisplayTest, CanSetAndCheckPixel) {
  chip8::Display d;
  d.set_pixel(2, 3, true);
  EXPECT_TRUE(d.is_pixel_set(2, 3));
}

TEST(DisplayTest, SetPixelWrapsAround) {
  chip8::Display d;
  d.set_pixel(chip8::SCREEN_WIDTH + 1, chip8::SCREEN_HEIGHT + 2, true);
  EXPECT_TRUE(d.is_pixel_set(1, 2)); // wrapped coordinates
}

TEST(DisplayTest, SetPixelWrapsNegativeCoordinates) {
  chip8::Display d;
  d.set_pixel(-1, -1, true);
  EXPECT_TRUE(
      d.is_pixel_set(chip8::SCREEN_WIDTH - 1, chip8::SCREEN_HEIGHT - 1));
}

TEST(DisplayTest, ClearResetsAllPixels) {
  chip8::Display d;
  d.set_pixel(5, 5, true);
  d.clear();
  EXPECT_FALSE(d.is_pixel_set(5, 5));
}

TEST(DisplayTest, DrawSpriteWithoutCollision) {
  chip8::Display d;
  uint8_t sprite[2] = {0b11110000, 0b00001111};
  bool collision = d.draw_sprite(0, 0, sprite);

  EXPECT_FALSE(collision);
  EXPECT_TRUE(d.is_pixel_set(0, 0));
  EXPECT_TRUE(d.is_pixel_set(7, 1));
}

TEST(DisplayTest, DrawSpriteWithCollision) {
  chip8::Display d;
  uint8_t sprite[1] = {0b11110000};

  auto _ = d.draw_sprite(0, 0, sprite);         // first time
  bool collision = d.draw_sprite(0, 0, sprite); // second time → XOR clears

  EXPECT_TRUE(collision);
  EXPECT_FALSE(d.is_pixel_set(0, 0)); // flipped off
}
