/*   Copyright 1996 TeCom Inc. All rights reserved                  */
/*    File Created: 11 NOV 96    By: TMMSL                           */

#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <dos.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <bios.h>
#include <malloc.h>

#include "cebus.h"
#include "2box.h"
#include "vga.h"
#include "scrnobjt.h"
#include "lwindow.h"
#include "pshbtn.h"
#include "inutil.h"
#include "inside.h"
#include "mouse.h"

extern int which_mouse; //0 = arrow, 1 = finger.... from mloop.c
extern int im_busy;
extern CURSOR_DEF_T hourglass_ms;
extern CURSOR_DEF_T arrow_ms;
extern CURSOR_DEF_T finger_ms;
extern CURSOR_DEF_T bar_ms;

extern int item_index;
extern int last_index;

void PutWindow(LWINDOW_T* thisWindow) {
  int i;

  hide_mouse();
  which_mouse = 0;

  item_index = 0;
  last_index = -1;

  thisWindow->so[thisWindow->focus.current]->got_focus = FALSE;
  thisWindow->focus.current = 0;
  if (thisWindow->so[thisWindow->focus.current]->type != PUSH_BUTTON) {
    thisWindow->so[thisWindow->focus.current]->got_focus = TRUE;
  }

  pbutton(thisWindow->win_pos.top,
          thisWindow->win_pos.left,
          thisWindow->win_pos.height - 1,
          thisWindow->win_pos.width - 1,
          PB_UP,
          BG_WHT + FG_WHT);

  for (i = 0; i < thisWindow->item_count; i++) {
    if (thisWindow->so[i]->visible == TRUE) {
      (*thisWindow->so[i]->DisplayObject)(thisWindow->so[i]);
    }
  }
  ShowTitle(thisWindow->title);
  Status("");

  if (thisWindow->Draw != NULL) {
    (*thisWindow->Draw)();
  }
  if (im_busy == TRUE) {
    mouse_cursor(&hourglass_ms);
  } else {
    mouse_cursor(&arrow_ms);
  }
  unhide_mouse();
}

void ShowTitle(char* title) {
  void TitleBar(int top, int left, int height, int width);

  TitleBar(glbWindow->win_pos.top + 4,    //top
           glbWindow->win_pos.left + 4,   //left
           17,                            //height
           glbWindow->win_pos.width - 9); //width

  if (title != NULL) {
    ptext(title,
          glbWindow->win_pos.top + 6,
          glbWindow->win_pos.left + 7,
          BG_BLU + FG_HWHT);
  }
}

void TitleBar(int top, int left, int height, int width) {

  area_clear(top, top + height, left, left + width, BG_BLU + FG_BLU);
  //    hline(top, left,left + width , FG_GRY);
  //vline(left, top, top + height, FG_GRY);
  //
  //hline(top + height, left, left + width, FG_HWHT);
  //vline(left + width, top, top + height, FG_HWHT);
}

void DefaultOnKeyPress(SCREEN_OBJECT_T* so, int c) {
  if (so->OnKeyPress != NULL) {
    (*so->OnKeyPress)(so, c); // call the object's mouse down event handler
  }
}
