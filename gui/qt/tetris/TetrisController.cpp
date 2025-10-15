#include <QKeyEvent>

#include "../snake/SidebarWidget.h"
#include "TetrisController.h"
#include "TetrisWidget.h"

using namespace s21::tetris;

TetrisController::TetrisController(TetrisWidget* view, QObject* parent)
    : QObject(parent), view_(view) {
  connect(&timer_, &QTimer::timeout, this, &TetrisController::onTick);
}

void TetrisController::start() {
  model_.dispatch(Event::kStart);
  syncView();
  updateTimerFromModel();
  timer_.start();
}

void TetrisController::onKeyPressed(int key) {
  Event e = Event::kTick;
  switch (key) {
    case Qt::Key_Return:
      e = Event::kStart;
      break;
    case Qt::Key_Space:
      e = Event::kPauseToggle;
      break;
    case Qt::Key_R:
      e = Event::kReset;
      break;
    case Qt::Key_Left:
      e = Event::kMoveLeft;
      break;
    case Qt::Key_Right:
      e = Event::kMoveRight;
      break;
    case Qt::Key_Up:
      e = Event::kRotate;
      break;
    case Qt::Key_Down:
      e = Event::kMoveDown;
      break;
    case Qt::Key_Q:
      e = Event::kQuit;
      emit quitRequested();
      break;
    default:
      e = Event::kTick;
      break;
  }
  model_.dispatch(e);
  syncView();
  updateTimerFromModel();
}

void TetrisController::onTick() {
  auto s = model_.snapshot();

  if (s.state == s21::tetris::State::kRunning) {
    model_.dispatch(Event::kTick);
    syncView();
    updateTimerFromModel();
  }
}

void TetrisController::syncView() {
  auto s = model_.snapshot();
  if (view_) view_->setSnapshot(s);
  if (sidebar_)
    sidebar_->setSnapshot({(s21::snake::State)0,

                           s.width,
                           s.height,
                           {},
                           s.score,
                           s.best,
                           s.level,
                           s.speed_ms,
                           {},
                           {0, 0}});
}

void TetrisController::updateTimerFromModel() {
  auto s = model_.snapshot();

  int ms = 500;
  if (s.level > 1) {
    ms = std::max(50, 500 - (s.level - 1) * 50);
  }
  if (timer_.interval() != ms) timer_.setInterval(ms);
}