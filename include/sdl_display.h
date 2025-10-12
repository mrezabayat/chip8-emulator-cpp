#pragma once
#include "SDL2/SDL.h"
#include "constants.h"
#include <array>
#include <memory>
#include <stdexcept>

namespace chip8 {

class SdlDisplay {
public:
  explicit SdlDisplay(int scale = 10) : scale_(scale) {
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
      throw std::runtime_error(SDL_GetError());
    }

    // Create window
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> win(
        SDL_CreateWindow("CHIP8 Emulator", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH * scale_,
                         SCREEN_HEIGHT * scale_, SDL_WINDOW_SHOWN),
        SDL_DestroyWindow);
    if (!win) {
      throw std::runtime_error(SDL_GetError());
    }

    // Create renderer
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> rend(
        SDL_CreateRenderer(win.get(), -1,
                           SDL_RENDERER_ACCELERATED |
                               SDL_RENDERER_PRESENTVSYNC),
        SDL_DestroyRenderer);
    if (!rend) {
      throw std::runtime_error(SDL_GetError());
    }

    // Create texture
    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> tex(
        SDL_CreateTexture(rend.get(), SDL_PIXELFORMAT_RGBA8888,
                          SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH,
                          SCREEN_HEIGHT),
        SDL_DestroyTexture);
    if (!tex) {
      throw std::runtime_error(SDL_GetError());
    }

    window_ = std::move(win);
    renderer_ = std::move(rend);
    texture_ = std::move(tex);
  }

private:
  std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window_{
      nullptr, SDL_DestroyWindow};
  std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer_{
      nullptr, SDL_DestroyRenderer};
  std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> texture_{
      nullptr, SDL_DestroyTexture};

  std::array<uint32_t, SCREEN_HEIGHT * SCREEN_WIDTH> pixel_buffer_{};
  int scale_;
};

} // namespace chip8