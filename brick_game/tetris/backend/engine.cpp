#include "engine.h"

extern "C" {
#include "include/api.h"
#include "include/tetris_types.h"
}

#include <cstring>
#include <vector>

namespace s21::tetris {

struct Engine::Impl {
  tetris_t g;
  Config cfg;
};

static State map_state(game_state st) {
  switch (st) {
    case START:
      return State::kInit;
    case SPAWN:
    case FALL:
    case LOCK:
    case LINE_CLEAR:
      return State::kRunning;
    case PAUSE:
      return State::kPaused;
    case GAMEOVER:
      return State::kGameOver;
    case EXIT_STATE:
      return State::kGameOver;
  }
  return State::kInit;
}

static signals map_event_to_signal(Event e) {
  switch (e) {
    case Event::kStart:
      return ENTER_BTN;
    case Event::kTick:
      return NOSIG;
    case Event::kPauseToggle:
      return PAUSE_BTN;
    case Event::kMoveLeft:
      return MOVE_LEFT;
    case Event::kMoveRight:
      return MOVE_RIGHT;
    case Event::kMoveDown:
      return MOVE_DOWN;
    case Event::kRotate:
      return ROTATE;
    case Event::kDrop:
      return HARD_DROP;
    case Event::kQuit:
      return ESCAPE_BTN;
    case Event::kReset:
      return ENTER_BTN;
  }
  return NOSIG;
}

Engine::Engine(Config cfg) : p_(new Impl{}) {
  p_->cfg = cfg;
  tetris_init(&p_->g);
}

State Engine::state() const { return map_state(tetris_state(&p_->g)); }

void Engine::dispatch(Event e) {
  if (e == Event::kReset) {
    tetris_init(&p_->g);
    return;
  }

  tetris_input(&p_->g, map_event_to_signal(e));
}

Snapshot Engine::snapshot() const {
  Snapshot s{};
  s.state = state();
  s.width = TETRIS_COLS;
  s.height = TETRIS_ROWS;

  s.grid.resize(s.width * s.height, Cell::kEmpty);
  uint8_t raw[TETRIS_ROWS][TETRIS_COLS];
  tetris_export(&p_->g, raw);
  for (int r = 0; r < s.height; ++r) {
    for (int c = 0; c < s.width; ++c) {
      s.grid[r * s.width + c] = raw[r][c] ? Cell::kBlock : Cell::kEmpty;
    }
  }

  const game_stats_t* st = tetris_stats(&p_->g);
  s.score = st->score;
  s.level = st->level;
  s.speed_ms = st->speed;
  s.best = st->best_score;

  s.current.x = p_->g.cur.x;
  s.current.y = p_->g.cur.y;
  s.current.type = static_cast<int>(p_->g.cur.type);
  s.current.rotation = static_cast<int>(p_->g.cur.rotation);
  s.current.visible = (p_->g.state == SPAWN || p_->g.state == FALL);

  s.ghost.visible = s.current.visible;
  if (s.ghost.visible) {
    s.ghost.x = s.current.x;
    s.ghost.y = s.current.y;

    int max_y = s.height;
    for (int test_y = s.current.y; test_y < s.height; test_y++) {
      bool can_place = true;
      for (int r = 0; r < 4 && can_place; r++) {
        for (int c = 0; c < 4 && can_place; c++) {
          if (shape_has_block(static_cast<tetromino_type>(s.current.type),
                              static_cast<rotation_t>(s.current.rotation), r,
                              c)) {
            int board_y = test_y + r;
            int board_x = s.current.x + c;
            if (board_y >= s.height || board_x < 0 || board_x >= s.width ||
                (board_y >= 0 && raw[board_y][board_x])) {
              can_place = false;
            }
          }
        }
      }
      if (can_place) {
        max_y = test_y;
      } else {
        break;
      }
    }
    s.ghost.y = max_y;
  }

  return s;
}

}  // namespace s21::tetris