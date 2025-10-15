#include <gtest/gtest.h>

#include "brick_game/snake/backend.h"

using s21::snake::Config;
using s21::snake::Engine;
using s21::snake::Event;
using s21::snake::State;

TEST(SnakeMove, StartThenTickMovesRight) {
  Engine e{Config{20, 20, 0, false}};
  e.dispatch(Event::kStart);
  auto before = e.snapshot().snake.front();
  auto [bx, by] = before;

  e.dispatch(Event::kTick);

  auto after = e.snapshot().snake.front();
  auto [ax, ay] = after;

  EXPECT_EQ(ay, by);
  EXPECT_EQ(ax, bx + 1);
}

TEST(SnakeMove, OppositeTurnIsIgnored) {
  Engine e{};
  e.dispatch(Event::kStart);
  auto [bx, by] = e.snapshot().snake.front();
  e.dispatch(Event::kMoveLeft);
  e.dispatch(Event::kTick);
  auto [ax, ay] = e.snapshot().snake.front();
  EXPECT_EQ(ay, by);
  EXPECT_EQ(ax, bx + 1);
}

TEST(SnakeMove, TurnUpChangesDY) {
  Engine e{};
  e.dispatch(Event::kStart);
  auto [bx, by] = e.snapshot().snake.front();
  e.dispatch(Event::kMoveUp);
  e.dispatch(Event::kTick);
  auto [ax, ay] = e.snapshot().snake.front();
  EXPECT_EQ(ax, bx);
  EXPECT_EQ(ay, by - 1);
}

TEST(SnakeMove, HitWallLeadsToGameOver) {
  Engine e{Config{8, 5, 0, false}};
  e.dispatch(Event::kStart);

  for (int i = 0; i < 20 && e.state() == State::kRunning; ++i) {
    e.dispatch(Event::kTick);
  }
  EXPECT_EQ(e.state(), State::kGameOver);
}