#include <QFont>
#include <QLinearGradient>
#include <QPainter>
#include <algorithm>

#include "SnakeWidget.h"

SnakeWidget::SnakeWidget(QWidget* parent) : QWidget(parent) {
  setMinimumSize(400, 400);
}

void SnakeWidget::setSnapshot(const s21::snake::Snapshot& snap) {
  snap_ = snap;
  update();
}

QSize SnakeWidget::sizeHint() const { return QSize(640, 640); }

void SnakeWidget::paintEvent(QPaintEvent*) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing, true);
  p.setRenderHint(QPainter::TextAntialiasing, true);
  p.setRenderHint(QPainter::SmoothPixmapTransform, true);

  const auto snap = snap_;
  const int cols = std::max(1, snap.width);
  const int rows = std::max(1, snap.height);

  const int W = width();
  const int H = height();

  cell_ = std::max(1, std::min((W - 2 * pad_) / cols, (H - 2 * pad_) / rows));

  const int boardW = cell_ * cols;
  const int boardH = cell_ * rows;

  const int ox = (W - boardW) / 2;
  const int oy = (H - boardH) / 2;

  {
    QLinearGradient g(0, 0, 0, H);
    g.setColorAt(0.0, bgTop_);
    g.setColorAt(1.0, bgBot_);
    p.fillRect(rect(), g);
  }

  p.fillRect(QRect(ox, oy, boardW, boardH), QColor(255, 255, 255, 10));

  p.setPen(QColor(200, 200, 200, 120));
  p.drawRect(ox - 1, oy - 1, boardW + 2, boardH + 2);

  {
    QPen penGrid(grid_);
    penGrid.setWidthF(1.0);
    p.setPen(penGrid);
    for (int c = 0; c <= cols; ++c) {
      const int x = ox + c * cell_;
      p.drawLine(x, oy, x, oy + boardH);
    }
    for (int r = 0; r <= rows; ++r) {
      const int y = oy + r * cell_;
      p.drawLine(ox, y, ox + boardW, y);
    }
  }

  p.setPen(Qt::NoPen);
  for (int y = 0; y < rows; ++y) {
    for (int x = 0; x < cols; ++x) {
      const auto t = snap.grid.empty() ? s21::snake::Cell::kEmpty
                                       : snap.grid[y * cols + x];
      const QRect r(ox + x * cell_, oy + y * cell_, cell_, cell_);

      if (t == s21::snake::Cell::kFood) {
        const int m = std::max(1, cell_ / 6);
        QRect inner = r.adjusted(m, m, -m, -m);
        p.setBrush(QColor(food_.red(), food_.green(), food_.blue(), 80));
        const int halo = std::max(1, cell_ / 10);
        p.drawEllipse(inner.adjusted(-halo, -halo, +halo, +halo));
        p.setBrush(food_);
        p.drawEllipse(inner);
      } else if (t == s21::snake::Cell::kSnake) {
        const int m = std::max(1, cell_ / 8);
        QRect inner = r.adjusted(m, m, -m, -m);
        p.setBrush(body_);
        p.drawRoundedRect(inner, cell_ * 0.25, cell_ * 0.25);
      } else {
        p.setBrush(QColor(25, 25, 25));
        p.drawRect(r);
      }
    }
  }

  if (!snap.snake.empty()) {
    auto [hx, hy] = snap.snake.front();
    const QRect r(ox + hx * cell_, oy + hy * cell_, cell_, cell_);
    const int m = std::max(1, cell_ / 8);
    QRect inner = r.adjusted(m, m, -m, -m);

    p.setBrush(head_);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(inner, cell_ * 0.2, cell_ * 0.2);

    int dx = 0, dy = 0;
    if (snap.snake.size() > 1) {
      auto [nx, ny] = snap.snake[1];
      dx = std::clamp(hx - nx, -1, 1);
      dy = std::clamp(hy - ny, -1, 1);
    }

    const int eyeR = std::max(2, cell_ / 7);
    const int pupilR = std::max(1, cell_ / 12);
    const int eyeOffset = std::max(2, cell_ / 5);
    const int lookShift = std::max(1, cell_ / 10);

    QPoint center(inner.center());
    QPoint left = center + QPoint(-eyeOffset, -eyeOffset / 2);
    QPoint right = center + QPoint(+eyeOffset, -eyeOffset / 2);

    p.setBrush(Qt::white);
    p.drawEllipse(QRect(left.x() - eyeR, left.y() - eyeR, 2 * eyeR, 2 * eyeR));
    p.drawEllipse(
        QRect(right.x() - eyeR, right.y() - eyeR, 2 * eyeR, 2 * eyeR));

    QPoint shift(dx * lookShift, dy * lookShift);
    p.setBrush(Qt::black);
    p.drawEllipse(QRect((left + shift).x() - pupilR,
                        (left + shift).y() - pupilR, 2 * pupilR, 2 * pupilR));
    p.drawEllipse(QRect((right + shift).x() - pupilR,
                        (right + shift).y() - pupilR, 2 * pupilR, 2 * pupilR));
  }

  using S = s21::snake::State;
  if (snap.state == S::kPaused || snap.state == S::kGameOver) {
    p.fillRect(QRect(ox, oy, boardW, boardH), QColor(0, 0, 0, 100));
    const QString msg = (snap.state == S::kPaused) ? "PAUSED" : "GAME OVER";
    QFont f("Menlo", std::max(12, int(cell_ * 1.2)), QFont::Bold);
    p.setFont(f);
    QFontMetrics fm(f);
    const int tw = fm.horizontalAdvance(msg);
    const int th = fm.ascent();
    const int tx = ox + (boardW - tw) / 2;
    const int ty = oy + (boardH + th) / 2;
    p.setPen(QColor(0, 0, 0, 160));
    p.drawText(tx + 2, ty + 2, msg);
    p.setPen(Qt::white);
    p.drawText(tx, ty, msg);
  }
}