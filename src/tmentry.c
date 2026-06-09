// ==========================================================================
// $Workfile:   tmentry.c  $
// $Revision:   1.4  $
//
// HISTORY:
// Author            Date      Description
// --------------------------------------------------------------------------
//
//
//
// DESCRIPTION
// ==========================================================================
//   Copyright 1998 TeCom Inc. All rights reserved
// ==========================================================================

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
#include "tmentry.h"
#include "mouse.h"
#include "mloop.h"
#include "msgbox.h"

#define down 0
#define up 1
static UCHAR btntxt[2][2] = {
    0x9C, 0x00, // down
    0x9D, 0x00  // up
};

extern void Status(char*);

void DisplayTimeEntry(TIME_ENTRY_T* te) {
  int bg_color;
  int fg_color;
  int left;
  int top;
  int i;
  int adder;
  UCHAR boxes;
  char hour[3];
  char minute[3];
  char month[3];
  char day[3];
  char year[5];
  char* am_pm[2] = {"AM", "PM"};

  if (te->so.visible == FALSE) {
    return;
  }

  boxes = 0;
  adder = 0;
  left = te->so.position.left;
  top = te->so.position.top;
  te->so.position.height = 17;

  switch (te->type) {
  case date_ent:
    boxes = 3;
    adder = 0;
    break;
  case time_ent:
    boxes = 3;
    adder = 3;
    break;
  case combo_ent:
    boxes = 6;
    adder = 0;
    break;
  case mo_yr_ent:
    boxes = 2;
    adder = 1;
    break;
  }

  hide_mouse();
  //show all the boxes
  for (i = 0; i < boxes; i++) {
    if (te->so.got_focus == TRUE) {
      if (te->selected == i + adder) {
        bg_color = BG_BLU;
      } else {
        bg_color = BG_GRY;
      }
    } else {
      bg_color = BG_HWHT;
    }

    if ((i == 2) && ((te->type == date_ent) || (te->type == combo_ent))) {
      pbutton(top,
              left,
              17,        //height
              8 * 4 + 6, //width
              PB_DN,
              bg_color);
      left += (8 * 4 + 18);
    } else if ((i == 1) && (te->type == mo_yr_ent)) {
      pbutton(top,
              left,
              17,        //height
              8 * 4 + 6, //width
              PB_DN,
              bg_color);
      left += (8 * 4 + 18);
    } else {
      pbutton(top,
              left,
              17,        //height
              8 * 2 + 6, //width
              PB_DN,
              bg_color);
      left += (8 * 2 + 18);
    }
  }
  //show little spinner buttons
  left -= 11;
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

  if (te->so.got_focus == TRUE) {
    fg_color = FG_HWHT;
  } else {
    fg_color = FG_BLK;
  }

  left = te->so.position.left;
  top = te->so.position.top;
  switch (te->type) {
  case date_ent:
    if (((te->year) % 4) == 0) // it's a leap year!!
    {
      if (te->day > day_tab[1][te->month]) {
        te->day = day_tab[1][te->month];
      }
    } else // it's NOT a leap year!!
    {
      if (te->day > day_tab[0][te->month]) {
        te->day = day_tab[0][te->month];
      }
    }
    sprintf(month, "%02d", te->month);
    sprintf(day, "%02d", te->day);
    sprintf(year, "%04d", 1900 + te->year);
    te->so.position.width = 122;
    ptext(month, top + 2, left + 3, BG_TRANSPARENT + fg_color);
    ptext("/", top + 2, left + (8 * 2) + 9, BG_WHT + FG_BLK);
    left += (8 * 2 + 18);
    ptext(day, top + 2, left + 3, BG_TRANSPARENT + fg_color);
    ptext("/", top + 2, left + (8 * 2) + 9, BG_WHT + FG_BLK);
    left += (8 * 2 + 18);
    ptext(year, top + 2, left + 3, BG_TRANSPARENT + fg_color);
    break;
  case time_ent:
    sprintf(hour, "%02d", te->hour);
    sprintf(minute, "%02d", te->minute);
    te->so.position.width = 106;
    ptext(hour, top + 2, left + 3, BG_TRANSPARENT + fg_color);
    ptext(":", top + 2, left + (8 * 2) + 9, BG_WHT + FG_BLK);
    left += (8 * 2 + 18);
    ptext(minute, top + 2, left + 3, BG_TRANSPARENT + fg_color);
    left += (8 * 2 + 18);
    ptext(am_pm[te->am_pm], top + 2, left + 3, BG_TRANSPARENT + fg_color);
    break;
  case combo_ent:
    if (((te->year) % 4) == 0) // it's a leap year!!
    {
      if (te->day > day_tab[1][te->month]) {
        te->day = day_tab[1][te->month];
      }
    } else // it's NOT a leap year!!
    {
      if (te->day > day_tab[0][te->month]) {
        te->day = day_tab[0][te->month];
      }
    }
    sprintf(month, "%02d", te->month);
    sprintf(day, "%02d", te->day);
    sprintf(year, "%04d", 1900 + te->year);
    sprintf(hour, "%02d", te->hour);
    sprintf(minute, "%02d", te->minute);
    te->so.position.width = 224;
    ptext(month, top + 2, left + 3, BG_TRANSPARENT + fg_color);
    ptext("/", top + 2, left + (8 * 2) + 9, BG_WHT + FG_BLK);
    left += (8 * 2 + 18);
    ptext(day, top + 2, left + 3, BG_TRANSPARENT + fg_color);
    ptext("/", top + 2, left + (8 * 2) + 9, BG_WHT + FG_BLK);
    left += (8 * 4 + 18);
    ptext(year, top + 2, left + 3, BG_TRANSPARENT + fg_color);
    ptext("-", top + 2, left + (8 * 2) + 9, BG_WHT + FG_BLK);
    left += (8 * 2 + 18);
    ptext(hour, top + 2, left + 3, BG_TRANSPARENT + fg_color);
    ptext(":", top + 2, left + (8 * 2) + 9, BG_WHT + FG_BLK);
    left += (8 * 2 + 18);
    ptext(minute, top + 2, left + 3, BG_TRANSPARENT + fg_color);
    left += (8 * 2 + 18);
    ptext(am_pm[te->am_pm], top + 2, left + 3, BG_TRANSPARENT + fg_color);
    break;
  case mo_yr_ent:
    sprintf(month, "%02d", te->month);
    sprintf(year, "%04d", 1900 + te->year);
    te->so.position.width = 122;
    ptext(month, top + 2, left + 3, BG_TRANSPARENT + fg_color);
    ptext("/", top + 2, left + (8 * 2) + 9, BG_WHT + FG_BLK);
    left += (8 * 2 + 18);
    ptext(year, top + 2, left + 3, BG_TRANSPARENT + fg_color);
    break;
  }
  unhide_mouse();
}

void TimeEntryClick(TIME_ENTRY_T* te) {
  COORDINATE_T pos;

  switch (te->type) {
  case date_ent:
    pos.top = te->so.position.top;
    pos.left = te->so.position.left + 107;
    pos.width = 14;
    pos.height = 8;
    if (MouseOver(&pos) == TRUE) // up button hit
    {
      switch (te->selected) {
      case 0: //month
        if ((++(te->month)) > 12) {
          te->month = 1;
        }
        break;
      case 1:                      //day
        if (((te->year) % 4) == 0) // it's a leap year!!
        {
          if ((++(te->day)) > day_tab[1][te->month]) {
            te->day = 1;
          }
        } else // it's NOT a leap year!!
        {
          if ((++(te->day)) > day_tab[0][te->month]) {
            te->day = 1;
          }
        }
        break;
      case 2: //year
        ++(te->year);
        break;
      }
    }
    pos.top = te->so.position.top + 9;
    pos.left = te->so.position.left + 107;
    pos.width = 14;
    pos.height = 8;
    if (MouseOver(&pos) == TRUE) // down button hit
    {
      switch (te->selected) {
      case 0: //month
        if (--(te->month) < 1) {
          te->month = 12;
        }
        break;
      case 1:                    //day
        if ((te->year % 4) == 0) // it's a leap year!!
        {
          if (--(te->day) < 1) {
            te->day = day_tab[1][te->month];
          }
        } else // it's NOT a leap year!!
        {
          if (--(te->day) < 1) {
            te->day = day_tab[0][te->month];
          }
        }
        break;
      case 2: //year
        --(te->year);
        break;
      }
    }
    break;

  case time_ent:
    pos.top = te->so.position.top;
    pos.left = te->so.position.left + 91;
    pos.width = 14;
    pos.height = 8;
    if (MouseOver(&pos) == TRUE) // up button hit
    {
      switch (te->selected) {
      case 3: //hour
        if ((++(te->hour)) > 12) {
          te->hour = 1;
        }
        break;
      case 4: //minute
        if ((++(te->minute)) > 59) {
          te->minute = 0;
        }
        break;
      case 5: // am_pm
        if (++(te->am_pm) > 1) {
          te->am_pm = 0;
        }
        break;
      }
    }
    pos.top = te->so.position.top + 9;
    pos.left = te->so.position.left + 91;
    pos.width = 14;
    pos.height = 8;
    if (MouseOver(&pos) == TRUE) // down button hit
    {
      switch (te->selected) {
      case 3: //hour
        if (--(te->hour) < 1) {
          te->hour = 12;
        }
        break;
      case 4: //minute
        if (--(te->minute) < 0) {
          te->minute = 59;
        }
        break;
      case 5: // am_pm
        if (--(te->am_pm) < 0) {
          te->am_pm = 1;
        }
        break;
      }
    }
    break;
  case combo_ent:
    pos.top = te->so.position.top;
    pos.left = te->so.position.left + 209;
    pos.width = 14;
    pos.height = 8;
    if (MouseOver(&pos) == TRUE) // up button hit
    {
      switch (te->selected) {
      case 0: //month
        if ((++(te->month)) > 12) {
          te->month = 1;
        }
        break;
      case 1:                    //day
        if ((te->year % 4) == 0) // it's a leap year!!
        {
          if ((++(te->day)) > day_tab[1][te->month]) {
            te->day = 1;
          }
        } else // it's NOT a leap year!!
        {
          if ((++(te->day)) > day_tab[0][te->month]) {
            te->day = 1;
          }
        }
        break;
      case 2: //year
        ++(te->year);
        break;
      case 3: //hour
        if ((++(te->hour)) > 12) {
          te->hour = 1;
        }
        break;
      case 4: //minute
        if ((++(te->minute)) > 59) {
          te->minute = 0;
        }
        break;
      case 5: // am_pm
        if ((++(te->am_pm)) > 1) {
          te->am_pm = 0;
        }
        break;
      }
    }
    pos.top = te->so.position.top + 9;
    pos.left = te->so.position.left + 209;
    pos.width = 14;
    pos.height = 8;
    if (MouseOver(&pos) == TRUE) // down button hit
    {
      switch (te->selected) {
      case 0: //month
        if (--(te->month) < 1) {
          te->month = 12;
        }
        break;
      case 1:                      //day
        if (((te->year) % 4) == 0) // it's a leap year!!
        {
          if (--(te->day) < 1) {
            te->day = day_tab[1][te->month];
          }
        } else // it's NOT a leap year!!
        {
          if (--(te->day) < 1) {
            te->day = day_tab[0][te->month];
          }
        }
        break;
      case 2: //year
        --(te->year);
        break;
      case 3: //hour
        if ((--(te->hour)) < 1) {
          te->hour = 12;
        }
        break;
      case 4: //minute
        if ((--(te->minute)) < 0) {
          te->minute = 59;
        }
        break;
      case 5: // am_pm
        if ((--(te->am_pm)) < 0) {
          te->am_pm = 1;
        }
        break;
      }
    }
    break;
  case mo_yr_ent:
    pos.top = te->so.position.top;
    pos.left = te->so.position.left + 73;
    pos.width = 14;
    pos.height = 8;
    if (MouseOver(&pos) == TRUE) // up button hit
    {
      switch (te->selected) {
      case 1: //month
        if ((++(te->month)) > 12) {
          te->month = 1;
        }
        break;
      case 2: //year
        ++(te->year);
        break;
      }
    }
    pos.top = te->so.position.top + 9;
    pos.left = te->so.position.left + 73;
    pos.width = 14;
    pos.height = 8;
    if (MouseOver(&pos) == TRUE) // down button hit
    {
      switch (te->selected) {
      case 1: //month
        if ((--(te->month)) < 1) {
          te->month = 12;
        }
        break;
      case 2: //year
        --(te->year);
        break;
      }
    }
    break;
  }
  if (te->OnChange != NULL) {
    (*te->OnChange)();
  }
  DisplayTimeEntry(te);
}

void TimeEntryMouseDown(TIME_ENTRY_T* te) {
  int boxes;
  int adder;
  int i;
  COORDINATE_T pos;

  boxes = 0;
  adder = 0;
  te->so.got_focus = TRUE;
  hide_mouse();
  switch (te->type) {
  case date_ent:
    boxes = 3;
    adder = 0;
    pos.top = te->so.position.top;
    pos.left = te->so.position.left + 107;
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
      unhide_mouse();
      return;
    }

    pos.top = te->so.position.top + 9;
    if (MouseOver(&pos) == TRUE) // down button hit
    {
      pbutton(pos.top,
              pos.left,
              8,
              14,
              PB_DN,
              BG_WHT + FG_WHT);
      ptext(&btntxt[down][0], pos.top - 1, pos.left + 4, BG_TRANSPARENT + FG_BLU);
      unhide_mouse();
      return;
    }
    break;
  case time_ent:
    adder = 3;
    boxes = 3;
    pos.top = te->so.position.top;
    pos.left = te->so.position.left + 91;
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
      unhide_mouse();
      return;
    }

    pos.top = te->so.position.top + 9;
    if (MouseOver(&pos) == TRUE) // down button hit
    {
      pbutton(pos.top,
              pos.left,
              8,
              14,
              PB_DN,
              BG_WHT + FG_WHT);
      ptext(&btntxt[down][0], pos.top - 1, pos.left + 4, BG_TRANSPARENT + FG_BLU);
      unhide_mouse();
      return;
    }
    break;
  case combo_ent:
    adder = 0;
    boxes = 6;
    pos.top = te->so.position.top;
    pos.left = te->so.position.left + 209;
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
      unhide_mouse();
      return;
    }

    pos.top = te->so.position.top + 9;
    if (MouseOver(&pos) == TRUE) // down button hit
    {
      pbutton(pos.top,
              pos.left,
              8,
              14,
              PB_DN,
              BG_WHT + FG_WHT);
      ptext(&btntxt[down][0], pos.top - 1, pos.left + 4, BG_TRANSPARENT + FG_BLU);
      unhide_mouse();
      return;
    }
    break;

  case mo_yr_ent:
    boxes = 2;
    adder = 1;
    pos.top = te->so.position.top;
    pos.left = te->so.position.left + 73;
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
      unhide_mouse();
      return;
    }

    pos.top = te->so.position.top + 9;
    if (MouseOver(&pos) == TRUE) // down button hit
    {
      pbutton(pos.top,
              pos.left,
              8,
              14,
              PB_DN,
              BG_WHT + FG_WHT);
      ptext(&btntxt[down][0], pos.top - 1, pos.left + 4, BG_TRANSPARENT + FG_BLU);
      unhide_mouse();
      return;
    }
    break;
  }
  unhide_mouse();
  //test all the boxes
  pos.top = te->so.position.top;
  pos.left = te->so.position.left;
  pos.width = 8 * 2 + 6;
  pos.height = 17;
  for (i = 0; i < boxes; i++) {
    if ((i == 2) && ((te->type == date_ent) || (te->type == combo_ent))) {
      pos.width = 8 * 4 + 6;
    } else if ((i == 1) && (te->type == mo_yr_ent)) {
      pos.width = 8 * 4 + 6;
    } else {
      pos.width = 8 * 2 + 6;
    }
    if (MouseOver(&pos) == TRUE) {
      te->selected = i + adder;
      DisplayTimeEntry(te);
      return;
    }
    if ((i == 2) && ((te->type == date_ent) || (te->type == combo_ent))) {
      pos.left += (8 * 4 + 18);
    } else if ((i == 1) && (te->type == mo_yr_ent)) {
      pos.left += (8 * 4 + 18);
    } else {
      pos.left += (8 * 2 + 18);
    }
  }
}

void TimeEntryMouseUp(TIME_ENTRY_T* te) {
  COORDINATE_T pos;

  te->so.got_focus = TRUE;
  switch (te->type) {
  case date_ent:
    pos.top = te->so.position.top;
    pos.left = te->so.position.left + 91 + 8 * 2;
    pos.width = 14;
    pos.height = 8;
    break;
  case time_ent:
    pos.top = te->so.position.top;
    pos.left = te->so.position.left + 91;
    pos.width = 14;
    pos.height = 8;
    break;
  case combo_ent:
    pos.top = te->so.position.top;
    pos.left = te->so.position.left + 193 + 8 * 2;
    pos.width = 14;
    pos.height = 8;
    break;
  case mo_yr_ent:
    pos.top = te->so.position.top;
    pos.left = te->so.position.left + 73;
    pos.width = 14;
    pos.height = 8;
    break;
  }
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

void TimeEntryKeyPress(TIME_ENTRY_T* te, int key) {
  int max_sel;
  int adder;
  switch (te->type) {
  case date_ent:
    max_sel = 2;
    adder = 0;
    break;
  case time_ent:
    max_sel = 2;
    adder = 3;
    break;
  case combo_ent:
    max_sel = 5;
    adder = 0;
    break;
  case mo_yr_ent:
    max_sel = 1;
    adder = 1;
    break;
  }
  if (key == 0) {
    key = kbchar();
    switch (key) {
    case 77: // right arrow
      if (++(te->selected) > max_sel + adder) {
        te->selected = 0 + adder;
      }
      break;
    case 75: // left arrow
      if (--(te->selected) < 0 + adder) {
        te->selected = max_sel + adder;
      }
      break;
    case 72: // up arrow
      switch (te->type) {
      case date_ent:
        switch (te->selected) {
        case 0: //month
          if (++(te->month) > 12) {
            te->month = 1;
          }
          break;
        case 1:                      //day
          if (((te->year) % 4) == 0) // it's a leap year!!
          {
            if (++(te->day) > day_tab[1][te->month]) {
              te->day = 1;
            }
          } else // it's NOT a leap year!!
          {
            if (++(te->day) > day_tab[0][te->month]) {
              te->day = 1;
            }
          }
          break;
        case 2: //year
          ++(te->year);
          break;
        }
        break;
      case time_ent:
        switch (te->selected) {
        case 3: //hour
          if (++(te->hour) > 12) {
            te->hour = 1;
          }
          break;
        case 4: //minute
          if (++(te->minute) > 59) {
            te->minute = 0;
          }
          break;
        case 5: // am_pm
          if (++(te->am_pm) > 1) {
            te->am_pm = 0;
          }
          break;
        }
        break;
      case combo_ent:
        switch (te->selected) {
        case 0: //month
          if (++(te->month) > 12) {
            te->month = 1;
          }
          break;
        case 1:                      //day
          if (((te->year) % 4) == 0) // it's a leap year!!
          {
            if (++(te->day) > day_tab[1][te->month]) {
              te->day = 1;
            }
          } else // it's NOT a leap year!!
          {
            if (++(te->day) > day_tab[0][te->month]) {
              te->day = 1;
            }
          }
          break;
        case 2: //year
          ++(te->year);
          break;
        case 3: //hour
          if (++(te->hour) > 12) {
            te->hour = 1;
          }
          break;
        case 4: //minute
          if (++(te->minute) > 59) {
            te->minute = 0;
          }
          break;
        case 5: // am_pm
          if (++(te->am_pm) > 1) {
            te->am_pm = 0;
          }
          break;
        }
        break;

      case mo_yr_ent:
        switch (te->selected) {
        case 1: //month
          if (++(te->month) > 12) {
            te->month = 1;
          }
          break;
        case 2: //year
          ++(te->year);
          break;
        }
        break;
      }
      break;
    case 80: // down arrow
      switch (te->type) {
      case date_ent:
        switch (te->selected) {
        case 0: //month
          if ((--(te->month)) < 1) {
            te->month = 12;
          }
          break;
        case 1:                      //day
          if (((te->year) % 4) == 0) // it's a leap year!!
          {
            if ((--(te->day)) < 1) {
              te->day = day_tab[1][te->month];
            }
          } else // it's NOT a leap year!!
          {
            if ((--(te->day)) < 1) {
              te->day = day_tab[0][te->month];
            }
          }
          break;
        case 2: //year
          --(te->year);
          break;
        }
        break;
      case time_ent:
        switch (te->selected) {
        case 3: //hour
          if ((--(te->hour)) < 1) {
            te->hour = 12;
          }
          break;
        case 4: //minute
          if ((--(te->minute)) < 0) {
            te->minute = 59;
          }
          break;
        case 5: // am_pm
          if ((--(te->am_pm)) < 0) {
            te->am_pm = 1;
          }
          break;
        }
        break;
      case combo_ent:
        switch (te->selected) {
        case 0: //month
          if ((--(te->month)) < 1) {
            te->month = 12;
          }
          break;
        case 1:                      //day
          if (((te->year) % 4) == 0) // it's a leap year!!
          {
            if ((--(te->day)) < 1) {
              te->day = day_tab[1][te->month];
            }
          } else // it's NOT a leap year!!
          {
            if ((--(te->day)) < 1) {
              te->day = day_tab[0][te->month];
            }
          }
          break;
        case 2: //year
          --(te->year);
          break;
        case 3: //hour
          if ((--(te->hour)) < 1) {
            te->hour = 12;
          }
          break;
        case 4: //minute
          if ((--(te->minute)) < 0) {
            te->minute = 59;
          }
          break;
        case 5: // am_pm
          if ((--(te->am_pm)) < 0) {
            te->am_pm = 1;
          }
          break;
        }
        break;

      case mo_yr_ent:
        switch (te->selected) {
        case 1: //month
          if ((--(te->month)) < 1) {
            te->month = 12;
          }
          break;
        case 2: //year
          --(te->year);
          break;
        }
        break;
      }
      break;
    }
    if (te->OnChange != NULL) {
      (*te->OnChange)();
    }
    DisplayTimeEntry(te);
  }
}
