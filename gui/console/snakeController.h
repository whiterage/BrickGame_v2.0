#ifndef SNAKECONTROLLER_H
#define SNAKECONTROLLER_H

#include "snakeModel.h"

class ConsoleView;

class SnakeController {
 private:
  SnakeModel* model_;

 public:
  SnakeController(SnakeModel* model);

  void startGame();
  void pauseGame();
  void resetGame();
  void moveUp();
  void moveDown();
  void moveLeft();
  void moveRight();
  void tick();
  void quit();

  const SnakeModel* getModel() const;
};

#endif
