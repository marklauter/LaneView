#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <time.h>

#include "2box.h"
#include "vga.h"
#include "frame_3d.h"

void frame_3d(int top, int left, int height, int width, char type) {
  hide_mouse();
  switch (type) {
  case FT_FLAT:
    hline(top, left, left + width, FG_BLK);
    hline(top + height, left, left + width, FG_BLK);
    vline(left, top, top + height, FG_BLK);
    vline(left + width, top, top + height, FG_BLK);
    break;
  case FT_RAISED:
    hline(top, left, left + width, FG_HWHT);
    hline(top + height, left, left + width, FG_GRY);
    vline(left, top, top + height, FG_HWHT);
    vline(left + width, top, top + height, FG_GRY);
    break;
  case FT_PRESSED:
    hline(top, left, left + width, FG_GRY);
    hline(top + height, left, left + width, FG_HWHT);
    vline(left, top, top + height, FG_GRY);
    vline(left + width, top + 1, top + height, FG_HWHT);
    break;
  case FT_RAISED_LINE:
    hline(top, left, left + width, FG_HWHT);
    hline(top + 1, left + 1, left + width - 1, FG_GRY);
    hline(top + height, left, left + width, FG_GRY);
    hline(top + height - 1, left + 1, left + width - 1, FG_HWHT);
    vline(left, top, top + height - 1, FG_HWHT);
    vline(left + 1, top + 1, top + height - 2, FG_GRY);
    vline(left + width, top, top + height, FG_GRY);
    vline(left + width - 1, top + 1, top + height - 1, FG_HWHT);
    break;
  case FT_RECESSED_LINE:
    hline(top, left, left + width, FG_GRY);
    hline(top + 1, left + 1, left + width - 1, FG_HWHT);
    hline(top + height, left, left + width, FG_HWHT);
    hline(top + height - 1, left + 1, left + width - 1, FG_GRY);
    vline(left, top, top + height - 1, FG_GRY);
    vline(left + 1, top + 1, top + height - 2, FG_HWHT);
    vline(left + width, top, top + height, FG_HWHT);
    vline(left + width - 1, top + 1, top + height - 1, FG_GRY);
    break;
  }
  unhide_mouse();
}

void text_3d(UCHAR* text, int top, int left) {
  ptext(text, top + 1, left + 1, BG_TRANSPARENT + FG_HWHT);
  ptext(text, top, left, BG_TRANSPARENT + FG_GRY);
}

void GroupBox(UCHAR* text, int top, int left, int height, int width) {
  hide_mouse();
  frame_3d(top + 5, left, height - 5, width, FT_RECESSED_LINE);
  ptext(text, top, left + 8, BG_WHT + FG_BLK);
  unhide_mouse();
}
