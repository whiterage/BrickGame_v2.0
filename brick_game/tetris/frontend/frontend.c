#include <ncurses.h>
#include <string.h>

#include "../backend/include/scoreboard.h"
#include "../backend/include/tetris_backend.h"
#include "include/tetris_frontend.h"

static int CELL_W = 4;
static int CELL_H = 2;
static const int OFF_Y = 1;
static const int OFF_X = 2;
static int HUD_X;
static const int HUD_Y = 1;

static inline int field_w(void) { return TETRIS_COLS * CELL_W; }
static inline int field_h(void) { return TETRIS_ROWS * CELL_H; }

enum { CP_I = 1, CP_O, CP_T, CP_S, CP_Z, CP_J, CP_L, CP_GHOST };

static int fe_color_for(tetromino_type t) {
  switch (t) {
    case TETROMINO_I:
      return CP_I;
    case TETROMINO_O:
      return CP_O;
    case TETROMINO_T:
      return CP_T;
    case TETROMINO_S:
      return CP_S;
    case TETROMINO_Z:
      return CP_Z;
    case TETROMINO_J:
      return CP_J;
    case TETROMINO_L:
      return CP_L;
    default:
      return 0;
  }
}

void fe_set_cell_size(int cell_w, int cell_h) {
  if (cell_w >= 1) CELL_W = cell_w;
  if (cell_h >= 1) CELL_H = cell_h;
  HUD_X = OFF_X + field_w() + 4;
}

void fe_draw_block(int top, int left, int h, int w, chtype ch, int color_pair) {
  attr_t on = A_NORMAL;
  if (color_pair > 0 && has_colors()) on |= COLOR_PAIR(color_pair) | A_BOLD;

  if (on != A_NORMAL) attron(on);
  for (int dy = 0; dy < h; ++dy) {
    for (int dx = 0; dx < w; ++dx) {
      mvaddch(top + dy, left + dx, ch);
    }
  }
  if (on != A_NORMAL) attroff(on);
}

void win_init(int timeout_ms) {
  initscr();
  set_escdelay(0);
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  nodelay(stdscr, FALSE);
  timeout(timeout_ms);
  curs_set(0);

  if (has_colors()) {
    start_color();
    use_default_colors();

    init_pair(CP_I, COLOR_CYAN, -1);
    init_pair(CP_O, COLOR_YELLOW, -1);
    init_pair(CP_T, COLOR_MAGENTA, -1);
    init_pair(CP_S, COLOR_GREEN, -1);
    init_pair(CP_Z, COLOR_RED, -1);
    init_pair(CP_J, COLOR_BLUE, -1);
    init_pair(CP_L, COLOR_WHITE, -1);
    init_pair(CP_GHOST, COLOR_WHITE, -1);
  }

  fe_set_cell_size(CELL_W, CELL_H);
}

void print_board(const board_t* board, const tetromino_t* current,
                 const tetromino_t* next, const game_stats_t* stats,
                 game_state state) {
  clear();

  const int FW = field_w();
  const int FH = field_h();

  for (int r = 0; r < TETRIS_ROWS; ++r) {
    for (int c = 0; c < TETRIS_COLS; ++c) {
      int top = OFF_Y + r * CELL_H;
      int left = OFF_X + c * CELL_W;
      if (board->grid[r][c]) {
        int col = fe_color_for((tetromino_type)(board->grid[r][c] - 1));
        fe_draw_block(top, left, CELL_H, CELL_W, ACS_BLOCK, col);
      } else {
        fe_draw_block(top, left, CELL_H, CELL_W, ' ', 0);
      }
    }
  }

  tetromino_t ghost = *current;
  bg_compute_ghost(board, current, &ghost);
  attron(A_DIM);
  {
    int col = fe_color_for(ghost.type);
    for (int r = 0; r < TETROMINO_SIZE; ++r) {
      for (int c = 0; c < TETROMINO_SIZE; ++c) {
        if (!shape_has_block(ghost.type, ghost.rotation, r, c)) continue;
        int wy = ghost.y + r, wx = ghost.x + c;
        if (wy < 0 || wy >= TETRIS_ROWS || wx < 0 || wx >= TETRIS_COLS)
          continue;
        int top = OFF_Y + wy * CELL_H;
        int left = OFF_X + wx * CELL_W;
        fe_draw_block(top, left, CELL_H, CELL_W, ACS_BLOCK, col);
      }
    }
  }
  attroff(A_DIM);

  {
    int col = fe_color_for(current->type);
    for (int r = 0; r < TETROMINO_SIZE; ++r) {
      for (int c = 0; c < TETROMINO_SIZE; ++c) {
        if (!shape_has_block(current->type, current->rotation, r, c)) continue;
        int wy = current->y + r, wx = current->x + c;
        if (wy < 0 || wy >= TETRIS_ROWS || wx < 0 || wx >= TETRIS_COLS)
          continue;
        int top = OFF_Y + wy * CELL_H;
        int left = OFF_X + wx * CELL_W;
        fe_draw_block(top, left, CELL_H, CELL_W, ACS_BLOCK, col);
      }
    }
  }

  for (int y = 0; y < FH; ++y) {
    mvaddch(OFF_Y + y, OFF_X - 1, ACS_VLINE);
    mvaddch(OFF_Y + y, OFF_X + FW, ACS_VLINE);
  }
  for (int x = 0; x < FW; ++x) {
    mvaddch(OFF_Y - 1, OFF_X + x, ACS_HLINE);
    mvaddch(OFF_Y + FH, OFF_X + x, ACS_HLINE);
  }
  mvaddch(OFF_Y - 1, OFF_X - 1, ACS_ULCORNER);
  mvaddch(OFF_Y - 1, OFF_X + FW, ACS_URCORNER);
  mvaddch(OFF_Y + FH, OFF_X - 1, ACS_LLCORNER);
  mvaddch(OFF_Y + FH, OFF_X + FW, ACS_LRCORNER);

  mvprintw(HUD_Y + 0, HUD_X, "Score: %d", stats->score);
  mvprintw(HUD_Y + 1, HUD_X, "Lines: %d", stats->lines_cleared);
  mvprintw(HUD_Y + 2, HUD_X, "Level: %d", stats->level);
  mvprintw(HUD_Y + 3, HUD_X, "Speed: %d", stats->speed);

  mvprintw(HUD_Y + 5, HUD_X, "Next:");
  {
    tetromino_t pv = *next;

    for (int r = 0; r < TETROMINO_SIZE; ++r) {
      for (int c = 0; c < TETROMINO_SIZE; ++c) {
        if (!shape_has_block(pv.type, pv.rotation, r, c)) continue;
        int top = HUD_Y + 7 + r * CELL_H;
        int left = HUD_X + c * CELL_W;
        fe_draw_block(top, left, CELL_H, CELL_W, ACS_BLOCK,
                      fe_color_for(pv.type));
      }
    }
  }

  switch (state) {
    case START:
      mvprintw(HUD_Y + 12, HUD_X, "TETRIS");
      mvprintw(HUD_Y + 14, HUD_X, "Press ENTER to start");
      mvprintw(HUD_Y + 15, HUD_X, "Press H to highscores");
      mvprintw(HUD_Y + 16, HUD_X, "Press ESC to exit");
      break;
    case PAUSE:
      mvprintw(HUD_Y + 12, HUD_X, "PAUSED");
      mvprintw(HUD_Y + 14, HUD_X, "Press P to resume");
      break;
    case GAMEOVER:
      mvprintw(HUD_Y + 12, HUD_X, "GAME OVER");
      mvprintw(HUD_Y + 14, HUD_X, "Press ENTER to restart");
      mvprintw(HUD_Y + 15, HUD_X, "Press H to highscores");
      mvprintw(HUD_Y + 16, HUD_X, "Press ESC to exit");
      break;
    default:
      break;
  }

  refresh();
}

void fe_prompt_username(char* out, size_t n) {
  if (!out || n == 0) return;

  bool was_nodelay = is_nodelay(stdscr);
  echo();
  nodelay(stdscr, FALSE);

  int box_h = 5;
  int box_w = 30;
  int top = OFF_Y + field_h() / 2 - box_h / 2;
  int left = OFF_X + field_w() / 2 - box_w / 2;
  if (top < 1) top = 1;
  if (left < 1) left = 1;

  for (int x = 0; x < box_w; ++x) {
    mvaddch(top, left + x, ACS_HLINE);
    mvaddch(top + box_h - 1, left + x, ACS_HLINE);
  }
  for (int y = 0; y < box_h; ++y) {
    mvaddch(top + y, left, ACS_VLINE);
    mvaddch(top + y, left + box_w - 1, ACS_VLINE);
  }
  mvaddch(top, left, ACS_ULCORNER);
  mvaddch(top, left + box_w - 1, ACS_URCORNER);
  mvaddch(top + box_h - 1, left, ACS_LLCORNER);
  mvaddch(top + box_h - 1, left + box_w - 1, ACS_LRCORNER);

  mvprintw(top + 1, left + 2, "Enter your name:");
  move(top + 2, left + 2);
  clrtoeol();

  int maxn = (int)(n - 1);
  if (maxn < 1) maxn = 1;
  getnstr(out, maxn);
  out[n - 1] = '\0';
  if (out[0] == '\0') strncpy(out, "Player", n);

  noecho();
  nodelay(stdscr, was_nodelay ? TRUE : FALSE);
  clear();
  refresh();
}

void fe_show_scores(void) {
  scoreboard_t tb;
  (void)sc_load(&tb, NULL);

  timeout(-1);

  clear();
  mvprintw(1, 2, "High Scores (top 20) — press Q/ESC/ENTER to return");

  int start_y = 3, off_x = 2;
  mvprintw(start_y, off_x, "#");
  mvprintw(start_y, off_x + 4, "Name");
  mvprintw(start_y, off_x + 24, "Score");

  int show = tb.count < 20 ? tb.count : 20;
  for (int i = 0; i < show; ++i) {
    mvprintw(start_y + 2 + i, off_x, "%2d", i + 1);
    mvprintw(start_y + 2 + i, off_x + 4, "%-*s", 18, tb.list[i].name);
    mvprintw(start_y + 2 + i, off_x + 24, "%8d", tb.list[i].score);
  }
  refresh();

  int ch;
  while ((ch = getch()), !(ch == '\n' || ch == 27 || ch == 'q' || ch == 'Q')) {
  }

  timeout(120);

  clear();
  refresh();
}