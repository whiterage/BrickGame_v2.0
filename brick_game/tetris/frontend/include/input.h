/**
 * @file input.h
 * @brief Маппинг нажатий клавиш терминала в сигналы игры.
 * @ingroup gui
 */
#pragma once
#include "../../backend/include/tetris_types.h"

/**
 * @brief Преобразовать символ/код ncurses в сигнал (для tetris_input).
 * @param ch Значение, возвращённое getch().
 * @return Сигнал управления/системный или NOSIG.
 * @ingroup gui
 */
signals get_signal(int ch);