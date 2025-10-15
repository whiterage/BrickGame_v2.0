/**
 * @file backend.h
 * @brief Заголовочный файл игровой логики Snake
 * @author BrickGame Team
 * @version 2.0
 * @date 2024
 * 
 * @details
 * Содержит определения классов и структур для реализации игры Snake.
 * Реализует паттерн MVC с конечным автоматом состояний.
 */

#pragma once
#include <deque>
#include <string>
#include <utility>
#include <vector>

/**
 * @namespace s21::snake
 * @brief Пространство имен игры Snake
 * 
 * @details
 * Содержит все классы и структуры, связанные с игровой логикой Snake.
 * Реализует паттерн MVC и конечный автомат для управления состояниями игры.
 */
namespace s21::snake {
/**
 * @enum Event
 * @brief События игры Snake
 * 
 * @details
 * Перечисление всех возможных событий в игре Snake.
 * Используется в конечном автомате для обработки пользовательского ввода
 * и игровых событий.
 */
enum class Event {
  kStart,        ///< Запуск игры
  kTick,         ///< Игровой тик (движение змейки)
  kPauseToggle,  ///< Переключение паузы
  kMoveUp,       ///< Движение вверх
  kMoveDown,     ///< Движение вниз
  kMoveLeft,     ///< Движение влево
  kMoveRight,    ///< Движение вправо
  kReset,        ///< Сброс игры
  kQuit          ///< Выход из игры
};

/**
 * @enum State
 * @brief Состояния игры Snake
 * 
 * @details
 * Перечисление всех возможных состояний игры.
 * Используется в конечном автомате для управления логикой игры.
 */
enum class State { 
  kInit,      ///< Начальное состояние
  kReady,     ///< Готовность к запуску
  kRunning,   ///< Игра активна
  kPaused,    ///< Игра на паузе
  kGameOver   ///< Игра завершена
};

/**
 * @struct Config
 * @brief Конфигурация игры Snake
 * 
 * @details
 * Содержит настройки игрового поля и параметры игры.
 */
struct Config {
  int width{20}, height{20};    ///< Размеры игрового поля
  unsigned seed{0};             ///< Семя для генератора случайных чисел
  bool wrap{false};             ///< Обертывание змейки через границы
  std::string best_path{};      ///< Путь к файлу с лучшим результатом
};

/**
 * @struct Cell
 * @brief Типы клеток игрового поля
 * 
 * @details
 * Определяет возможные типы клеток на игровом поле.
 */
struct Cell {
  /**
   * @enum Type
   * @brief Типы клеток
   */
  enum Type { 
    kEmpty,  ///< Пустая клетка
    kSnake,  ///< Клетка змейки
    kFood    ///< Клетка с едой
  };
};

/**
 * @struct Snapshot
 * @brief Снимок состояния игры
 * 
 * @details
 * Содержит полное состояние игры в определенный момент времени.
 * Используется для отображения и сохранения состояния.
 */
struct Snapshot {
  State state;                                    ///< Текущее состояние игры
  int width, height;                              ///< Размеры поля
  std::vector<Cell::Type> grid;                  ///< Игровое поле
  int score, best, level, speed_ms;               ///< Игровые параметры
  std::vector<std::pair<int, int>> snake;         ///< Координаты змейки
  std::pair<int, int> food;                       ///< Координаты еды
};

/**
 * @struct Point
 * @brief Точка на игровом поле
 * 
 * @details
 * Простая структура для представления координат.
 */
struct Point {
  int x{0}, y{0};  ///< Координаты точки
};

/**
 * @enum Direction
 * @brief Направления движения
 * 
 * @details
 * Перечисление возможных направлений движения змейки.
 */
enum class Direction { 
  kUp,     ///< Вверх
  kDown,   ///< Вниз
  kLeft,   ///< Влево
  kRight   ///< Вправо
};
/**
 * @class Engine
 * @brief Игровой движок Snake
 * 
 * @details
 * Основной класс, реализующий игровую логику Snake.
 * Содержит конечный автомат состояний и управляет игровым процессом.
 * Реализует паттерн MVC в роли Model.
 */
class Engine {
 public:
  /**
   * @brief Конструктор игрового движка
   * @param cfg Конфигурация игры
   */
  explicit Engine(Config cfg = {});
  
  /**
   * @brief Получить текущее состояние игры
   * @return Текущее состояние
   */
  State state() const;
  
  /**
   * @brief Обработать событие
   * @param e Событие для обработки
   */
  void dispatch(Event e);
  
  /**
   * @brief Получить снимок состояния игры
   * @return Снимок текущего состояния
   */
  Snapshot snapshot() const;
  
  /**
   * @brief Деструктор
   */
  ~Engine();

 private:
  Config cfg_;                           ///< Конфигурация игры
  State state_{State::kInit};            ///< Текущее состояние
  Direction dir_{Direction::kRight};     ///< Направление движения
  std::deque<Point> snake_;              ///< Координаты змейки
  std::vector<Cell::Type> grid_;         ///< Игровое поле
  int score_{0};                         ///< Текущий счет
  std::pair<int, int> food_{-1, -1};   ///< Координаты еды
  unsigned rng_state_{0};                 ///< Состояние ГПСЧ
  int best_{0};                          ///< Лучший результат
  std::string best_path_;                ///< Путь к файлу с лучшим результатом
  int level_{1};                         ///< Текущий уровень
  int speed_ms_{200};
  int levelForScore(int score) const;
  int speedForLevel(int level) const;
  void recomputeLevelAndSpeed();

  int W() const { return cfg_.width; }
  int H() const { return cfg_.height; }
  int idx(int x, int y) const { return y * W() + x; }
  bool isOpposite(Direction a, Direction b) const;
  void resetGrid();
  void placeInitialSnake();
  void applySnakeToGrid();
  void step();
  void spawnFood();
  bool isSnakeCell(int x, int y) const;
  unsigned nextRand();
  int loadBestFromFile(const std::string& path);
  void saveBestToFile(const std::string& path, int);
  std::string defaultBestPath() const;
  void UpdateBest();
};

}  // namespace s21::snake