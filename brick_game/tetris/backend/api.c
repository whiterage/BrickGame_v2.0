#include <string.h>

#include "include/api.h"

static void fsm_sigact(signals sig, tetris_t* g) {
  switch (g->state) {
    case START:
      if (sig == ENTER_BTN) {
        g->state = SPAWN;
      } else if (sig == ESCAPE_BTN) {
        g->state = EXIT_STATE;
      } else {
        g->state = START;
      }
      break;
    case SPAWN: {
      int rc = bg_spawn(&g->board, &g->cur, &g->next);
      if (rc == 1) {
        g->state = GAMEOVER;
      } else {
        g->state = FALL;
      }
    } break;
    case FALL:
      if (sig == HARD_DROP) {
        bg_hard_drop(&g->board, &g->cur, &g->stats);
        g->state = SPAWN;
        break;
      }
      if (sig == MOVE_LEFT || sig == MOVE_RIGHT || sig == MOVE_DOWN ||
          sig == ROTATE) {
        bg_apply_input(&g->board, &g->cur, sig);
        g->state = FALL;
      } else if (sig == PAUSE_BTN) {
        g->state = PAUSE;
      } else if (sig == ESCAPE_BTN) {
        g->state = EXIT_STATE;
      } else {
        bool falling = bg_tick(&g->board, &g->cur, &g->stats);
        if (falling) {
          g->state = FALL;
        } else {
          g->state = SPAWN;
        }
      }
      break;
    case PAUSE:
      if (sig == PAUSE_BTN) {
        g->state = FALL;
      } else if (sig == ESCAPE_BTN) {
        g->state = EXIT_STATE;
      } else {
        g->state = PAUSE;
      }
      break;
    case GAMEOVER:
      if (sig == ENTER_BTN) {
        bg_init(&g->board, &g->stats, &g->cur, &g->next);
        g->state = SPAWN;
      } else if (sig == ESCAPE_BTN) {
        g->state = EXIT_STATE;
      } else {
        g->state = GAMEOVER;
      }
      break;
    case EXIT_STATE:
    default:
      break;
  }
}

void tetris_init(tetris_t* g) {
  bg_init(&g->board, &g->stats, &g->cur, &g->next);
  g->state = START;
}

void tetris_input(tetris_t* g, signals sig) {
  fsm_sigact(sig, g);

  while (g->state == SPAWN) {
    fsm_sigact(NOSIG, g);
  }
}

void tetris_export(const tetris_t* g, uint8_t out[TETRIS_ROWS][TETRIS_COLS]) {
  memcpy(out, g->board.grid, sizeof g->board.grid);
}
