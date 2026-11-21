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

// Your turn (episode 6): one line per screen row, '#' for a lit pixel and '.' for a dark
// one, and a newline after every row. The golden files in tests/e2e/golden look exactly like that.
std::string render(const chip8::Display &display) {
  throw std::runtime_error("Not built yet");
}

// Your turn (episode 6): build the parts and a Cpu (seed the random generator with 0xC0FFEE),
// load the ROM file at START_ADDRESS, then run frames x cycles_per_frame instructions,
// ticking the timer once per frame. Return the screen as render() draws it.
std::string run_rom(const RomCase &rom_case) {
  throw std::runtime_error("Not built yet");
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
                      RomCase{"2-ibm-logo", 0, 300, 10}),
    [](const auto &info) {
      std::string name = info.param.name;
      for (auto &c : name)
        if (!std::isalnum(static_cast<unsigned char>(c)))
          c = '_';
      return name;
    });

} // namespace
