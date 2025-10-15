#include <ncurses.h>

#include "include/input.h"
#include "include/tetris_keys.h"
#include "../backend/include/tetris_types.h"

signals get_signal(int ch) {
  signals s = NOSIG;

  switch (ch) {
    case KEY_LEFT_CODE:
      s = MOVE_LEFT;
      break;
    case KEY_RIGHT_CODE:
      s = MOVE_RIGHT;
      break;
    case KEY_DOWN_CODE:
      s = HARD_DROP;
      break;
    case KEY_UP_CODE:
      s = NOSIG;
      break;
    case ' ':
      s = ROTATE;
      break;

    case 'h':
    case 'H':
      s = UI_SHOW_SCORES;
      break;

    case KEY_ESC_CODE:
      s = ESCAPE_BTN;
      break;
#ifdef KEY_ENTER
    case KEY_ENTER:
      s = ENTER_BTN;
      break;
#endif
    case KEY_ENTER_CODE:
      s = ENTER_BTN;
      break;

    case KEY_PAUSE_CODE:
    case 'P':
      s = PAUSE_BTN;
      break;

    default:
      s = NOSIG;
      break;
  }

  return s;
}