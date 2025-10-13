#pragma once
#include "SDL2/SDL.h"
#include "chip8_keyboard.h"
#include <unordered_map>

namespace chip8 {

class SdlInput {
public:
  explicit SdlInput(Keyboard &kb) noexcept : kb_(kb) {
    keymap_ = {{SDL_SCANCODE_1, 0x1}, {SDL_SCANCODE_2, 0x2},
               {SDL_SCANCODE_3, 0x3}, {SDL_SCANCODE_4, 0xC},

               {SDL_SCANCODE_Q, 0x4}, {SDL_SCANCODE_W, 0x5},
               {SDL_SCANCODE_E, 0x6}, {SDL_SCANCODE_R, 0xD},

               {SDL_SCANCODE_A, 0x7}, {SDL_SCANCODE_S, 0x8},
               {SDL_SCANCODE_D, 0x9}, {SDL_SCANCODE_F, 0xE},

               {SDL_SCANCODE_Z, 0xA}, {SDL_SCANCODE_X, 0x0},
               {SDL_SCANCODE_C, 0xB}, {SDL_SCANCODE_V, 0xF}};
  }

  constexpr void handle_event(const SDL_Event &event) const {
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
      const auto scancode = event.key.keysym.scancode;
      if (keymap_.find(scancode) != keymap_.end()) {
        kb_.set_key_state(keymap_.at(scancode), event.type == SDL_KEYDOWN);
      }
    }
  }

  constexpr void
  set_keymap(const std::unordered_map<SDL_Scancode, uint8_t> &keymap) {
    keymap_ = keymap;
  }

private:
  Keyboard &kb_;
  std::unordered_map<SDL_Scancode, uint8_t> keymap_;
};

} // namespace chip8