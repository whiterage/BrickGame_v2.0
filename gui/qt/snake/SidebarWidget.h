#pragma once
#include <QPushButton>
#include <QWidget>

#include "../../brick_game/snake/backend.h"

class SidebarWidget : public QWidget {
  Q_OBJECT
 public:
  explicit SidebarWidget(QWidget* parent = nullptr);

  void setSnapshot(const s21::snake::Snapshot& snap);

  QPushButton* startButton() const { return start_btn_; }
  QPushButton* pauseButton() const { return pause_btn_; }
  QPushButton* resetButton() const { return reset_btn_; }
  QPushButton* quitButton() const { return quit_btn_; }

 protected:
  void paintEvent(QPaintEvent*) override;

 private:
  s21::snake::Snapshot snap_;
  QPushButton* start_btn_;
  QPushButton* pause_btn_;
  QPushButton* reset_btn_;
  QPushButton* quit_btn_;
};