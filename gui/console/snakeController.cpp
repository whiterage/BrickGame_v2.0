#include "snakeController.h"

SnakeController::SnakeController(SnakeModel* model) : model_(model) {}

void SnakeController::startGame() {
  if (model_) model_->startGame();
}

void SnakeController::pauseGame() {
  if (model_) model_->pauseGame();
}

void SnakeController::resetGame() {
  if (model_) model_->resetGame();
}

void SnakeController::moveUp() {
  if (model_) model_->moveUp();
}

void SnakeController::moveDown() {
  if (model_) model_->moveDown();
}

void SnakeController::moveLeft() {
  if (model_) model_->moveLeft();
}

void SnakeController::moveRight() {
  if (model_) model_->moveRight();
}

void SnakeController::tick() {
  if (model_) model_->tick();
}

void SnakeController::quit() {
  if (model_) model_->quit();
}

const SnakeModel* SnakeController::getModel() const { return model_; }
