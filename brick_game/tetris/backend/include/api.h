/**
 * @file api.h
 * @brief Публичный API игры: обёртка над конечным автоматом + экспорт поля.
 * @defgroup api Публичный API
 * @{
 */
#ifndef TETRIS_API_H
#define TETRIS_API_H
#include "tetris_backend.h"
#include "tetris_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Композит: всё состояние игры для API-уровня. \ingroup api */
typedef struct {
  board_t board;
  tetromino_t cur;
  tetromino_t next;
  game_stats_t stats;
  game_state state;
} tetris_t;

/** @brief Полная инициализация игры и перевод в состояние START. \ingroup api
 */
void tetris_init(tetris_t* g);

/** @brief Подать сигнал во внутренний конечный автомат. \ingroup api */
void tetris_input(tetris_t* g, signals sig);

/**
 * @brief Скопировать текущее поле в внешний буфер [20×10].
 * @param out 0 — пусто, >0 — индекс типа (можно трактовать как цвет/ID).
 * @ingroup api
 */
void tetris_export(const tetris_t* g, uint8_t out[TETRIS_ROWS][TETRIS_COLS]);

/** @brief Доступ к статистике (read-only). \ingroup api */
static inline const game_stats_t* tetris_stats(const tetris_t* g) {
  return &g->stats;
}
/** @brief Текущее состояние КА. \ingroup api */
static inline game_state tetris_state(const tetris_t* g) { return g->state; }
/** @brief Следующая фигура для превью. \ingroup api */
static inline const tetromino_t* tetris_next(const tetris_t* g) {
  return &g->next;
}

#ifdef __cplusplus
}
#endif
#endif
/** @} */  // end of group api