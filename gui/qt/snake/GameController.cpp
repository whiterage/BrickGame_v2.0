#include <QKeyEvent>

#include "GameController.h"
#include "SidebarWidget.h"
#include "SnakeWidget.h"

using namespace s21::snake;

GameController::GameController(SnakeWidget* view, QObject* parent)
    : QObject(parent), view_(view) {
  connect(&timer_, &QTimer::timeout, this, &GameController::onTick);
}

void GameController::start() {
  model_.dispatch(Event::kStart);
  syncView();
  updateTimerFromModel();
  timer_.start();
}

void GameController::onTick() {
  auto s = model_.snapshot();
  if (s.state == State::kRunning) {
    model_.dispatch(Event::kTick);
    syncView();
    updateTimerFromModel();
  }
}

void GameController::onKeyPressed(int key) {
  Event ev;
  bool has = false;
  switch (key) {
    case Qt::Key_Up:
      ev = Event::kMoveUp;
      has = true;
      break;
    case Qt::Key_Down:
      ev = Event::kMoveDown;
      has = true;
      break;
    case Qt::Key_Left:
      ev = Event::kMoveLeft;
      has = true;
      break;
    case Qt::Key_Right:
      ev = Event::kMoveRight;
      has = true;
      break;
    case Qt::Key_Space:
      ev = Event::kPauseToggle;
      has = true;
      break;
    case Qt::Key_R:
      ev = Event::kReset;
      has = true;
      break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
      ev = Event::kStart;
      has = true;
      break;
    case Qt::Key_Q:
      ev = Event::kQuit;
      has = true;
      break;
  }
  if (!has) return;

  if (ev == Event::kQuit) {
    emit quitRequested();
  } else {
    model_.dispatch(ev);
    syncView();
    updateTimerFromModel();
  }
}

void GameController::syncView() {
  auto snap = model_.snapshot();
  if (view_) view_->setSnapshot(snap);
  if (sidebar_) sidebar_->setSnapshot(snap);
  updateTimerFromModel();
}

void GameController::updateTimerFromModel() {
  int ms = model_.snapshot().speed_ms;
  if (ms < 10) ms = 10;
  timer_.setInterval(ms);
}