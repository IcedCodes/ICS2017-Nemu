#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  int value;
  struct watchpoint *next;
  char *expression;
  bool busy;
} WP;
WP* new_wp();
void free_wp(WP *wp);
void show();
WP* checkchange();
#endif
