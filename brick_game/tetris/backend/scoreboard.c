#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/scoreboard.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

static void sc_trim_newline(char *s) {
  if (!s) return;
  size_t n = strlen(s);
  while (n && (s[n - 1] == '\n' || s[n - 1] == '\r')) {
    s[--n] = '\0';
  }
}

int sc_default_path(char *buf, size_t buflen) {
  int rc = 0;
  if (!buf || buflen == 0) {
    errno = EINVAL;
    rc = -1;
  } else {
    const char *home = getenv("HOME");
    if (!home || !*home) home = ".";
    const char *fname = "/.tetris_scores.tsv";
    size_t need = strlen(home) + strlen(fname) + 1;
    if (need > buflen) {
      errno = ENAMETOOLONG;
      rc = -1;
    } else {
      int n = snprintf(buf, buflen, "%s%s", home, fname);
      if (n < 0 || (size_t)n >= buflen) {
        errno = ENAMETOOLONG;
        rc = -1;
      }
    }
  }
  return rc;
}

static int sc_resolve_path(const char *in, char *out, size_t out_len) {
  int rc = 0;
  if (in && *in) {
    size_t need = strlen(in) + 1;
    if (need > out_len) {
      errno = ENAMETOOLONG;
      rc = -1;
    } else {
      memcpy(out, in, need);
    }
  } else {
    rc = sc_default_path(out, out_len);
  }
  return rc;
}

int sc_load(scoreboard_t *tb, const char *path) {
  int rc = 0;
  char real_path[PATH_MAX];
  FILE *f = NULL;

  if (!tb) {
    errno = EINVAL;
    rc = -1;
  } else if (sc_resolve_path(path, real_path, sizeof real_path) != 0) {
    rc = -1;
  } else {
    tb->count = 0;
    f = fopen(real_path, "r");
    if (!f) {
      if (errno == ENOENT) {
        rc = 0;
      } else {
        rc = -1;
      }
    } else {
      char line[256];
      while (fgets(line, sizeof line, f) && tb->count < TETRIS_MAX_SCORES) {
        sc_trim_newline(line);
        char *tab = strchr(line, '\t');
        if (!tab) continue;
        *tab = '\0';
        const char *name = line;
        const char *score_str = tab + 1;
        long v = strtol(score_str, NULL, 10);
        if (v < 0 || v > INT_MAX) continue;

        score_entry_t *e = &tb->list[tb->count++];
        strncpy(e->name, name, TETRIS_NAME_MAX);
        e->name[TETRIS_NAME_MAX] = '\0';
        e->score = (int)v;
      }
    }
  }

  if (f) {
    if (fclose(f) != 0 && rc == 0) rc = -1;
  }
  return rc;
}

static void sc_insert_sorted(scoreboard_t *tb, const score_entry_t *ent) {
  int i = tb->count;
  if (i < TETRIS_MAX_SCORES) {
    tb->list[i] = *ent;
    tb->count++;
  } else {
    // если список полон и новый результат хуже последнего — выходим
    if (ent->score <= tb->list[tb->count - 1].score) return;
    tb->list[tb->count - 1] = *ent;
  }
  // вставка по убыванию
  for (int j = tb->count - 1; j > 0; --j) {
    if (tb->list[j].score > tb->list[j - 1].score) {
      score_entry_t tmp = tb->list[j - 1];
      tb->list[j - 1] = tb->list[j];
      tb->list[j] = tmp;
    } else {
      break;
    }
  }
}

int sc_submit(scoreboard_t *tb, const char *name, int score) {
  int rc = 0;
  if (!tb || !name) {
    errno = EINVAL;
    rc = -1;
  } else {
    score_entry_t e;
    strncpy(e.name, name, TETRIS_NAME_MAX);
    e.name[TETRIS_NAME_MAX] = '\0';
    if (e.name[0] == '\0') {
      strncpy(e.name, "Player", TETRIS_NAME_MAX);
      e.name[TETRIS_NAME_MAX] = '\0';
    }
    if (score < 0) score = 0;
    e.score = score;
    sc_insert_sorted(tb, &e);
  }
  return rc;
}

int sc_save(const scoreboard_t *tb, const char *path) {
  int rc = 0;
  char real_path[PATH_MAX];
  FILE *f = NULL;

  if (!tb) {
    errno = EINVAL;
    rc = -1;
  } else if (sc_resolve_path(path, real_path, sizeof real_path) != 0) {
    rc = -1;
  } else {
    f = fopen(real_path, "w");
    if (!f) {
      rc = -1;
    } else {
      for (int i = 0; i < tb->count; ++i) {
        if (fprintf(f, "%s\t%d\n", tb->list[i].name, tb->list[i].score) < 0) {
          rc = -1;
          break;
        }
      }
      if (rc == 0 && fflush(f) != 0) rc = -1;
    }
  }

  if (f) {
    if (fclose(f) != 0 && rc == 0) rc = -1;
  }
  return rc;
}