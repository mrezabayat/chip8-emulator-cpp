#pragma once
#include "SDL2/SDL.h"
#include <atomic>
#include <cmath>
#include <stdexcept>

namespace chip8 {

class SdlAudio {
public:
  SdlAudio() {
    SDL_AudioSpec desired{};
    desired.freq = 44100;
    desired.format = AUDIO_F32SYS;
    desired.channels = 1;
    desired.samples = 4096;
    desired.userdata = this;
    desired.callback = [](void *userdata, Uint8 *stream, int len) {
      auto *self = static_cast<SdlAudio *>(userdata);
      self->audio_callback(stream, len);
    };

    device_ = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained_, 0);
    if (!device_)
      throw std::runtime_error(SDL_GetError());
  }

  ~SdlAudio() {
    if (device_)
      SDL_CloseAudioDevice(device_);
  }

  void play() noexcept { SDL_PauseAudioDevice(device_, 0); }
  void stop() noexcept { SDL_PauseAudioDevice(device_, 1); }

private:
  void audio_callback(Uint8 *stream, int len) noexcept {
    float *buf = reinterpret_cast<float *>(stream);
    int samples = len / sizeof(float);
    static float phase = 0.0f;
    float freq = 440.0f;

    for (int i = 0; i < samples; ++i) {
      buf[i] = playing_ ? std::sin(phase) * 0.2f : 0.0f;
      phase += 2.0f * float(M_PI) * freq / obtained_.freq;
      if (phase >= 2.0f * float(M_PI))
        phase -= 2.0f * float(M_PI);
    }
  }

  SDL_AudioDeviceID device_{};
  SDL_AudioSpec obtained_{};
  bool playing_{true};
};

} // namespace chip8