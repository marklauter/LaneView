#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <dos.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "2box.h"
#include "vga.h"
#include "scrnobjt.h"
#include "mouse.h"
#include "progind.h"
#include "frame_3d.h"

void PutIndicator(PROGRESS_INDICATOR_T* pi) {
  char buf[6];
  int acw = 0; //area_clear width... fill width for indicator
  int center = 0;

  if ((pi->progress > 100) || (pi->progress < 0)) {
    return;
  }
  pi->position.height = 17;
  acw = (int)(pi->position.width * (pi->progress / 100.0));

  if ((acw < 0) || (acw > 639)) {
    return;
  }

  sprintf(buf, "%%%3d", pi->progress);
  center = pi->position.left + (int)((pi->position.width - (strlen(buf) * 8)) / 2);

  hide_mouse();
  pbutton(pi->position.top, pi->position.left, pi->position.height, pi->position.width, 1, FG_GRY);
  ptext(buf, pi->position.top + 2, center + 1, BG_TRANSPARENT + FG_YEL);
  unhide_mouse();
}

void UpdateIndicator(PROGRESS_INDICATOR_T* pi) {
  char buf[6];
  int i = 0;
  int acw = 0; //area_clear width... fill width for indicator
  int center = 0;

  if ((pi->progress > 100) || (pi->progress < 0)) {
    return;
  }
  acw = (int)(pi->position.width * (pi->progress / 100.0));
  if ((acw < 0) || (acw > 639)) {
    return;
  }

  sprintf(buf, "%%%3d", pi->progress);
  center = pi->position.left + (int)((pi->position.width - (strlen(buf) * 8)) / 2);

  hide_mouse();

  area_clear(pi->position.top + 2,
             pi->position.top + pi->position.height - 2,
             pi->position.left + acw,
             pi->position.left + pi->position.width - 2,
             FG_GRY);
  area_clear(pi->position.top + 2,
             pi->position.top + pi->position.height - 2,
             pi->position.left + 2,
             pi->position.left + acw - 1,
             pi->color);

  ptext(buf, pi->position.top + 2, center + 1, BG_TRANSPARENT + FG_YEL);

  unhide_mouse();
}
