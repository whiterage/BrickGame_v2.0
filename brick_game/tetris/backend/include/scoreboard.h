/**
 * @file scoreboard.h
 * @brief Таблица рекордов: загрузка/сохранение TSV и вставка результата.
 * @defgroup scores Таблица рекордов
 * @{
 */
#pragma once
#include <stddef.h>

#define TETRIS_NAME_MAX 15    /**< Макс. длина имени игрока. \ingroup scores */
#define TETRIS_MAX_SCORES 100 /**< Макс. записей в таблице. \ingroup scores */

/** @brief Одна запись в таблице рекордов. \ingroup scores */
typedef struct {
  char name[TETRIS_NAME_MAX + 1];
  int score;
} score_entry_t;

/** @brief Таблица рекордов. \ingroup scores */
typedef struct {
  score_entry_t list[TETRIS_MAX_SCORES];
  int count;
} scoreboard_t;

/** @brief Путь по умолчанию: $HOME/.tetris_scores.tsv (или ./
 * .tetris_scores.tsv). \ingroup scores */
int sc_default_path(char *buf, size_t buflen);

/** @brief Загрузка таблицы из TSV. \ingroup scores */
int sc_load(scoreboard_t *tb, const char *path);

/** @brief Сохранение таблицы в TSV. \ingroup scores */
int sc_save(const scoreboard_t *tb, const char *path);

/**
 * @brief Добавить результат и отсортировать по убыванию.
 * Если таблица полна и результат ниже последнего — игнорируется.
 * @ingroup scores
 */
int sc_submit(scoreboard_t *tb, const char *name, int score);

/** @} */  // end of group scores