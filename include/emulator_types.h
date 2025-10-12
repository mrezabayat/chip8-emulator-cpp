#pragma once
#include <cstdint>

namespace chip8 {

enum class EmulatorState { Stopped, Running, Paused };

struct RunFrameResult {
  bool frame_complete{false};
  bool sound_active{false};
};

} // namespace chip8
