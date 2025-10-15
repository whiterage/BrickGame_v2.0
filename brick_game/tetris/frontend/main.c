#include <ncurses.h>

#include "../backend/include/api.h"
#include "include/input.h"
#include "../backend/include/scoreboard.h"
#include "include/tetris_frontend.h"
#include "include/tetris_keys.h"
#include "../backend/include/tetris_types.h"

void game_loop(void) {
  char username[TETRIS_NAME_MAX + 1] = {0};
  bool saved_this_round = false;
  tetris_t g = {0};

  win_init(500);
  fe_prompt_username(username, sizeof username);
  fe_set_cell_size(4, 2);
  tetris_init(&g);

  print_board(&g.board, &g.cur, &g.next, &g.stats, g.state);
  refresh();
  while (g.state != EXIT_STATE) {
    int ch = getch();
    signals sig = (ch == ERR) ? NOSIG : get_signal(ch);

    if (sig == UI_SHOW_SCORES && g.state != FALL) {
      flushinp();
      fe_show_scores();
      flushinp();
      print_board(&g.board, &g.cur, &g.next, &g.stats, g.state);
      refresh();
      continue;
    }

    tetris_input(&g, sig);

    if (g.state == FALL) {
      int timeout_ms = 500 - (g.stats.level - 1) * 50;
      if (timeout_ms < 50) timeout_ms = 50;
      timeout(timeout_ms);
    } else {
      timeout(120);
    }

    if (g.state == GAMEOVER && !saved_this_round) {
      scoreboard_t tb;
      if (sc_load(&tb, NULL) == 0) {
        sc_submit(&tb, username, g.stats.score);
        (void)sc_save(&tb, NULL);
      }
      saved_this_round = true;
    }
    if (saved_this_round && g.state != GAMEOVER) {
      saved_this_round = false;
    }

    print_board(&g.board, &g.cur, &g.next, &g.stats, g.state);
    refresh();
  }
}

int main(void) {
  game_loop();
  endwin();
  return 0;
}