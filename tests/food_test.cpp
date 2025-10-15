#include <gtest/gtest.h>

#include "brick_game/snake/backend.h"

using namespace s21::snake;

TEST(SnakeFood, FoodExistsAfterStart) {
  Engine e{Config{10, 10, 123, false}};
  e.dispatch(Event::kStart);
  auto snap = e.snapshot();

  auto [fx, fy] = snap.food;
  EXPECT_GE(fx, 0);
  EXPECT_GE(fy, 0);
  EXPECT_LT(fx, snap.width);
  EXPECT_LT(fy, snap.height);

  EXPECT_EQ(snap.grid[fy * snap.width + fx], Cell::kFood);
}

TEST(SnakeFood, EatIncreasesScoreAndGrows) {
  Engine e{Config{12, 8, 777, false}};
  e.dispatch(Event::kStart);

  auto s0 = e.snapshot();
  int initial_len = (int)s0.snake.size();
  int initial_score = s0.score;

  bool ate = false;
  for (int guard = 0; guard < 400 && e.state() == State::kRunning; ++guard) {
    auto s = e.snapshot();
    auto [fx, fy] = s.food;
    auto [hx, hy] = s.snake.front();

    if (fx > hx)
      e.dispatch(Event::kMoveRight);
    else if (fx < hx)
      e.dispatch(Event::kMoveLeft);
    else if (fy < hy)
      e.dispatch(Event::kMoveUp);
    else if (fy > hy)
      e.dispatch(Event::kMoveDown);

    e.dispatch(Event::kTick);

    auto s2 = e.snapshot();
    if (s2.score == initial_score + 1) {
      EXPECT_EQ((int)s2.snake.size(), initial_len + 1);
      ate = true;
      break;
    }
  }
  EXPECT_TRUE(ate);
}