#include "chip8_timer.h"
#include "gtest/gtest.h"

TEST(TimerTest, DelayTimerDecreases) {
  chip8::Timer timer;
  timer.set_delay(5);
  for (int i = 0; i < 4; ++i) {
    timer.tick_60hz();
  }
  EXPECT_EQ(timer.delay(), 1);
}

TEST(TimerTest, DelayTimerStopsAtZero) {
  chip8::Timer timer;
  timer.set_delay(2);
  for (int i = 0; i < 50; ++i) {
    timer.tick_60hz();
  }
  EXPECT_EQ(timer.delay(), 0);
}

TEST(TimerTest, SoundTimerDecrementsAndTriggersBeep) {
  chip8::Timer timer;
  timer.set_sound(3);

  EXPECT_TRUE(timer.beep()); // sound_ > 0

  timer.tick_60hz();
  EXPECT_TRUE(timer.beep()); // still > 0

  timer.tick_60hz();
  timer.tick_60hz();
  EXPECT_FALSE(timer.beep()); // now 0
}

TEST(TimerTest, SettingValuesWorks) {
  chip8::Timer timer;
  timer.set_delay(123);
  timer.set_sound(59);

  EXPECT_EQ(timer.delay(), 123);
  EXPECT_EQ(timer.sound(), 59);
  EXPECT_TRUE(timer.sound());
}