#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  int value;
  struct watchpoint *next;
  char expression[32];
  bool busy;
  bool breakpoint;
  int stop;
} WP;
WP* new_wp(char *args);
WP* new_bp(char *args, int value);
void free_wp(int num);
void show();
WP* checkchange();
#endif
