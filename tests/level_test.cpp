#include <gtest/gtest.h>

#include "brick_game/snake/backend.h"
using namespace s21::snake;

static void smartStepTowardFood(Engine& e) {
  auto s = e.snapshot();
  auto [fx, fy] = s.food;
  auto [hx, hy] = s.snake.front();

  int dx = 0, dy = 0;
  if (s.snake.size() >= 2) {
    auto [nx, ny] = s.snake[1];
    dx = hx - nx;
    dy = hy - ny;
  }

  auto right = [&] { e.dispatch(Event::kMoveRight); };
  auto left = [&] { e.dispatch(Event::kMoveLeft); };
  auto up = [&] { e.dispatch(Event::kMoveUp); };
  auto down = [&] { e.dispatch(Event::kMoveDown); };

  if (fx != hx) {
    bool wantL = fx<hx, wantR = fx> hx;
    bool oppL = wantL && dx == +1;
    bool oppR = wantR && dx == -1;
    if (oppL || oppR) {
      if (fy < hy && dy != +1)
        up();
      else if (fy > hy && dy != -1)
        down();
      else if (dy == 0)
        up();
    } else {
      if (wantL) left();
      if (wantR) right();
    }
  } else {
    bool wantU = fy<hy, wantD = fy> hy;
    bool oppU = wantU && dy == +1;
    bool oppD = wantD && dy == -1;
    if (oppU || oppD) {
      if (fx < hx && dx != +1)
        left();
      else if (fx > hx && dx != -1)
        right();
      else if (dx == 0)
        left();
    } else {
      if (wantU) up();
      if (wantD) down();
    }
  }

  e.dispatch(Event::kTick);
}

TEST(Levels, LevelAndSpeedRespondToScore) {
  Engine e{Config{12, 8, 123, false}};
  e.dispatch(Event::kStart);

  int lastLevel = e.snapshot().level;
  int lastSpeed = e.snapshot().speed_ms;

  int eaten = 0;
  for (int guard = 0; guard < 3000 && eaten < 6 && e.state() == State::kRunning;
       ++guard) {
    smartStepTowardFood(e);
    auto s2 = e.snapshot();
    if (s2.score > eaten) {
      eaten = s2.score;
      EXPECT_GE(s2.level, lastLevel);
      EXPECT_LE(s2.speed_ms, lastSpeed);
      lastLevel = s2.level;
      lastSpeed = s2.speed_ms;
    }
  }
  EXPECT_GE(lastLevel, 2);
}