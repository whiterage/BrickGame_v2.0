/**
 * @file SnakeWidget.h
 * @brief Заголовочный файл виджета игры Snake
 * @author BrickGame Team
 * @version 2.0
 * @date 2024
 * 
 * @details
 * Содержит определение класса SnakeWidget - графического виджета
 * для отображения игры Snake в Qt интерфейсе.
 */

#pragma once
#include <QWidget>

#include "../../brick_game/snake/backend.h"

/**
 * @class SnakeWidget
 * @brief Виджет отображения игры Snake
 * 
 * @details
 * Наследуется от QWidget и отвечает за отрисовку игрового поля Snake.
 * Реализует паттерн MVC в роли View. Отображает змейку, еду, сетку
 * и игровую информацию с современным дизайном.
 */
class SnakeWidget : public QWidget {
  Q_OBJECT
 public:
  /**
   * @brief Конструктор виджета Snake
   * @param parent Родительский виджет
   */
  explicit SnakeWidget(QWidget* parent = nullptr);

  /**
   * @brief Установить снимок состояния игры
   * @param snap Снимок состояния для отображения
   */
  void setSnapshot(const s21::snake::Snapshot& snap);

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
  s21::snake::Snapshot snap_;              ///< Текущий снимок состояния игры
  int cell_ = 20;                          ///< Размер клетки в пикселях
  int pad_ = 12;                           ///< Отступы от краев
  QColor bgTop_ = QColor(18, 18, 28);     ///< Цвет верхней части фона
  QColor bgBot_ = QColor(8, 8, 14);       ///< Цвет нижней части фона
  QColor grid_ = QColor(255, 255, 255, 18); ///< Цвет сетки
  QColor food_ = QColor(255, 105, 180);    ///< Цвет еды
  QColor body_ = QColor(60, 190, 120);     ///< Цвет тела змейки
  QColor head_ = QColor(80, 230, 160);      ///< Цвет головы змейки
  QColor hud_ = QColor(255, 255, 255);     ///< Цвет HUD элементов
};