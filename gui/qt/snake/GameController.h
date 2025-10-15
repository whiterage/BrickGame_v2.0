#pragma once
#include <QObject>
#include <QTimer>

#include "../../brick_game/snake/backend.h"

class SnakeWidget;
class SidebarWidget;

class GameController : public QObject {
  Q_OBJECT
 public:
  explicit GameController(SnakeWidget* view, QObject* parent = nullptr);

  void setSidebar(SidebarWidget* sb) { sidebar_ = sb; }

  void onKeyPressed(int key);

  void start();

 signals:
  void quitRequested();

 private slots:
  void onTick();

 private:
  s21::snake::Engine model_;
  SnakeWidget* view_{nullptr};
  SidebarWidget* sidebar_{nullptr};

  QTimer timer_;

  void syncView();
  void updateTimerFromModel();
};