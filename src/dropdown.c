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
#include "dropdown.h"
#include "mouse.h"
#include "mloop.h"

static UCHAR far* dd_saved_window = NULL;

static UCHAR btntxt[2] = {0x9C, 0x00}; //triangle pointing down

void DisplayDropDown(DROPDOWN_BOX_T* ddb) {
  if (ddb->so.visible == FALSE) {
    return;
  }
  hide_mouse();
  if (dd_saved_window != NULL) {
    put_window(dd_saved_window);
    dd_saved_window = NULL;
  }
  if (ddb->so.got_focus == TRUE) {
    pbutton(ddb->so.position.top,
            ddb->so.position.left,
            18,
            ddb->so.position.width,
            PB_DN,
            BG_BLU + FG_BLU);
    if (ddb->count > 0) {
      ptext(ddb->strings[ddb->selected],
            ddb->so.position.top + 2,
            ddb->so.position.left + 4,
            BG_TRANSPARENT + FG_HWHT);
    }
  } else {
    pbutton(ddb->so.position.top,
            ddb->so.position.left,
            18,
            ddb->so.position.width,
            PB_DN,
            BG_HWHT + FG_HWHT);
    if (ddb->count > 0) {
      ptext(ddb->strings[ddb->selected],
            ddb->so.position.top + 2,
            ddb->so.position.left + 4,
            BG_TRANSPARENT + FG_BLK);
    }
  }
  pbutton(ddb->so.position.top + 2,
          ddb->so.position.left + ddb->so.position.width - 13,
          14,
          12,
          PB_UP,
          BG_WHT + FG_WHT);
  ptext(btntxt,
        ddb->so.position.top + 2,
        ddb->so.position.left + ddb->so.position.width - 11,
        BG_TRANSPARENT + FG_BLU);

  if (ddb->so.got_focus == FALSE) {
    ddb->dropped = FALSE;
  }
  if (ddb->dropped == FALSE) {
    ddb->so.position.height = 17;
  }
  if ((ddb->dropped == TRUE) && (ddb->so.got_focus == TRUE)) {
    ShowDDList(ddb);
    ddb->so.position.height = (ddb->count * 14) + 17;
  }
  unhide_mouse();
}

void DropDownClick(DROPDOWN_BOX_T* ddb) {
  int i;
  COORDINATE_T pos;
  int MAX_DROP;

  MAX_DROP = (440 - ddb->so.position.top) / 14;

  pos.left = ddb->so.position.left;
  pos.width = ddb->so.position.width;
  pos.height = 14;

  if (ddb->dropped == TRUE) {
    for (i = 0; i < MAX_DROP; i++) {
      pos.top = (i * 14) + ddb->so.position.top + 18;
      if (MouseOver(&pos) == TRUE) {
        ddb->selected = i;
        break;
      }
    }
    ddb->dropped = FALSE;
    DisplayDropDown(ddb);
    if (ddb->OnChange != NULL) {
      (*ddb->OnChange)();
    }
  } else {
    ddb->dropped = TRUE;
    DisplayDropDown(ddb);
  }
}

void DropDownMouseDown(DROPDOWN_BOX_T* ddb) {
  pbutton(ddb->so.position.top + 2,
          ddb->so.position.left + ddb->so.position.width - 13,
          14,
          12,
          PB_DN,
          BG_WHT + FG_WHT);
  ptext(btntxt,
        ddb->so.position.top + 3,
        ddb->so.position.left + ddb->so.position.width - 10,
        BG_TRANSPARENT + FG_BLU);
  ddb->so.got_focus = TRUE;
}

void DropDownMouseUp(DROPDOWN_BOX_T* ddb) {
  ddb->dropped = FALSE;
  DisplayDropDown(ddb);
}

void ShowDDList(DROPDOWN_BOX_T* ddb) {
  int i;
  int display_count;
  int MAX_DROP;

  MAX_DROP = (440 - ddb->so.position.top) / 14;

  if (ddb->count > MAX_DROP) {
    display_count = MAX_DROP;
  } else {
    display_count = ddb->count;
  }

  dd_saved_window = get_window(((ddb->so.position.top + 18) / 14),
                               (ddb->so.position.left / 8) - 1,
                               (ddb->so.position.width / 8) + 4,
                               (display_count + 3));
  area_clear(ddb->so.position.top + 18,
             ddb->so.position.top + 18 + (display_count * 14),
             ddb->so.position.left,
             ddb->so.position.left + ddb->so.position.width,
             BG_HWHT + FG_HWHT);
  for (i = 0; i < display_count; i++) {
    ptext(ddb->strings[i],
          (i * 14) + ddb->so.position.top + 18,
          ddb->so.position.left + 3,
          BG_HWHT + FG_BLK);
  }
  frame_3d(ddb->so.position.top + 18,
           ddb->so.position.left,
           display_count * 14,
           ddb->so.position.width,
           FT_FLAT);
}

void DropDownKeyPress(DROPDOWN_BOX_T* ddb, int c) {
  ddb->dropped = FALSE;
  switch (c) {
  case 0:
    c = kbchar();
    switch (c) {
    case 72: // arrow up
      if (ddb->selected != 0) {
        --ddb->selected;
      }
      if (ddb->OnChange != NULL) {
        (*ddb->OnChange)();
      }
      break;
    case 80: //down arrow
      if (ddb->selected != (ddb->count - 1)) {
        ++ddb->selected;
      }
      if (ddb->OnChange != NULL) {
        (*ddb->OnChange)();
      }
      break;
    }
    break;
  case CR:
    TabRight();
    break;
  }
  DisplayDropDown(ddb);
}
