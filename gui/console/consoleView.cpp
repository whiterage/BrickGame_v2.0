#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>

#include "consoleView.h"
#include "snakeController.h"

ConsoleView::ConsoleView(SnakeController* controller)
    : controller_(controller), running_(true) {
  last_tick_ = std::chrono::steady_clock::now();
}

void ConsoleView::clearScreen() { system("clear"); }

void ConsoleView::displayMenu() {
  std::cout << "\033[2J\033[H";  // Очистка экрана
  std::cout << "  S N A K E    " << std::endl;
  std::cout << "===============" << std::endl;
  std::cout << "1. Start Game" << std::endl;
  std::cout << "2. Pause/Resume" << std::endl;
  std::cout << "3. Move Up (W)" << std::endl;
  std::cout << "4. Move Down (S)" << std::endl;
  std::cout << "5. Move Left (A)" << std::endl;
  std::cout << "6. Move Right (D)" << std::endl;
  std::cout << "7. Reset Game" << std::endl;
  std::cout << "0. Quit" << std::endl;
  std::cout << "===============" << std::endl;
  std::cout << "Controls:" << std::endl;
  std::cout << "  W/A/S/D - Move snake" << std::endl;
  std::cout << "  Space - Pause/Resume" << std::endl;
  std::cout << "  R - Reset" << std::endl;
  std::cout << "  Q - Quit" << std::endl;
  std::cout << "\nНажмите '1' или Enter для начала игры..." << std::endl;
}

int ConsoleView::performChoice() {
  // Эта функция больше не используется
  // Выбор меню теперь обрабатывается в основном цикле
  return 0;
}

char ConsoleView::getCellChar(int x, int y) {
  if (!controller_ || !controller_->getModel()) return ' ';

  auto snapshot = controller_->getModel()->getSnapshot();
  if (x < 0 || x >= snapshot.width || y < 0 || y >= snapshot.height) return '#';

  int index = y * snapshot.width + x;
  auto cell = snapshot.grid[index];

  switch (cell) {
    case s21::snake::Cell::kSnake:
      return 'O';
    case s21::snake::Cell::kFood:
      return '*';
    case s21::snake::Cell::kEmpty:
    default:
      return ' ';
  }
}

void ConsoleView::displayField() {
  if (!controller_ || !controller_->getModel()) return;

  auto snapshot = controller_->getModel()->getSnapshot();

  std::cout << "+";
  for (int x = 0; x < snapshot.width; ++x) {
    std::cout << "--";
  }
  std::cout << "+" << std::endl;

  for (int y = 0; y < snapshot.height; ++y) {
    std::cout << "|";
    for (int x = 0; x < snapshot.width; ++x) {
      char cell = getCellChar(x, y);
      std::cout << cell << " ";
    }
    std::cout << "|" << std::endl;
  }

  std::cout << "+";
  for (int x = 0; x < snapshot.width; ++x) {
    std::cout << "--";
  }
  std::cout << "+" << std::endl;
}

void ConsoleView::displayStats() {
  if (!controller_ || !controller_->getModel()) return;

  auto model = controller_->getModel();

  std::cout << "Score: " << model->getScore() << std::endl;
  std::cout << "Best:  " << model->getBestScore() << std::endl;
  std::cout << "Level: " << model->getLevel() << std::endl;
  std::cout << "Speed: " << model->getSpeed() << "ms" << std::endl;

  std::cout << "State: ";
  if (model->isInitialized()) {
    std::cout << "INIT";
  } else if (model->isRunning()) {
    std::cout << "RUNNING";
  } else if (model->isPaused()) {
    std::cout << "PAUSED";
  } else if (model->isGameOver()) {
    std::cout << "GAME OVER";
  }
  std::cout << std::endl;
}

void ConsoleView::displayGame() {
  clearScreen();
  std::cout << "=== SNAKE GAME ===" << std::endl;
  displayField();
  displayStats();
  std::cout << "Controls: W/A/S/D=move, Space=pause, R=reset, Q=quit"
            << std::endl;
}

void ConsoleView::displayGameOver() {
  clearScreen();
  std::cout << "=== GAME OVER ===" << std::endl;
  displayField();
  displayStats();
  std::cout << "Press 'R' to restart or 'Q' to quit" << std::endl;
}

void ConsoleView::displayPaused() {
  clearScreen();
  std::cout << "=== PAUSED ===" << std::endl;
  displayField();
  displayStats();
  std::cout << "Press 'Space' to resume or 'Q' to quit" << std::endl;
}

void ConsoleView::handleGameTick() {
  if (!controller_ || !controller_->getModel()) return;

  auto now = std::chrono::steady_clock::now();
  auto elapsed =
      std::chrono::duration_cast<std::chrono::milliseconds>(now - last_tick_)
          .count();

  auto model = controller_->getModel();
  int tick_interval = model->getSpeed();

  if (elapsed >= tick_interval && model->isRunning()) {
    controller_->tick();
    last_tick_ = now;
  }
}

void ConsoleView::startEventLoop() {
  struct termios original_termios;
  tcgetattr(STDIN_FILENO, &original_termios);
  struct termios new_termios = original_termios;
  new_termios.c_lflag &= ~(ICANON | ECHO);
  new_termios.c_cc[VMIN] = 0;
  new_termios.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);

  // Не запускаем игру автоматически, пусть пользователь сам выберет
  displayMenu();

  while (running_) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 10000;

    int result = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);

    if (result > 0 && FD_ISSET(STDIN_FILENO, &readfds)) {
      char ch;
      if (read(STDIN_FILENO, &ch, 1) > 0) {
        switch (ch) {
          case 'w':
          case 'W':
            controller_->moveUp();
            break;
          case 's':
          case 'S':
            controller_->moveDown();
            break;
          case 'a':
          case 'A':
            controller_->moveLeft();
            break;
          case 'd':
          case 'D':
            controller_->moveRight();
            break;
          case ' ':
            controller_->pauseGame();
            break;
          case 'r':
          case 'R':
            controller_->resetGame();
            // Автоматически запускаем игру после сброса
            controller_->startGame();
            break;
          case 'q':
          case 'Q':
            running_ = false;
            break;
          case '\n':
          case '\r':
            controller_->startGame();
            break;
          case '1':
            controller_->startGame();
            break;
          case '2':
            controller_->pauseGame();
            break;
          case '3':
            controller_->moveUp();
            break;
          case '4':
            controller_->moveDown();
            break;
          case '5':
            controller_->moveLeft();
            break;
          case '6':
            controller_->moveRight();
            break;
          case '7':
            controller_->resetGame();
            // Автоматически запускаем игру после сброса
            controller_->startGame();
            break;
          case '0':
            running_ = false;
            break;
        }
      }
    }

    handleGameTick();

    if (controller_ && controller_->getModel()) {
      auto model = controller_->getModel();
      if (model->isGameOver()) {
        displayGameOver();
      } else if (model->isPaused()) {
        displayPaused();
      } else if (model->isRunning()) {
        displayGame();
      } else if (model->isInitialized()) {
        // Показываем меню когда игра в состоянии kInit
        displayMenu();
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
  std::cout << "\nThanks for playing Snake!" << std::endl;
}
