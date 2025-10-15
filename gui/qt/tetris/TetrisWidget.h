/**
 * @file TetrisWidget.h
 * @brief Заголовочный файл виджета игры Tetris
 * @author BrickGame Team
 * @version 2.0
 * @date 2024
 * 
 * @details
 * Содержит определение класса TetrisWidget - графического виджета
 * для отображения игры Tetris в Qt интерфейсе.
 */

#pragma once
#include <QWidget>

#include "../../brick_game/tetris/backend/engine.h"

/**
 * @class TetrisWidget
 * @brief Виджет отображения игры Tetris
 * 
 * @details
 * Наследуется от QWidget и отвечает за отрисовку игрового поля Tetris.
 * Реализует паттерн MVC в роли View. Отображает падающие фигуры,
 * призрачные фигуры, сетку и игровую информацию.
 */
class TetrisWidget : public QWidget {
  Q_OBJECT
 public:
  /**
   * @brief Конструктор виджета Tetris
   * @param parent Родительский виджет
   */
  explicit TetrisWidget(QWidget* parent = nullptr);
  
  /**
   * @brief Установить снимок состояния игры
   * @param s Снимок состояния для отображения
   */
  void setSnapshot(const s21::tetris::Snapshot& s);
  
  /**
   * @brief Получить рекомендуемый размер виджета
   * @return Рекомендуемый размер
   */
  QSize sizeHint() const override;

 protected:
  /**
   * @brief Отрисовка виджета
   * @param event Событие отрисовки
   */
  void paintEvent(QPaintEvent*) override;

 private:
  s21::tetris::Snapshot snap_;  ///< Текущий снимок состояния игры
  int cell_ = 28;               ///< Размер клетки в пикселях
  int pad_ = 12;                ///< Отступы от краев

  /**
   * @brief Получить цвет для типа клетки
   * @param t Тип клетки
   * @return Цвет клетки
   */
  QColor colorForCell(s21::tetris::Cell::Type t) const;

  /**
   * @brief Отрисовать отдельную клетку
   * @param p Объект рисования
   * @param x Координата X
   * @param y Координата Y
   * @param type Тип клетки
   */
  void drawCell(QPainter& p, int x, int y, s21::tetris::Cell::Type type);
  
  /**
   * @brief Отрисовать тетромино
   * @param p Объект рисования
   * @param ox Смещение X игрового поля
   * @param oy Смещение Y игрового поля
   * @param x Координата X фигуры
   * @param y Координата Y фигуры
   * @param type Тип фигуры
   * @param rotation Поворот фигуры
   * @param cellType Тип клетки для отрисовки
   */
  void drawTetromino(QPainter& p, int ox, int oy, int x, int y, int type,
                     int rotation, s21::tetris::Cell::Type cellType);
  
  /**
   * @brief Проверить наличие блока в тетромино
   * @param type Тип фигуры
   * @param rotation Поворот фигуры
   * @param r Строка в маске 4x4
   * @param c Столбец в маске 4x4
   * @return true если блок существует
   */
  bool hasBlock(int type, int rotation, int r, int c);
};