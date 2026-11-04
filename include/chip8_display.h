// include/chip8_display.h
#pragma once
#include "constants.h"
#include <array>
#include <cstdint>
#include <span>
#include <stdexcept>

namespace chip8 {

// Episode 3: build this class until every test in tests/test_display.cpp passes.
//   Step 1: a framebuffer: set_pixel, is_pixel_set, clear, wrapping past the edge
//           (ScreenStartsCleared, CanSetAndCheckPixel, SetPixelWrapsAround, ClearResetsAllPixels)
//   Step 2: wrap negative coordinates too (SetPixelWrapsNegativeCoordinates)
//   Step 3: draw_sprite: XOR each set bit, report collisions
//           (DrawSpriteWithoutCollision, DrawSpriteWithCollision)
class Display {
public:
  explicit Display() = default;

  void clear() { throw std::logic_error("Not built yet"); }

  void set_pixel(int x, int y, bool on) {
    (void)x;
    (void)y;
    (void)on;
    throw std::logic_error("Not built yet");
  }

  [[nodiscard]] bool is_pixel_set(int x, int y) const {
    (void)x;
    (void)y;
    throw std::logic_error("Not built yet");
  }

  [[nodiscard]] bool draw_sprite(int x, int y, std::span<const uint8_t> sprite) {
    (void)x;
    (void)y;
    (void)sprite;
    throw std::logic_error("Not built yet");
  }
};

} // namespace chip8
