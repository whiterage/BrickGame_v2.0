#pragma once
#include <QObject>
#include <QTimer>

#include "../../brick_game/tetris/backend/engine.h"

class TetrisWidget;
class SidebarWidget;

class TetrisController : public QObject {
  Q_OBJECT
 public:
  explicit TetrisController(TetrisWidget* view, QObject* parent = nullptr);
  void setSidebar(SidebarWidget* sb) { sidebar_ = sb; }

  void start();
  void onKeyPressed(int key);

 signals:
  void quitRequested();

 private slots:
  void onTick();

 private:
  s21::tetris::Engine model_;
  TetrisWidget* view_{nullptr};
  SidebarWidget* sidebar_{nullptr};
  QTimer timer_;

  void syncView();
  void updateTimerFromModel();
};