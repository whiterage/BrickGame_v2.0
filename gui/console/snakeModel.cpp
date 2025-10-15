#include "snakeModel.h"

SnakeModel::SnakeModel() : engine_(s21::snake::Config{}) {}

void SnakeModel::startGame() { engine_.dispatch(s21::snake::Event::kStart); }

void SnakeModel::pauseGame() {
  engine_.dispatch(s21::snake::Event::kPauseToggle);
}

void SnakeModel::resetGame() { engine_.dispatch(s21::snake::Event::kReset); }

void SnakeModel::moveUp() { engine_.dispatch(s21::snake::Event::kMoveUp); }

void SnakeModel::moveDown() { engine_.dispatch(s21::snake::Event::kMoveDown); }

void SnakeModel::moveLeft() { engine_.dispatch(s21::snake::Event::kMoveLeft); }

void SnakeModel::moveRight() {
  engine_.dispatch(s21::snake::Event::kMoveRight);
}

void SnakeModel::tick() { engine_.dispatch(s21::snake::Event::kTick); }

void SnakeModel::quit() { engine_.dispatch(s21::snake::Event::kQuit); }

const s21::snake::Snapshot& SnakeModel::getSnapshot() const {
  current_snapshot_ = engine_.snapshot();
  return current_snapshot_;
}

bool SnakeModel::isRunning() const {
  return engine_.state() == s21::snake::State::kRunning;
}

bool SnakeModel::isPaused() const {
  return engine_.state() == s21::snake::State::kPaused;
}

bool SnakeModel::isGameOver() const {
  return engine_.state() == s21::snake::State::kGameOver;
}

bool SnakeModel::isInitialized() const {
  return engine_.state() != s21::snake::State::kInit;
}

int SnakeModel::getScore() const { return getSnapshot().score; }

int SnakeModel::getBestScore() const { return getSnapshot().best; }

int SnakeModel::getLevel() const { return getSnapshot().level; }

int SnakeModel::getSpeed() const { return getSnapshot().speed_ms; }
