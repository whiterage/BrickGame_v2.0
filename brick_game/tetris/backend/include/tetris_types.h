/**
 * @file tetris_types.h
 * @brief Базовые типы и константы игры (размеры, фигуры, состояния, сигналы).
 */
#ifndef TETRIS_TYPES_H
#define TETRIS_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/** Высота игрового поля в клетках. */
#define TETRIS_ROWS 20
/** Ширина игрового поля в клетках. */
#define TETRIS_COLS 10
/** Ширина/высота маски тетрамино (4x4). */
#define TETROMINO_SIZE 4

/** Тип клетки поля. 0 — пусто, >0 — индекс фигуры. */
typedef uint8_t cell_t;

/** @brief Игровое поле 10x20. */
typedef struct {
  cell_t grid[TETRIS_ROWS][TETRIS_COLS];
} board_t;

/** @brief Виды тетрамино. */
typedef enum {
  TETROMINO_I,
  TETROMINO_O,
  TETROMINO_T,
  TETROMINO_S,
  TETROMINO_Z,
  TETROMINO_J,
  TETROMINO_L,
  TETROMINO_NONE
} tetromino_type;

/** @brief Поворот фигуры (кратно 90°). */
typedef enum { ROTATE_0, ROTATE_90, ROTATE_180, ROTATE_270 } rotation_t;

/** @brief Параметры текущей фигуры. */
typedef struct {
  tetromino_type type; /**< Вид фигуры. */
  rotation_t rotation; /**< Текущий поворот. */
  int x;               /**< Координата X (колонка), левая граница фигуры. */
  int y;               /**< Координата Y (строка), верхняя граница фигуры. */
} tetromino_t;

/** @brief Текущая статистика игры. */
typedef struct {
  int score;         /**< Счёт. */
  int level;         /**< Уровень (1..10). */
  int lines_cleared; /**< Кол-во убранных линий. */
  int best_score;    /**< Лучший счёт (для отображения/сохранения). */
  int speed;         /**< Произвольная метрика скорости (для HUD). */
} game_stats_t;

/** @brief Состояния конечного автомата. */
typedef enum {
  START,
  SPAWN,
  FALL,
  LOCK,
  LINE_CLEAR,
  PAUSE,
  GAMEOVER,
  EXIT_STATE
} game_state;

/** @brief Сигналы ввода (управление/кнопки/события UI). */
typedef enum {
  NOSIG = 0,
  MOVE_LEFT,
  MOVE_RIGHT,
  MOVE_DOWN,
  ROTATE,
  HARD_DROP,
  ENTER_BTN,
  ESCAPE_BTN,
  PAUSE_BTN,
  UI_SHOW_SCORES /**< Показать таблицу рекордов (UI-горячая клавиша). */
} signals;

#endif