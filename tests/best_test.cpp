#include <gtest/gtest.h>

#include <cstdio>

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
    bool wantLeft = fx < hx;
    bool wantRight = fx > hx;

    bool oppositeLeft = (wantLeft && dx == +1);
    bool oppositeRight = (wantRight && dx == -1);

    if (oppositeLeft || oppositeRight) {
      if (fy < hy && dy != +1)
        up();
      else if (fy > hy && dy != -1)
        down();
      else if (dy == 0)
        up();

    } else {
      if (wantLeft) left();
      if (wantRight) right();
    }
  } else {
    bool wantUp = fy < hy;
    bool wantDown = fy > hy;

    bool oppositeUp = (wantUp && dy == +1);
    bool oppositeDown = (wantDown && dy == -1);

    if (oppositeUp || oppositeDown) {
      if (fx < hx && dx != +1)
        left();
      else if (fx > hx && dx != -1)
        right();
      else if (dx == 0)
        left();
    } else {
      if (wantUp) up();
      if (wantDown) down();
    }
  }

  e.dispatch(Event::kTick);
}

TEST(BestScore, PersistsAcrossRuns) {
  std::string path = "tmp_best.txt";
  std::remove(path.c_str());

  {
    Engine e{Config{10, 10, 42, false, path}};
    e.dispatch(Event::kStart);

    int target = 2;
    for (int guard = 0; guard < 3000 && e.state() == State::kRunning; ++guard) {
      auto s = e.snapshot();
      if (s.score >= target) break;
      smartStepTowardFood(e);
    }

    for (int i = 0; i < 200 && e.state() == State::kRunning; ++i)
      e.dispatch(Event::kTick);
    EXPECT_EQ(e.state(), State::kGameOver);
  }

  {
    Engine e{Config{10, 10, 777, false, path}};
    e.dispatch(Event::kStart);
    EXPECT_GE(e.snapshot().best, 2);
  }

  std::remove(path.c_str());
}