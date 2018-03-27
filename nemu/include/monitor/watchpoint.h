#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  int value;
  struct watchpoint *next;
  char expression[32];
  bool busy;
} WP;
WP* new_wp(char *args);
void free_wp(int num);
void show();
WP* checkchange();
#endif
