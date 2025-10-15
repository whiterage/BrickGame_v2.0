#include <QFont>
#include <QPainter>
#include <QVBoxLayout>

#include "SidebarWidget.h"

SidebarWidget::SidebarWidget(QWidget* parent) : QWidget(parent) {
  setFixedWidth(280);

  start_btn_ = new QPushButton("Start Game", this);
  pause_btn_ = new QPushButton("Pause", this);
  reset_btn_ = new QPushButton("Reset", this);
  quit_btn_ = new QPushButton("Quit", this);

  QString buttonStyle =
      "QPushButton {"
      "  background-color: #333;"
      "  color: white;"
      "  border: 2px solid #555;"
      "  border-radius: 5px;"
      "  padding: 8px;"
      "  font-size: 12px;"
      "  font-weight: bold;"
      "}"
      "QPushButton:hover {"
      "  background-color: #444;"
      "  border-color: #666;"
      "}"
      "QPushButton:pressed {"
      "  background-color: #222;"
      "}";

  start_btn_->setStyleSheet(buttonStyle);
  pause_btn_->setStyleSheet(buttonStyle);
  reset_btn_->setStyleSheet(buttonStyle);
  quit_btn_->setStyleSheet(buttonStyle);

  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(10, 10, 10, 10);
  layout->setSpacing(10);

  layout->addWidget(start_btn_);
  layout->addWidget(pause_btn_);
  layout->addWidget(reset_btn_);
  layout->addWidget(quit_btn_);
  layout->addStretch();
}

void SidebarWidget::setSnapshot(const s21::snake::Snapshot& snap) {
  snap_ = snap;
  update();
}

void SidebarWidget::paintEvent(QPaintEvent*) {
  QPainter p(this);
  p.fillRect(rect(), QColor(20, 20, 20));

  const int margin = 15;
  const int lineHeight = 20;
  int y = margin + 150;

  p.setPen(Qt::white);
  QFont f = p.font();
  f.setPointSize(11);
  f.setBold(true);
  p.setFont(f);

  p.drawText(margin, y, "GAME INFO");
  y += lineHeight + 10;

  f.setBold(false);
  f.setPointSize(10);
  p.setFont(f);

  p.drawText(margin, y += lineHeight, QString("Score: %1").arg(snap_.score));
  p.drawText(margin, y += lineHeight, QString("Best:  %1").arg(snap_.best));
  p.drawText(margin, y += lineHeight, QString("Level: %1").arg(snap_.level));
  p.drawText(margin, y += lineHeight,
             QString("Speed: %1 ms").arg(snap_.speed_ms));

  y += 20;

  f.setBold(true);
  p.setFont(f);
  p.drawText(margin, y += lineHeight, "STATE:");
  f.setBold(false);
  p.setFont(f);

  QString stateText = "INIT";
  using S = s21::snake::State;
  if (snap_.state == S::kRunning) {
    stateText = "RUNNING";
  } else if (snap_.state == S::kPaused) {
    stateText = "PAUSED";
  } else if (snap_.state == S::kGameOver) {
    stateText = "GAME OVER";
  }

  p.drawText(margin, y += lineHeight, stateText);

  y += 30;

  f.setBold(true);
  p.setFont(f);
  p.drawText(margin, y += lineHeight, "CONTROLS:");
  f.setBold(false);
  f.setPointSize(9);
  p.setFont(f);

  p.drawText(margin, y += lineHeight, "↑↓←→ Move");
  p.drawText(margin, y += lineHeight, "Space Pause");
  p.drawText(margin, y += lineHeight, "Enter Start");
  p.drawText(margin, y += lineHeight, "R Reset");
  p.drawText(margin, y += lineHeight, "Q Quit");
}
