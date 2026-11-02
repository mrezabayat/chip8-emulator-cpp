// End-to-end tests: run Timendus' chip8-test-suite ROMs headlessly and compare
// the final screen with a golden snapshot. Set CHIP8_UPDATE_GOLDEN=1 to
// (re)write the snapshots, then review them before committing.
#include "chip8_cpu.h"
#include "chip8_display.h"
#include "chip8_keyboard.h"
#include "chip8_memory.h"
#include "chip8_pcg_rand.h"
#include "chip8_timer.h"
#include "constants.h"
#include "gtest/gtest.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

namespace {

struct RomCase {
  std::string name;
  uint8_t platform; // Value forced into 0x1FF to skip the menu, 0 for none
  uint32_t frames;
  uint32_t cycles_per_frame;
};

void PrintTo(const RomCase &rom_case, std::ostream *os) { *os << rom_case.name; }

std::string render(const chip8::Display &display) {
  std::string out;
  for (int y = 0; y < chip8::SCREEN_HEIGHT; ++y) {
    for (int x = 0; x < chip8::SCREEN_WIDTH; ++x)
      out += display.is_pixel_set(x, y) ? '#' : '.';
    out += '\n';
  }
  return out;
}

std::string run_rom(const RomCase &rom_case) {
  chip8::Memory memory;
  chip8::Display display;
  chip8::Keyboard keyboard;
  chip8::Timer timer;
  chip8::PcgRandom rng{0xC0FFEEu};
  chip8::Cpu cpu{memory, display, keyboard, timer, rng};

  const auto rom_path =
      std::filesystem::path{CHIP8_TEST_SUITE_DIR} / (rom_case.name + ".ch8");
  std::ifstream file(rom_path, std::ios::binary);
  if (!file)
    throw std::runtime_error("Missing ROM: " + rom_path.string());
  const std::vector<uint8_t> rom{std::istreambuf_iterator<char>(file), {}};

  for (std::size_t i = 0; i < rom.size(); ++i)
    memory.write_byte(static_cast<uint16_t>(chip8::START_ADDRESS + i), rom[i]);
  if (rom_case.platform != 0)
    memory.write_byte(0x1FF, rom_case.platform);

  for (uint32_t frame = 0; frame < rom_case.frames; ++frame) {
    for (uint32_t cycle = 0; cycle < rom_case.cycles_per_frame; ++cycle)
      cpu.execute();
    timer.tick_60hz();
  }
  return render(display);
}

class RomTest : public ::testing::TestWithParam<RomCase> {};

TEST_P(RomTest, ScreenMatchesGolden) {
  const auto &rom_case = GetParam();
  const auto screen = run_rom(rom_case);
  const auto golden_path =
      std::filesystem::path{CHIP8_GOLDEN_DIR} / (rom_case.name + ".txt");

  if (std::getenv("CHIP8_UPDATE_GOLDEN") != nullptr) {
    std::ofstream(golden_path) << screen;
    GTEST_SKIP() << "Wrote " << golden_path;
  }

  std::ifstream golden_file(golden_path);
  ASSERT_TRUE(golden_file) << "Missing golden " << golden_path
                           << ", run with CHIP8_UPDATE_GOLDEN=1";
  std::stringstream golden;
  golden << golden_file.rdbuf();
  EXPECT_EQ(screen, golden.str()) << "Actual screen:\n" << screen;
}

INSTANTIATE_TEST_SUITE_P(
    TimendusSuite, RomTest,
    ::testing::Values(RomCase{"1-chip8-logo", 0, 300, 10},
                      RomCase{"2-ibm-logo", 0, 300, 10},
                      RomCase{"3-corax+", 0, 300, 10},
                      RomCase{"4-flags", 0, 300, 10},
                      // Golden records this emulator's quirk choices against
                      // the CHIP-8 profile: no vF reset, no display wait,
                      // wrapping instead of clipping, shifts operate on Vx.
                      RomCase{"5-quirks", 1, 300, 30}),
    [](const auto &info) {
      std::string name = info.param.name;
      for (auto &c : name)
        if (!std::isalnum(static_cast<unsigned char>(c)))
          c = '_';
      return name;
    });

} // namespace
