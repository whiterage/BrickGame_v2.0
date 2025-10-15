/**
 * @file tetris_backend.h
 * @brief Внутренняя логика тетриса: столкновения, движение, поворот, линии.
 * @defgroup core Бэкенд-логика (Core)
 * @{
 */
#ifndef TETRIS_BACKEND_H
#define TETRIS_BACKEND_H

#include "tetris_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Битовые маски фигур (7 видов × 4 поворота). \ingroup core */
extern const uint16_t TETROMINO_SHAPES[7][4];

/**
 * @brief Проверяет, занята ли ячейка (r,c) в 4×4 маске конкретной фигуры.
 * @ingroup core
 */
bool shape_has_block(tetromino_type type, rotation_t rot, int r, int c);

/**
 * @brief Инициализация поля/статистики и генерация текущей/следующей фигуры.
 * @ingroup core
 */
void bg_init(board_t* board, game_stats_t* stats, tetromino_t* current,
             tetromino_t* next);

/**
 * @brief Появление новой фигуры: current <- next, next = случайная.
 * @return 1 если коллизия на спауне (GAMEOVER), иначе 0.
 * @ingroup core
 */
int bg_spawn(board_t* board, tetromino_t* current, tetromino_t* next);

/**
 * @brief Попытка сдвига текущей фигуры.
 * @return true если удалось, иначе false.
 * @ingroup core
 */
bool bg_try_move(board_t* board, tetromino_t* current, int dx, int dy);

/**
 * @brief Попытка поворота (с простыми стен-клирами).
 * @param dir +1 по часовой, -1 против (используем +1).
 * @return true если удалось, иначе false.
 * @ingroup core
 */
bool bg_try_rotate(board_t* board, tetromino_t* current, int dir);

/**
 * @brief Единичный тик падения: либо опускает фигуру, либо фиксирует её.
 * @return true если фигура продолжает падать, false если зафиксирована.
 * @ingroup core
 */
bool bg_tick(board_t* board, tetromino_t* current, game_stats_t* stats);

/**
 * @brief Удаляет заполненные строки, сдвигает поле, начисляет очки/уровень.
 * @return Количество очищенных строк [0..4].
 * @ingroup core
 */
int bg_clear_full_lines(board_t* board, game_stats_t* stats);

/**
 * @brief Проверка столкновений для фигуры с оффсетом (dx,dy).
 * @return true если есть коллизия (стена/дно/занято), иначе false.
 * @ingroup core
 */
bool bg_collides(board_t* board, const tetromino_t* t, int dx, int dy);

/**
 * @brief Применяет сигнал управления к текущей фигуре (L/R/D/Rotate).
 * @return true если произошли изменения (двиг/поворот), иначе false.
 * @ingroup core
 */
bool bg_apply_input(board_t* board, tetromino_t* current, signals sig);

/**
 * @brief Жёсткое опускание фигуры до упора, фиксация и очистка линий.
 * @return Кол-во очищенных линий после фиксации.
 * @ingroup core
 */
int bg_hard_drop(board_t* board, tetromino_t* current, game_stats_t* stats);

/**
 * @brief Вычисляет "тень" фигуры - позицию, куда она упадет.
 * @param board Игровое поле
 * @param cur Текущая фигура
 * @param out_ghost Выходная тень фигуры
 * @ingroup core
 */
void bg_compute_ghost(const board_t* board, const tetromino_t* cur,
                      tetromino_t* out_ghost);

#ifdef __cplusplus
}
#endif

#endif
/** @} */  // end of group core