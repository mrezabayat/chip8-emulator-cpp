# CHIP8 Emulator in C++

## 🚀 Getting Started

### Prerequisites

- C++23 capable compiler (GCC 13+, Clang 16+, MSVC 2022+)
- [CMake](https://cmake.org/) 3.18+
- [Conan](https://conan.io/) package manager
- [Python](https://www.python.org/) 3.12+

### Build

```bash
# Clone
git clone https://github.com/mrezabayat/chip8-emulator-cpp.git
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

## 🎮 Controls

The CHIP-8 keypad maps to hex digits (0x0–0xF). A typical layout:

```text
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
