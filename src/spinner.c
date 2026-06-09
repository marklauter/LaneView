#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <dos.h>
#include <time.h>
#include <conio.h>
#include <string.h>

#include "2box.h"
#include "vga.h"
#include "scrnobjt.h"
#include "frame_3d.h"
#include "textbox.h"
#include "pshbtn.h"
#include "calendar.h"
#include "spinner.h"
#include "mouse.h"
#include "mloop.h"

#define down 0
#define up 1
static UCHAR btntxt[2][2] = {
    0x9C, 0x00, // down
    0x9D, 0x00  // up
};

void DisplaySpinner(SPINNER_T* sp) {
  int bg_color;
  int fg_color;
  int left;
  int top;
  char value[3];

  if (sp->so.visible == FALSE) {
    return;
  }

  left = sp->so.position.left;
  top = sp->so.position.top;
  sp->so.position.height = 17;
  sp->so.position.width = 46;

  hide_mouse();

  //show the box
  if (sp->so.got_focus == TRUE) {
    bg_color = BG_BLU;
  } else {
    bg_color = BG_HWHT;
  }
  pbutton(top,
          left,
          17,        //height
          8 * 3 + 6, //width
          PB_DN,
          bg_color);
  left += (8 * 3 + 18);

  //show little spinner buttons
  left -= 10;
  pbutton(top,
          left,
          8,
          14,
          PB_UP,
          BG_WHT + FG_WHT);
  ptext(&btntxt[up][0], top - 3, left + 3, BG_TRANSPARENT + FG_BLU);
  pbutton(top + 9,
          left,
          8,
          14,
          PB_UP,
          BG_WHT + FG_WHT);
  ptext(&btntxt[down][0], top + 7, left + 3, BG_TRANSPARENT + FG_BLU);

  if (sp->so.got_focus == TRUE) {
    fg_color = FG_HWHT;
  } else {
    fg_color = FG_BLK;
  }

  left = sp->so.position.left + 4;
  top = sp->so.position.top + 2;
  sprintf(value, "% 3d", sp->value);
  ptext(value, top, left, BG_TRANSPARENT + fg_color);

  unhide_mouse();
}

void SpinnerClick(SPINNER_T* sp) {
  COORDINATE_T pos;

  pos.top = sp->so.position.top;
  pos.left = sp->so.position.left + (8 * 3) + 8;
  pos.width = 14;
  pos.height = 8;

  if (MouseOver(&pos) == TRUE) // up button hit
  {
    if ((sp->value += sp->increment) > sp->max) {
      sp->value = sp->min;
    }
    if (sp->OnChange != NULL) {
      (*sp->OnChange)();
    }
  }
  pos.top = sp->so.position.top + 9;
  if (MouseOver(&pos) == TRUE) // up button hit
  {
    if ((sp->value -= sp->increment) < sp->min) {
      sp->value = sp->max;
    }
    if (sp->OnChange != NULL) {
      (*sp->OnChange)();
    }
  }
  DisplaySpinner(sp);
}

void SpinnerMouseDown(SPINNER_T* sp) {
  COORDINATE_T pos;

  sp->so.got_focus = TRUE;

  pos.top = sp->so.position.top;
  pos.left = sp->so.position.left + (8 * 3) + 8;
  pos.width = 14;
  pos.height = 8;

  if (MouseOver(&pos) == TRUE) // up button hit
  {
    pbutton(pos.top,
            pos.left,
            8,
            14,
            PB_DN,
            BG_WHT + FG_WHT);
    ptext(&btntxt[up][0], pos.top - 2, pos.left + 4, BG_TRANSPARENT + FG_BLU);
    return;
  }

  pos.top = sp->so.position.top + 9;
  if (MouseOver(&pos) == TRUE) // down button hit
  {
    pbutton(pos.top,
            pos.left,
            8,
            14,
            PB_DN,
            BG_WHT + FG_WHT);
    ptext(&btntxt[down][0], pos.top - 1, pos.left + 4, BG_TRANSPARENT + FG_BLU);
    return;
  }

  DisplaySpinner(sp);
}

void SpinnerMouseUp(SPINNER_T* sp) {
  COORDINATE_T pos;

  sp->so.got_focus = TRUE;
  pos.top = sp->so.position.top;
  pos.left = sp->so.position.left + (8 * 3) + 8;
  pos.width = 14;
  pos.height = 8;

  pbutton(pos.top,
          pos.left,
          8,
          14,
          PB_UP,
          BG_WHT + FG_WHT);
  ptext(&btntxt[up][0], pos.top - 3, pos.left + 3, BG_TRANSPARENT + FG_BLU);
  pbutton(pos.top + 9,
          pos.left,
          8,
          14,
          PB_UP,
          BG_WHT + FG_WHT);
  ptext(&btntxt[down][0], pos.top + 7, pos.left + 3, BG_TRANSPARENT + FG_BLU);
}

void SpinnerKeyPress(SPINNER_T* sp, int key) {
  if (key == 0) {
    key = kbchar();
    switch (key) {
    case 72: // up arrow
      sp->value += sp->increment;
      if (sp->value > sp->max) {
        sp->value = sp->min;
      }
      if (sp->OnChange != NULL) {
        (*sp->OnChange)();
      }
      break;
    case 80: // down arrow
      sp->value -= sp->increment;
      if (sp->value < sp->min) {
        sp->value = sp->max;
      }
      if (sp->OnChange != NULL) {
        (*sp->OnChange)();
      }
      break;
    }
    DisplaySpinner(sp);
  }
}
