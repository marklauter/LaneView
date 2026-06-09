#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <dos.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "2box.h"
#include "vga.h"
#include "mouse.h"
#include "scrnobjt.h"
#include "textbox.h"
#include "pshbtn.h"
#include "lwindow.h"
#include "mloop.h"

extern int tb_cursor;
UCHAR over_write = 1;

static char KeyPressBuf[2][MAX_TEXT];
void TxtKeyPress(TEXT_BOX_T* tb, int c) {
  if (tb->so.visible == FALSE) {
    return;
  }
  memset(KeyPressBuf[0], 0, MAX_TEXT);
  memset(KeyPressBuf[1], 0, MAX_TEXT);

  //if( (c == 83) && (i < max) )    /* DEL */
  switch (c) {
  case 0:
    c = kbchar();
    switch (c) {
    case 79: //end key
      glbWindow->cursor_pos = tb->col + strlen(tb->text) - 1;
      PutTxtBox(tb);
      return;
    case 71: //home key
      glbWindow->cursor_pos = tb->col - 1;
      PutTxtBox(tb);
      return;
    case 75: //left arrow
      if ((glbWindow->cursor_pos - tb->col) > -1) {
        glbWindow->cursor_pos -= 1;
        PutTxtBox(tb);
      }
      return;
    case 77: //right arrow
      if ((glbWindow->cursor_pos - tb->col) < ((int)strlen(tb->text) - 1)) {
        glbWindow->cursor_pos += 1;
        PutTxtBox(tb);
      }
      return;
    case 82: //over_write
      if (over_write == 0) {
        over_write = 1;
        ptext("OVR", 460, 605, BG_WHT + FG_BLU);
      } else {
        over_write = 0;
        ptext("INS", 460, 605, BG_WHT + FG_BLU);
      }
      return;
    case 83: //delete
      if (((glbWindow->cursor_pos - tb->col) < (int)strlen(tb->text)) && (strlen(tb->text))) {
        if ((glbWindow->cursor_pos + 1) < (tb->col + (int)strlen(tb->text))) {
          strncpy(KeyPressBuf[0], tb->text, glbWindow->cursor_pos - tb->col + 1);
          strcpy(KeyPressBuf[1], &tb->text[glbWindow->cursor_pos - tb->col + 2]);
          strcat(KeyPressBuf[0], KeyPressBuf[1]);
          strcpy(tb->text, KeyPressBuf[0]);
        }
        if (tb->OnChange != NULL) {
          (*tb->OnChange)();
        }
        PutTxtBox(tb);
      }
      return;
    case 80: //down arrow
      TabRight();
      return;
    case 72: //up arrow
      TabLeft();
      return;
    default:
      return;
    }
  case CR: //carriage return
    TabRight();
    return;
  case 8: //back space
    if (((glbWindow->cursor_pos - tb->col) < (int)strlen(tb->text)) &&
        ((strlen(tb->text) > 0)) &&
        (glbWindow->cursor_pos > tb->col - 1)) {
      if ((glbWindow->cursor_pos + 1) < (tb->col + (int)strlen(tb->text))) {
        strncpy(KeyPressBuf[0], tb->text, glbWindow->cursor_pos - tb->col);
        strcpy(KeyPressBuf[1], &tb->text[glbWindow->cursor_pos - tb->col + 1]);
        strcat(KeyPressBuf[0], KeyPressBuf[1]);
        strcpy(tb->text, KeyPressBuf[0]);
      } else {
        tb->text[strlen(tb->text) - 1] = 0;
      }
      glbWindow->cursor_pos -= 1;
      if (tb->OnChange != NULL) {
        (*tb->OnChange)();
      }
      PutTxtBox(tb);
    }
    return;
  }
  switch (tb->type) {
  case tb_int:
    if (((c < 0x30) || (c > 0x39)) && ((c != '-') && (c != '+'))) {
      return;
    }
    break;
  case tb_float:
    if (((c < 0x30) || (c > 0x39)) && (c != 0x2E)) {
      return;
    }
    break;
  case tb_hex:
    if ((c > 0x60) && (c < 0x67)) {
      c -= (0x61 - 0x41);
    }
    if (((c < 0x30) || (c > 0x39)) && ((c < 0x41) || (c > 0x46))) {
      return;
    }
    break;
  case tb_date:
    if ((c < 0x30) || (c > 0x39) && ((c != 0x2F) || (c != 0x2D))) {
      return;
    }
    break;
  case tb_time:
    if ((c < 0x30) || (c > 0x39) && (c != 0x3A) && (c != 0x41) && (c != 0x61) && (c != 0x70) && (c != 0x50)) {
      return;
    }
    break;
  }
  if (((glbWindow->cursor_pos - tb->col) < (int)strlen(tb->text)) &&
      ((int)strlen(tb->text) < tb->box_len)) {
    if (!over_write) {
      if ((glbWindow->cursor_pos + 1) < (tb->col + (int)strlen(tb->text))) {
        strncpy(KeyPressBuf[0], tb->text, glbWindow->cursor_pos - tb->col + 1);
        strcpy(KeyPressBuf[1], &tb->text[glbWindow->cursor_pos - tb->col + 1]);
        strcat(KeyPressBuf[0], &(char)c);
        strcat(KeyPressBuf[0], KeyPressBuf[1]);
        strcpy(tb->text, KeyPressBuf[0]);
      } else {
        strcat(tb->text, &(char)c);
      }
    } else {
      if ((glbWindow->cursor_pos + 1) < (tb->col + tb->box_len)) {
        tb->text[glbWindow->cursor_pos - tb->col + 1] = c;
      }
    }
    glbWindow->cursor_pos += 1;
    PutTxtBox(tb);
    if (tb->OnChange != NULL) {
      (*tb->OnChange)();
    }
  } else // at end of text box
  {
    if (over_write == TRUE)
      ;
    {
      if ((glbWindow->cursor_pos + 1) < (tb->col + tb->box_len)) {
        tb->text[glbWindow->cursor_pos - tb->col + 1] = c;
        glbWindow->cursor_pos += 1;
        PutTxtBox(tb);
        if (tb->OnChange != NULL) {
          (*tb->OnChange)();
        }
      }
    }
  }
}

static char PutTxtBuf[MAX_TEXT];
void PutTxtBox(TEXT_BOX_T* tb) {
  if (tb->so.visible == FALSE) {
    return;
  }

  hide_mouse();

  memset(PutTxtBuf, 0, MAX_TEXT);

  pbutton(((tb->row - 1) * 14) - 2, ((tb->col - 1) * 8) - 3, 17, ((tb->box_len) * 8) + 4, 1, BG_HWHT);

  //position(tb->row, tb->col);
  if (tb->so.got_focus) {
    if ((glbWindow->cursor_pos > (tb->col + (int)strlen(tb->text))) ||
        (glbWindow->cursor_pos < tb->col - 1)) {
      glbWindow->cursor_pos = tb->col + (strlen(tb->text)) - 1;
    }
    tb_cursor = 1;
    BlinkCursor();
  }
  if (tb->type != tb_pass_word) {
    strcpy(PutTxtBuf, tb->text);
  } else {
    if (strlen(tb->text)) {
      memset(PutTxtBuf, '*', strlen(tb->text));
    }
  }

  SetTxtPos(tb, (tb->row - 1) * 14, (tb->col - 1) * 8);
  tb->so.position.width = tb->box_len * 8;
  ptext(PutTxtBuf, tb->so.position.top, tb->so.position.left, BG_TRANSPARENT + FG_BLK);

  unhide_mouse();
}

void SetTxtPos(TEXT_BOX_T* tb, int top, int left) {
  tb->so.position.top = top;
  tb->so.position.left = left;
}

void TxtClick(TEXT_BOX_T* tb) {
  if (((mouse_info.x / 8) + 1) > (tb->col + (int)strlen(tb->text))) {
    glbWindow->cursor_pos = tb->col + (strlen(tb->text)) - 1;
  } else {
    glbWindow->cursor_pos = (mouse_info.x / 8);
  }
  PutTxtBox(tb);
}

void TxtMouseDown(TEXT_BOX_T* tb) {
  tb->so.got_focus = TRUE;
  if (((mouse_info.x / 8) + 1) > (tb->col + (int)strlen(tb->text))) {
    glbWindow->cursor_pos = tb->col + (strlen(tb->text)) - 1;
  } else {
    glbWindow->cursor_pos = (mouse_info.x / 8);
  }
  PutTxtBox(tb);
}

void TxtMouseUp(TEXT_BOX_T* tb) {
  return;
}

extern int day_tab[2][13];

#if LOAD_CONTROL_TECO
int IsValid(TEXT_BOX_T* tb) {
  char work[257];
  char *cpos, *cpos1;
  int hr, min, mon, day, yr;

  switch (tb->type) {
  case tb_date:
    strcpy(work, tb->text);
    if ((cpos = strchr(work, '/')) == NULL) {
      return (FALSE);
    }
    *cpos = 0;
    ++cpos;

    if ((cpos1 = strchr(cpos, '/')) == NULL) {
      return (FALSE);
    }
    *cpos1 = 0;
    ++cpos1;

    mon = atoi(work);
    day = atoi(cpos);
    yr = atoi(cpos1);
    if ((mon < 1) || (mon > 12)) {
      return (FALSE);
    }
    if ((yr < 95) || (yr > 99)) {
      return (FALSE);
    }
    if (day < 1) {
      return (FALSE);
    }
    if ((yr % 4) == 0) {
      if (day > day_tab[1][mon]) // it's a leap year!!
      {
        return (FALSE);
      }
    } else {
      if (day > day_tab[0][mon]) // it's NOT a leap year!!
      {
        return (FALSE);
      }
    }
    return (TRUE);
  case tb_time:
    strcpy(work, tb->text);
    if ((cpos = strchr(work, ':')) == NULL) {
      return (FALSE);
    }
    *cpos = 0;
    ++cpos;
    hr = atoi(work);
    min = atoi(cpos);
    if ((hr < 1) || (hr > 12)) {
      return (FALSE);
    }
    if ((min < 0) || (min > 59)) {
      return (FALSE);
    }
    return (TRUE);
    break;
  default:
    return (FALSE);
  }
}
#endif
