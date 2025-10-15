#ifndef SNAKEMODEL_H
#define SNAKEMODEL_H

#include "../../brick_game/snake/backend.h"

class SnakeModel {
 private:
  s21::snake::Engine engine_;
  mutable s21::snake::Snapshot current_snapshot_;

 public:
  SnakeModel();

  void startGame();
  void pauseGame();
  void resetGame();
  void moveUp();
  void moveDown();
  void moveLeft();
  void moveRight();
  void tick();
  void quit();

  const s21::snake::Snapshot& getSnapshot() const;
  bool isRunning() const;
  bool isPaused() const;
  bool isGameOver() const;
  bool isInitialized() const;

  int getScore() const;
  int getBestScore() const;
  int getLevel() const;
  int getSpeed() const;
};

#endif
