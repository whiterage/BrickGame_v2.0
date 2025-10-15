#include <stdlib.h>
#include <time.h>

#include "include/tetris_backend.h"

static void board_clear(board_t* b) {
  for (int i = 0; i < TETRIS_ROWS; ++i) {
    for (int j = 0; j < TETRIS_COLS; ++j) {
      b->grid[i][j] = 0;
    }
  }
}

static void stats_reset(game_stats_t* s) {
  s->score = 0;
  s->level = 1;
  s->lines_cleared = 0;
  s->best_score = 0;
  s->speed = 1;
}

static tetromino_type random_piece(void) {
  return (tetromino_type)(rand() % 7);
}

static void set_spawn_position(tetromino_t* t) {
  t->rotation = ROTATE_0;
  t->x = (TETRIS_COLS / 2) - 2;
  t->y = -1;
}

void bg_init(board_t* board, game_stats_t* stats, tetromino_t* current,
             tetromino_t* next) {
  srand((unsigned)time(NULL));
  board_clear(board);
  stats_reset(stats);

  next->type = random_piece();
  set_spawn_position(next);

  *current = *next;
}

bool bg_collides(board_t* board, const tetromino_t* t, int dx, int dy) {
  int min_r = 3, max_r = 0, min_c = 3, max_c = 0;
  bool empty_shape = true;
  for (int r_scan = 0; r_scan < 4; r_scan++) {
    for (int c_scan = 0; c_scan < 4; c_scan++) {
      if (shape_has_block(t->type, t->rotation, r_scan, c_scan)) {
        if (r_scan < min_r) min_r = r_scan;
        if (r_scan > max_r) max_r = r_scan;
        if (c_scan < min_c) min_c = c_scan;
        if (c_scan > max_c) max_c = c_scan;
        empty_shape = false;
      }
    }
  }

  if (empty_shape) return false;

  for (int r = min_r; r <= max_r; ++r) {
    for (int c = min_c; c <= max_c; ++c) {
      if (!shape_has_block(t->type, t->rotation, r, c)) continue;

      int wx = t->x + c + dx;
      int wy = t->y + r + dy;

      if (wx < 0 || wx >= TETRIS_COLS || wy >= TETRIS_ROWS) {
        return true;
      }

      if (wy >= 0 && board->grid[wy][wx] != 0) {
        return true;
      }
    }
  }

  return false;
}

bool bg_try_move(board_t* board, tetromino_t* current, int dx, int dy) {
  if (bg_collides(board, current, dx, dy)) {
    return false;
  }

  current->x += dx;
  current->y += dy;

  return true;
}

bool bg_try_rotate(board_t* board, tetromino_t* current, int dir) {
  rotation_t old_rot = current->rotation;
  int old_x = current->x;
  int old_y = current->y;

  rotation_t new_rot =
      (rotation_t)(((int)current->rotation + (dir > 0 ? 1 : 3)) & 3);
  current->rotation = new_rot;

  if (!bg_collides(board, current, 0, 0)) return true;

  current->x -= 1;
  if (!bg_collides(board, current, 0, 0)) {
    return true;
  }
  current->x = old_x;

  current->x += 1;
  if (!bg_collides(board, current, 0, 0)) {
    return true;
  }
  current->x = old_x;

  current->rotation = old_rot;
  current->x = old_x;
  current->y = old_y;
  return false;
}

int bg_clear_full_lines(board_t* board, game_stats_t* stats) {
  int cleared = 0;

  for (int r = TETRIS_ROWS - 1; r >= 0; --r) {
    bool full = true;
    for (int c = 0; c < TETRIS_COLS && full; ++c) {
      if (board->grid[r][c] == 0) {
        full = false;
      }
    }

    if (full) {
      ++cleared;

      for (int rr = r; rr > 0; --rr) {
        for (int c = 0; c < TETRIS_COLS; ++c) {
          board->grid[rr][c] = board->grid[rr - 1][c];
        }
      }

      for (int c = 0; c < TETRIS_COLS; ++c) {
        board->grid[0][c] = 0;
      }

      ++r;
    }
  }

  if (cleared > 0) {
    int points = 0;
    switch (cleared) {
      case 1:
        points = 100;
        break;
      case 2:
        points = 300;
        break;
      case 3:
        points = 700;
        break;
      case 4:
        points = 1500;
        break;
    }

    stats->score += points;
    stats->lines_cleared += cleared;

    int new_level = (stats->score / 600) + 1;
    if (new_level > stats->level && new_level <= 10) {
      stats->level = new_level;

      stats->speed = new_level;
    }
  }

  return cleared;
}

void bg_lock(board_t* board, const tetromino_t* current) {
  for (int r = 0; r < TETROMINO_SIZE; ++r) {
    for (int c = 0; c < TETROMINO_SIZE; ++c) {
      bool filled = shape_has_block(current->type, current->rotation, r, c);
      if (!filled) continue;

      int wx = current->x + c;
      int wy = current->y + r;

      if (wy < 0 || wy >= TETRIS_ROWS || wx < 0 || wx >= TETRIS_COLS) continue;

      board->grid[wy][wx] = (uint8_t)current->type + 1;
    }
  }
}

bool bg_tick(board_t* board, tetromino_t* current, game_stats_t* stats) {
  bool falling;
  bool blocked = bg_collides(board, current, 0, 1);

  if (blocked) {
    bg_lock(board, current);
    bg_clear_full_lines(board, stats);
    falling = false;
  } else {
    bg_try_move(board, current, 0, 1);
    falling = true;
  }
  return falling;
}

int bg_spawn(board_t* board, tetromino_t* current, tetromino_t* next) {
  int rc = 0;

  *current = *next;
  set_spawn_position(current);

  if (bg_collides(board, current, 0, 0)) {
    rc = 1;
  } else {
    next->type = random_piece();
    set_spawn_position(next);
  }

  return rc;
}

bool bg_apply_input(board_t* board, tetromino_t* current, signals sig) {
  bool changed = false;

  switch (sig) {
    case MOVE_LEFT:
      changed = bg_try_move(board, current, -1, 0);
      break;
    case MOVE_RIGHT:
      changed = bg_try_move(board, current, 1, 0);
      break;
    case MOVE_DOWN:
      changed = bg_try_move(board, current, 0, 1);
      break;
    case ROTATE:
      changed = bg_try_rotate(board, current, +1);
      break;
    default:
      changed = false;
      break;
  }

  return changed;
}

int bg_hard_drop(board_t* board, tetromino_t* current, game_stats_t* stats) {
  int steps = 0;
  while (!bg_collides(board, current, 0, +1)) {
    current->y++;
    steps++;
  }
  bg_lock(board, current);
  int cleared = bg_clear_full_lines(board, stats);
  stats->score += steps;
  return cleared;
}

void bg_compute_ghost(const board_t* board, const tetromino_t* cur,
                      tetromino_t* out_ghost) {
  board_t* b = (board_t*)board;
  *out_ghost = *cur;
  while (!bg_collides(b, out_ghost, 0, +1)) out_ghost->y++;
}