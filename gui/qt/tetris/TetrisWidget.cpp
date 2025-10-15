#include <QPainter>
#include <algorithm>

#include "TetrisWidget.h"

TetrisWidget::TetrisWidget(QWidget* parent) : QWidget(parent) {
  setMinimumSize(360, 600);
}

void TetrisWidget::setSnapshot(const s21::tetris::Snapshot& s) {
  snap_ = s;
  update();
}

QSize TetrisWidget::sizeHint() const {
  return QSize(snap_.width * cell_ + pad_ * 2, snap_.height * cell_ + pad_ * 2);
}

QColor TetrisWidget::colorForCell(s21::tetris::Cell::Type t) const {
  switch (t) {
    case s21::tetris::Cell::kBlock:
      return QColor("#58a6ff");
    case s21::tetris::Cell::kCurrent:
      return QColor("#f85149");
    case s21::tetris::Cell::kGhost:
      return QColor("#6b7280");
    default:
      return QColor(0, 0, 0, 0);
  }
}

void TetrisWidget::paintEvent(QPaintEvent*) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing, true);
  p.setRenderHint(QPainter::TextAntialiasing, true);

  QLinearGradient g(0, 0, 0, height());
  g.setColorAt(0.0, QColor("#0d1117"));
  g.setColorAt(1.0, QColor("#161b22"));
  p.fillRect(rect(), g);

  const int cols = snap_.width;
  const int rows = snap_.height;

  cell_ = std::max(10, std::min((width() - 2 * pad_) / std::max(1, cols),
                                (height() - 2 * pad_) / std::max(1, rows)));
  const int boardW = cell_ * cols;
  const int boardH = cell_ * rows;
  const int ox = (width() - boardW) / 2;
  const int oy = (height() - boardH) / 2;

  p.fillRect(QRect(ox, oy, boardW, boardH), QColor(255, 255, 255, 8));

  QPen gridPen(QColor(255, 255, 255, 20));
  gridPen.setWidthF(1.0);
  p.setPen(gridPen);
  for (int c = 0; c <= cols; ++c)
    p.drawLine(ox + c * cell_, oy, ox + c * cell_, oy + boardH);
  for (int r = 0; r <= rows; ++r)
    p.drawLine(ox, oy + r * cell_, ox + boardW, oy + r * cell_);

  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < cols; ++c) {
      auto t = snap_.grid[r * cols + c];
      if (t == s21::tetris::Cell::kBlock) {
        drawCell(p, ox + c * cell_, oy + r * cell_, t);
      }
    }
  }

  if (snap_.ghost.visible) {
    drawTetromino(p, ox, oy, snap_.ghost.x, snap_.ghost.y, snap_.current.type,
                  snap_.current.rotation, s21::tetris::Cell::kGhost);
  }

  if (snap_.current.visible) {
    drawTetromino(p, ox, oy, snap_.current.x, snap_.current.y,
                  snap_.current.type, snap_.current.rotation,
                  s21::tetris::Cell::kCurrent);
  }

  using S = s21::tetris::State;
  QString st;
  if (snap_.state == S::kPaused)
    st = "PAUSED";
  else if (snap_.state == S::kGameOver)
    st = "GAME OVER";
  else if (snap_.state == S::kInit)
    st = "PRESS ENTER";

  if (!st.isEmpty()) {
    QFont f = p.font();
    f.setBold(true);
    f.setPointSize(std::max(12, cell_));
    p.setFont(f);
    p.setPen(QColor("#c9d1d9"));
    p.drawText(QRect(ox, oy + boardH / 2 - cell_, boardW, cell_ * 2),
               Qt::AlignCenter, st);
  }
}

void TetrisWidget::drawCell(QPainter& p, int x, int y,
                            s21::tetris::Cell::Type type) {
  QRect cell(x, y, cell_, cell_);
  QRect innerCell = cell.adjusted(2, 2, -2, -2);

  if (type == s21::tetris::Cell::kGhost) {
    QColor ghostColor = colorForCell(type);
    ghostColor.setAlpha(80);
    p.fillRect(innerCell, ghostColor);

    p.setPen(QPen(ghostColor.darker(150), 1));
    p.drawRect(innerCell.adjusted(0, 0, -1, -1));
    return;
  }

  QRect shadowCell = cell.adjusted(2, 2, 0, 0);
  p.fillRect(shadowCell, QColor(0, 0, 0, 100));

  QLinearGradient gradient(innerCell.topLeft(), innerCell.bottomRight());
  QColor baseColor = colorForCell(type);
  gradient.setColorAt(0, baseColor.lighter(120));
  gradient.setColorAt(1, baseColor.darker(120));

  p.fillRect(innerCell, gradient);

  p.setPen(QPen(baseColor.lighter(150), 1));
  p.drawRect(innerCell.adjusted(0, 0, -1, -1));
}

void TetrisWidget::drawTetromino(QPainter& p, int ox, int oy, int x, int y,
                                 int type, int rotation,
                                 s21::tetris::Cell::Type cellType) {
  for (int r = 0; r < 4; r++) {
    for (int c = 0; c < 4; c++) {
      if (hasBlock(type, rotation, r, c)) {
        int board_x = x + c;
        int board_y = y + r;
        if (board_x >= 0 && board_x < snap_.width && board_y >= 0 &&
            board_y < snap_.height) {
          drawCell(p, ox + board_x * cell_, oy + board_y * cell_, cellType);
        }
      }
    }
  }
}

bool TetrisWidget::hasBlock(int type, int rotation, int r, int c) {
  static const uint16_t shapes[7][4] = {
      {0x0F0, 0x4444, 0xF00, 0x2222},   {0x0660, 0x0660, 0x0660, 0x0660},
      {0x0270, 0x0464, 0x0E40, 0x2620}, {0x0360, 0x0462, 0x06C0, 0x4620},
      {0x0630, 0x0264, 0x0C60, 0x2640}, {0x0740, 0x0622, 0x02E0, 0x4460},
      {0x0710, 0x0226, 0x08E0, 0x6440}};

  if (type < 0 || type >= 7) return false;
  uint16_t mask = shapes[type][rotation];
  int bit = r * 4 + c;
  return (mask >> bit) & 1u;
}