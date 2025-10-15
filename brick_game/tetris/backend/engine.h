/**
 * @file engine.h
 * @brief Заголовочный файл игровой логики Tetris
 * @author BrickGame Team
 * @version 2.0
 * @date 2024
 * 
 * @details
 * Содержит определения классов и структур для реализации игры Tetris.
 * Реализует паттерн MVC с конечным автоматом состояний.
 */

#pragma once
#include <string>
#include <utility>
#include <vector>

/**
 * @namespace s21::tetris
 * @brief Пространство имен игры Tetris
 * 
 * @details
 * Содержит все классы и структуры, связанные с игровой логикой Tetris.
 * Реализует паттерн MVC и конечный автомат для управления состояниями игры.
 */
namespace s21::tetris {

enum class Event {
  kStart,
  kTick,
  kPauseToggle,
  kMoveLeft,
  kMoveRight,
  kMoveDown,
  kRotate,
  kDrop,
  kReset,
  kQuit
};

enum class State { kInit, kRunning, kPaused, kGameOver };

struct Config {
  int width{10}, height{20};
  unsigned seed{0};
  std::string best_path{};
};

struct Cell {
  enum Type { kEmpty, kBlock, kCurrent, kGhost };
};

struct Snapshot {
  State state;
  int width, height;
  std::vector<Cell::Type> grid;
  int score{0}, best{0}, level{1}, speed_ms{500};

  struct {
    int x, y;
    int type;
    int rotation;
    bool visible;
  } current;

  struct {
    int x, y;
    bool visible;
  } ghost;
};

class Engine {
 public:
  explicit Engine(Config cfg = {});
  State state() const;
  void dispatch(Event e);
  Snapshot snapshot() const;

 private:
  struct Impl;
  Impl* p_;
};

}  // namespace s21::tetris