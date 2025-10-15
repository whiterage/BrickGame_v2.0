#ifndef CONSOLEVIEW_H
#define CONSOLEVIEW_H

#include <chrono>
#include <thread>

class SnakeController;

enum Choice {
  START = 1,
  PAUSE = 2,
  MOVE_UP = 3,
  MOVE_DOWN = 4,
  MOVE_LEFT = 5,
  MOVE_RIGHT = 6,
  RESET = 7,
  QUIT = 0,
  NONE = -1
};

class ConsoleView {
 private:
  SnakeController* controller_;
  bool running_;
  std::chrono::steady_clock::time_point last_tick_;

 public:
  ConsoleView(SnakeController* controller);

  void displayMenu();
  int performChoice();
  void startEventLoop();
  void displayGame();
  void displayGameOver();
  void displayPaused();

 private:
  void clearScreen();
  void displayField();
  void displayStats();
  void handleGameTick();
  char getCellChar(int x, int y);
};

#endif
