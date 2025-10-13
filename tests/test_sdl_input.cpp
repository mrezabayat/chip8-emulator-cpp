#include "chip8_keyboard.h"
#include "sdl_input.h"
#include "gtest/gtest.h"
#include <unordered_map>

static SDL_Event make_event(SDL_Scancode sc, Uint32 type) {
  SDL_Event event{};
  event.type = type;
  event.key.keysym.scancode = sc;
  return event;
}

class SdlInputTest : public ::testing::Test {
protected:
  chip8::Keyboard kb;
  chip8::SdlInput input{kb};
};

TEST_F(SdlInputTest, PressingAndReleasingMappedKeySetsKeyState) {
  auto event = make_event(SDL_SCANCODE_1, SDL_KEYDOWN);
  input.handle_event(event);
  EXPECT_TRUE(kb.is_pressed(0x1));

  event.type = SDL_KEYUP;
  input.handle_event(event);
  EXPECT_FALSE(kb.is_pressed(0x1));
}

TEST_F(SdlInputTest, UnmappedKeyDoesNothing) {
  auto event = make_event(SDL_SCANCODE_N, SDL_KEYDOWN);
  input.handle_event(event);
  for (uint8_t key = 0; key < chip8::NUM_KEYS; ++key) {
    EXPECT_FALSE(kb.is_pressed(key))
        << "Key " << key << " should not be pressed\n";
  }
}

TEST_F(SdlInputTest, CustomKeymapWorks) {
  std::unordered_map<SDL_Scancode, uint8_t> keymap = {{SDL_SCANCODE_P, 0xA}};
  input.set_keymap(keymap);

  auto event = make_event(SDL_SCANCODE_P, SDL_KEYDOWN);
  input.handle_event(event);

  EXPECT_TRUE(kb.is_pressed(0xA));
}
