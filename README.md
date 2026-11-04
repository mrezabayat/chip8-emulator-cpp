# CHIP8 Emulator in C++

> **You're on the `series` branch: the starting point for Episode 3 (The display)** of *Build a CHIP-8
> Emulator* on MyCodingDays. Memory from Episode 2 is in place. `tests/test_display.cpp` is new, and the
> `Display` class in `include/chip8_display.h` is empty: its functions throw "Not built yet", so all seven
> display tests fail until you build it. Start your own branch here with
> `git switch -c my-build ep-03-start`. My solution is the tag `ep-03-done`, and the finished emulator is on `main`.
>
> **Building in your own project?** Carry on from where you are: copy `tests/test_display.cpp` and keep the
> same names (`chip8::Display`, `set_pixel`, `is_pixel_set`, `clear`, `draw_sprite`).

## 🚀 Getting Started

### Prerequisites

- C++23 capable compiler (GCC 13+, Clang 16+, MSVC 2022+)
- [CMake](https://cmake.org/) 3.18+
- [Conan](https://conan.io/) package manager
- [Python](https://www.python.org/) 3.12+

### Build

```bash
# Clone (with the e2e test ROMs submodule)
git clone --recurse-submodules https://github.com/mrezabayat/chip8-emulator-cpp.git
cd chip8-emu-cpp

# Install requirements
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
pre-commit install

# Instatll dependencies
conan profile detect --force
conan install . --build=missing

# Configure & build
cmake --preset conan-release
cmake --build --preset conan-release
```

### Test

```bash
ctest --preset conan-release --output-on-failure
```

End-to-end tests run [Timendus' chip8-test-suite](https://github.com/Timendus/chip8-test-suite)
ROMs headlessly and compare the final screen with snapshots in `tests/e2e/golden`.
If you cloned without submodules, run `git submodule update --init` first.
To regenerate snapshots after an intended change, run the e2e binary with
`CHIP8_UPDATE_GOLDEN=1` and review the diff.

## 🎮 Controls

The CHIP-8 keypad maps to hex digits (0x0–0xF). A typical layout:

```text
 chip8           PC
-------        -------
1 2 3 C        1 2 3 4
4 5 6 D  --->  Q W E R
7 8 9 E        A S D F
A 0 B F        Z X C V
```

## 📖 Learning Goals

- Practice **TDD in C++**
- Learn emulator architecture
- Explore modern C++23 design
- Build a cross-platform SDL application

## 📹 YouTube Tutorial

This repo is part of a teaching series: *Building a CHIP-8 Emulator with TDD in Modern C++*.
Follow along on [YouTube](https://youtube.com/yourchannel).

## 📜 License

MIT License – feel free to use this code for learning or as a starting point for your own emulator.
