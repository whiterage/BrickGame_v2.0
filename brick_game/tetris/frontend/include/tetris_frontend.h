/**
 * @file tetris_frontend.h
 * @brief Терминальный интерфейс (ncurses): окно, отрисовка, UI-экраны.
 * @defgroup gui Терминальный интерфейс (GUI)
 * @{
 */
#ifndef TETRIS_FRONTEND_H
#define TETRIS_FRONTEND_H

#include <ncurses.h>

#include "../../backend/include/tetris_backend.h"
#include "../../backend/include/tetris_types.h"

/** @brief Инициализация ncurses-окна и начального таймаута. \ingroup gui */
void win_init(int timeout_ms);

/** @brief Задать размер «крупного пикселя» (визуальный масштаб). \ingroup gui
 */
void fe_set_cell_size(int cell_w, int cell_h);

/** @brief Нарисовать прямоугольный блок. \ingroup gui */
void fe_draw_block(int top, int left, int h, int w, chtype ch, int color_pair);

/** @brief Основная отрисовка поля, текущей/следующей фигур, HUD. \ingroup gui
 */
void print_board(const board_t* board, const tetromino_t* current,
                 const tetromino_t* next, const game_stats_t* stats,
                 game_state state);

/** @brief Модальный ввод имени пользователя (блокирующий). \ingroup gui */
void fe_prompt_username(char* out, size_t n);

/** @brief Экран таблицы рекордов (блокирующий). \ingroup gui */
void fe_show_scores(void);

#endif
/** @} */  // end of group gui