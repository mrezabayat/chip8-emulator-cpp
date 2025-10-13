// include/chip8_display.h
#pragma once
#include "constants.h"
#include <array>
#include <cstdint>
#include <span>

namespace chip8 {

class Display {
public:
  explicit Display() noexcept { clear(); }

  // Clear screen
  void clear() noexcept { buffer_.fill(0); }

  // Set or reset pixel at (x,y)
  constexpr void set_pixel(int x, int y, bool on) noexcept {
    auto [nx, ny] = wrap(x, y);
    buffer_[ny * SCREEN_WIDTH + nx] = on ? 1 : 0;
  }

  // Query pixel state
  [[nodiscard]] constexpr bool is_pixel_set(int x, int y) const noexcept {
    auto [nx, ny] = wrap(x, y);
    return buffer_[ny * SCREEN_WIDTH + nx] != 0;
  }

  // Draw sprite at (x,y), returns true if collision occurred
  [[nodiscard]] bool draw_sprite(int x, int y,
                                 std::span<const uint8_t> sprite) noexcept {
    bool collision = false;
    for (int row = 0; row < static_cast<int>(sprite.size()); ++row) {
      uint8_t line = sprite[row];
      for (int bit = 0; bit < 8; ++bit) {
        if (line & (0b10000000 >> bit)) {
          auto [nx, ny] = wrap(x + bit, y + row);
          auto &pixel = buffer_[ny * SCREEN_WIDTH + nx];
          if (pixel == 1) {
            collision = true;
          }
          pixel ^= 1; // XOR drawing
        }
      }
    }
    return collision;
  }

private:
  // Wrap coordinates (Chip-8 wraps around screen edges)
  [[nodiscard]] static constexpr int wrap_coord(int value, int max) noexcept {
    const int mod = value % max;
    return mod < 0 ? mod + max : mod;
  }

  [[nodiscard]] static constexpr std::pair<int, int> wrap(int x,
                                                          int y) noexcept {
    return {wrap_coord(x, SCREEN_WIDTH), wrap_coord(y, SCREEN_HEIGHT)};
  }

  std::array<uint8_t, SCREEN_WIDTH * SCREEN_HEIGHT> buffer_;
};

} // namespace chip8
