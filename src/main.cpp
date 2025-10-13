#define SDL_MAIN_HANDLED 1
#include "SDL2/SDL.h"
#include "chip8_emulator.h"
#include "sdl_audio.h"
#include "sdl_display.h"
#include "sdl_input.h"
#include <chrono>
#include <filesystem>
#include <iostream>
#include <thread>

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <path-to-rom>\n";
    return 1;
  }

  const std::filesystem::path rom_path{argv[1]};

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
    std::cerr << "SDL initialization failed: " << SDL_GetError() << '\n';
    return 1;
  }

  try {
    chip8::Emulator emulator;
    emulator.load_rom(rom_path);

    chip8::SdlDisplay display{10};
    chip8::SdlAudio audio;
    chip8::SdlInput input{emulator.keyboard()};

    bool running = true;
    constexpr auto frame_duration = std::chrono::milliseconds(16);

    while (running) {
      const auto frame_start = std::chrono::steady_clock::now();

      SDL_Event event{};
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          running = false;
        } else if (event.type == SDL_KEYDOWN &&
                   event.key.keysym.sym == SDLK_ESCAPE) {
          running = false;
        } else {
          input.handle_event(event);
        }
      }

      const auto frame_result = emulator.run_frame();
      display.render(emulator.display());

      if (frame_result.sound_active) {
        audio.play();
      } else {
        audio.stop();
      }

      const auto frame_end = std::chrono::steady_clock::now();
      const auto elapsed = frame_end - frame_start;
      if (elapsed < frame_duration) {
        std::this_thread::sleep_for(frame_duration - elapsed);
      }
    }

    emulator.stop();
    audio.stop();
  } catch (const std::exception &ex) {
    std::cerr << "Fatal error: " << ex.what() << '\n';
    SDL_Quit();
    return 1;
  }

  SDL_Quit();
  return 0;
}
