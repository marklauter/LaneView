#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <dos.h>
#include <time.h>
#include <string.h>
//#include <math.h>

#include "2box.h"
#include "vga.h"
#include "mouse.h"
#include "scrnobjt.h"
//#include "lwindow.h"
#include "mloop.h"
#include "frame_3d.h"
#include "checkbox.h"

void DisplayCheckBox(CHECK_BOX_T* cb) {
  UCHAR buf[54];

  if (cb->so.visible == FALSE) {
    return;
  }

  hide_mouse();
  cb->so.position.width = (strlen(cb->text) + 3) * 8;
  cb->so.position.height = 14;
  area_clear(cb->so.position.top - 3,
             cb->so.position.top - 3 + 20,
             cb->so.position.left - 3,
             (cb->so.position.left - 3) + (cb->so.position.width + 6),
             FG_WHT);
  if (cb->so.got_focus == TRUE) {
    frame_3d(cb->so.position.top - 3,
             cb->so.position.left - 3,
             19,
             cb->so.position.width + 6,
             FT_RAISED_LINE);
  }
  pbutton(cb->so.position.top + 1,
          cb->so.position.left + 1,
          10,
          10,
          1,
          FG_HWHT);
  if (cb->checked == TRUE) {
    sprintf(buf, "%c ", 0xFE);
  } else {
    sprintf(buf, "  ");
  }
  strcat(buf, cb->text);
  ptext(buf, cb->so.position.top, cb->so.position.left + 3, BG_TRANSPARENT + FG_BLU);
  unhide_mouse();
}

void CheckBoxOnKeyPress(CHECK_BOX_T* cb, int c) {
  switch (c) {
  case 32: //space
  case CR: //eter key
    CheckBoxOnClick(cb);
    break;
  case 0:
    c = kbchar();
    switch (c) {
    case 72: //up arrow
    case 75: //left arrow
      TabLeft();
      break;
    case 80: //down arrow
    case 77: //right arrow
      TabRight();
      break;
    }
    break;
  }
}

void CheckBoxOnClick(CHECK_BOX_T* cb) {
  if (cb->checked == TRUE) {
    cb->checked = FALSE;
  } else {
    cb->checked = TRUE;
  }
  if (cb->cbf != NULL) {
    (*cb->cbf)();
  }
  DisplayCheckBox(cb);
}

void CheckBoxOnMouseDown(CHECK_BOX_T* cb) {
  cb->so.got_focus = TRUE;
  DisplayCheckBox(cb);
}

void CheckBoxOnMouseUp(CHECK_BOX_T* cb) {
  return;
}
