#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <dos.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "2box.h"
#include "inutil.h"
#include "vga.h"
#include "mouse.h"
#include "scrnobjt.h"
#include "pshbtn.h"
#include "textbox.h"
#include "lwindow.h"
#include "mloop.h"
#include "frame_3d.h"

static void BtnLog(PUSH_BUTTON_T* pb);

//=====================================
// display 24x24 16 color icon
//=====================================
extern void DisplayIcon(int top, int left, char far* icon);

void pbutton_body(int top, int left, int height, int width, char type, int fill_color, int erase) {
  //type:  PB_UP or PB_DN
  switch (fill_color) {
  case BG_BLK:
    fill_color = FG_BLK;
    break;
  case BG_BLU:
    fill_color = FG_BLU;
    break;
  case BG_GRN:
    fill_color = FG_GRN;
    break;
  case BG_CYN:
    fill_color = FG_CYN;
    break;
  case BG_RED:
    fill_color = FG_RED;
    break;
  case BG_MAG:
    fill_color = FG_MAG;
    break;
  case BG_TRANSPARENT:
    fill_color = FG_BRN;
    break;
  case BG_WHT:
    fill_color = FG_WHT;
    break;
  case BG_GRY:
    fill_color = FG_GRY;
    break;
  case BG_LBLU:
    fill_color = FG_LBLU;
    break;
  case BG_LGRN:
    fill_color = FG_LGRN;
    break;
  case BG_LCYN:
    fill_color = FG_LCYN;
    break;
  case BG_LRED:
    fill_color = FG_LRED;
    break;
  case BG_LMAG:
    fill_color = FG_LMAG;
    break;
  case BG_YEL:
    fill_color = FG_YEL;
    break;
  case BG_HWHT:
    fill_color = FG_HWHT;
    break;
  }
  hide_mouse();
  if (type == PB_UP) {
    hline(top, left, left + width, FG_WHT);
    hline(top + 1, left + 1, left + width, FG_HWHT);
    hline(top + height - 1, left, left + width, FG_GRY);
    hline(top + height, left, left + width, FG_BLK);
    vline(left, top, top + height - 1, FG_WHT);
    vline(left + 1, top + 1, top + height - 2, FG_HWHT);
    vline(left + width - 1, top + 1, top + height - 1, FG_GRY);
    vline(left + width, top, top + height, FG_BLK);
    if (erase != FALSE) {
      if (((left + width - 2) - (left + 2)) > 1) {
        area_clear(top + 2, top + height - 2, left + 2, left + width - 2, fill_color);
      }
    }
  } else {
    hline(top, left, left + width, FG_GRY);
    hline(top + 1, left, left + width, FG_BLK);
    hline(top + height, left, left + width, FG_HWHT);
    hline(top + height - 1, left + 1, left + width - 1, FG_WHT);
    vline(left, top, top + height, FG_GRY);
    vline(left + 1, top + 1, top + height - 1, FG_BLK);
    vline(left + width, top + 1, top + height, FG_HWHT);
    vline(left + width - 1, top + 2, top + height - 1, FG_WHT);
    if (erase != FALSE) {
      if (((left + width - 2) - (left + 2)) > 1) {
        area_clear(top + 2, top + height - 2, left + 2, left + width - 2, fill_color);
      }
    }
  }
  unhide_mouse();
}

void pbutton_nc(int top, int left, int height, int width, char type, int fill_color) {
  pbutton_body(top, left, height, width, type, fill_color, FALSE); // erase == FALSE
}

void pbutton(int top, int left, int height, int width, char type, int fill_color) {
  pbutton_body(top, left, height, width, type, fill_color, TRUE); // erase == TRUE
}

void PutButton(PUSH_BUTTON_T* pb) {
  int v_center = 0;
  int text_focus_color = FG_BLK + BG_BLK, btn_fcs_clr = FG_BLK + BG_BLK;
  int i = 0, j = 0;
  char buf[3][25];

  if (pb->so.visible == FALSE) {
    return;
  }
  memset(buf, 0, 75);

  for (i = 0; i < (signed)strlen(pb->text); i++) {
    if (pb->text[i] == 124) // vertical bar character
    {
      strcat(buf[j], "\0");
      j++;
    } else {
      strncat(buf[j], &pb->text[i], 1);
    }
  }
  for (i = 0; i < (j + 1); i++) {
    trim(buf[i]);
  }

  if ((pb->icon != NULL) && (pb->default_button != TRUE)) // add space for icon at top
  {
    v_center = (pb->so.position.height - (((j + 1) * 14) + 28)) / 2 + 24;
  } else if ((pb->icon == NULL) || (pb->default_button == TRUE)) {
    v_center = (pb->so.position.height - ((j + 1) * 14)) / 2; // center text vertically on button
  }

  text_focus_color = FG_BLU + BG_WHT;

  //   if (pb->btn_type != set)
  //   {
  //      if(pb->state != down)
  //      {
  if (pb->so.got_focus) {
    //            pb->state = up;
    btn_fcs_clr = FG_BLK;
    //      btn_fcs_clr = FG_BLK;
    //      text_focus_color = FG_BLU+BG_WHT;
    //      get_font_loc(font_array[BOLD]);
  } else {
    //            pb->state = flat;
    btn_fcs_clr = FG_WHT;
    //      text_focus_color = FG_BLK+BG_WHT;
    //      get_font_loc(font_array[LGHT]);
  }
  //      }
  //   }
  //   else
  //   {
  //      if (pb->so.got_focus)
  //      {
  //         btn_fcs_clr = FG_BLU;
  //      }
  //   }
  hide_mouse();
  if (pb->state == up) {
    area_clear(pb->so.position.top + 1,
               pb->so.position.top + pb->so.position.height - 2,
               pb->so.position.left + 1,
               pb->so.position.left + pb->so.position.width - 2,
               FG_WHT);
    if (pb->default_button != TRUE) {
      for (i = 0; i < j + 1; i++) {
        ptext(buf[i],
              pb->so.position.top + v_center + (i * 14),
              pb->so.position.left + (pb->so.position.width - (strlen(buf[i]) * 8)) / 2,
              text_focus_color);
      }
    } else {
      for (i = 0; i < j + 1; i++) {
        ptext(buf[i],
              pb->so.position.top + v_center + (i * 14),
              pb->so.position.left + 7,
              text_focus_color);
      }
    }
    hline(pb->so.position.top,
          pb->so.position.left,
          pb->so.position.left + pb->so.position.width,
          FG_HWHT);
    hline(pb->so.position.top + pb->so.position.height - 1,
          pb->so.position.left + 1,
          pb->so.position.left + pb->so.position.width,
          FG_GRY);
    hline(pb->so.position.top + pb->so.position.height,
          pb->so.position.left,
          pb->so.position.left + pb->so.position.width,
          FG_BLK);
    vline(pb->so.position.left - 1,
          pb->so.position.top,
          pb->so.position.top + pb->so.position.height - 1,
          FG_WHT);
    vline(pb->so.position.left,
          pb->so.position.top,
          pb->so.position.top + pb->so.position.height - 1,
          FG_HWHT);
    vline(pb->so.position.left + pb->so.position.width - 1,
          pb->so.position.top + 1,
          pb->so.position.top + pb->so.position.height - 1,
          FG_GRY);
    vline(pb->so.position.left + pb->so.position.width,
          pb->so.position.top,
          pb->so.position.top + pb->so.position.height,
          FG_BLK);

    hline(pb->so.position.top - 1,
          pb->so.position.left,
          pb->so.position.left + pb->so.position.width,
          btn_fcs_clr);
    vline(pb->so.position.left - 1,
          pb->so.position.top,
          pb->so.position.top + pb->so.position.height,
          btn_fcs_clr);
    hline(pb->so.position.top + pb->so.position.height + 1,
          pb->so.position.left,
          pb->so.position.left + pb->so.position.width,
          btn_fcs_clr);
    vline(pb->so.position.left + pb->so.position.width + 1,
          pb->so.position.top,
          pb->so.position.top + pb->so.position.height,
          btn_fcs_clr);
#if 0
      hline(pb->so.position.top + 3,
         pb->so.position.left + 4,
         pb->so.position.left + pb->so.position.width - 5,
         btn_fcs_clr);
      vline(pb->so.position.left + 3,
         pb->so.position.top + 4,
         pb->so.position.top + pb->so.position.height - 5,
         btn_fcs_clr);
      hline(pb->so.position.top + pb->so.position.height - 4,
         pb->so.position.left + 4,
         pb->so.position.left + pb->so.position.width - 5,
         btn_fcs_clr);
      vline(pb->so.position.left + pb->so.position.width - 4,
         pb->so.position.top + 4,
         pb->so.position.top + pb->so.position.height - 5,
         btn_fcs_clr);
#endif
  } else if (pb->state == down) {
    area_clear(pb->so.position.top + 2,
               pb->so.position.top + pb->so.position.height - 1,
               pb->so.position.left + 2,
               pb->so.position.left + pb->so.position.width - 1,
               FG_WHT);

    if (pb->default_button != TRUE) {
      for (i = 0; i < j + 1; i++) {
        ptext(buf[i],
              pb->so.position.top + v_center + (i * 14) + 2,
              pb->so.position.left + (pb->so.position.width - (strlen(buf[i]) * 8)) / 2 + 2,
              text_focus_color);
      }
    } else {
      for (i = 0; i < j + 1; i++) {
        ptext(buf[i],
              pb->so.position.top + v_center + (i * 14) + 2,
              pb->so.position.left + 7,
              text_focus_color);
      }
    }

    hline(pb->so.position.top,
          pb->so.position.left,
          pb->so.position.left + pb->so.position.width,
          FG_GRY);
    hline(pb->so.position.top + 1,
          pb->so.position.left,
          pb->so.position.left + pb->so.position.width,
          FG_BLK);
    hline(pb->so.position.top + pb->so.position.height,
          pb->so.position.left,
          pb->so.position.left + pb->so.position.width,
          FG_HWHT);
    vline(pb->so.position.left,
          pb->so.position.top,
          pb->so.position.top + pb->so.position.height,
          FG_GRY);
    vline(pb->so.position.left + 1,
          pb->so.position.top + 1,
          pb->so.position.top + pb->so.position.height - 1,
          FG_BLK);
    vline(pb->so.position.left + pb->so.position.width,
          pb->so.position.top + 1,
          pb->so.position.top + pb->so.position.height,
          FG_HWHT);

    hline(pb->so.position.top - 1,
          pb->so.position.left,
          pb->so.position.left + pb->so.position.width,
          btn_fcs_clr);
    vline(pb->so.position.left - 1,
          pb->so.position.top,
          pb->so.position.top + pb->so.position.height,
          btn_fcs_clr);
    hline(pb->so.position.top + pb->so.position.height + 1,
          pb->so.position.left,
          pb->so.position.left + pb->so.position.width,
          btn_fcs_clr);
    vline(pb->so.position.left + pb->so.position.width + 1,
          pb->so.position.top,
          pb->so.position.top + pb->so.position.height,
          btn_fcs_clr);
#if 0
      hline(pb->so.position.top + 4,
         pb->so.position.left + 5,
         pb->so.position.left + pb->so.position.width - 4,
         btn_fcs_clr);
      vline(pb->so.position.left + 4,
         pb->so.position.top + 5,
         pb->so.position.top + pb->so.position.height - 4,
         btn_fcs_clr);
      hline(pb->so.position.top + pb->so.position.height - 3,
         pb->so.position.left + 5,
         pb->so.position.left + pb->so.position.width - 4,
         btn_fcs_clr);
      vline(pb->so.position.left + pb->so.position.width - 3,
         pb->so.position.top + 5,
         pb->so.position.top + pb->so.position.height - 4,
         btn_fcs_clr);
#endif
  } else if (pb->state == flat) {
    area_clear(pb->so.position.top - 1,
               pb->so.position.top + pb->so.position.height + 2,
               pb->so.position.left - 1,
               pb->so.position.left + pb->so.position.width + 2,
               FG_WHT);
#if 0
      hline(pb->so.position.top,
         pb->so.position.left + 1,
         pb->so.position.left + pb->so.position.width - 1,
         FG_CYN);
      vline(pb->so.position.left,
         pb->so.position.top + 1,
         pb->so.position.top + pb->so.position.height - 1,
         FG_CYN);
      hline(pb->so.position.top + pb->so.position.height,
         pb->so.position.left + 1,
         pb->so.position.left + pb->so.position.width - 1,
         FG_CYN);
      vline(pb->so.position.left + pb->so.position.width,
         pb->so.position.top + 1,
         pb->so.position.top + pb->so.position.height - 1,
         FG_CYN);
#endif
    hline(pb->so.position.top,
          pb->so.position.left,
          pb->so.position.left + pb->so.position.width,
          FG_HWHT);
    vline(pb->so.position.left,
          pb->so.position.top,
          pb->so.position.top + pb->so.position.height,
          FG_HWHT);
    hline(pb->so.position.top + pb->so.position.height,
          pb->so.position.left,
          pb->so.position.left + pb->so.position.width,
          FG_GRY);
    vline(pb->so.position.left + pb->so.position.width,
          pb->so.position.top,
          pb->so.position.top + pb->so.position.height,
          FG_GRY);

    if (pb->default_button != TRUE) {
      for (i = 0; i < j + 1; i++) {
        ptext(buf[i],
              pb->so.position.top + v_center + (i * 14) + 1,
              pb->so.position.left + (pb->so.position.width - (strlen(buf[i]) * 8)) / 2 + 1,
              text_focus_color);
      }
    } else {
      for (i = 0; i < j + 1; i++) {
        ptext(buf[i],
              pb->so.position.top + v_center + (i * 14) + 1,
              pb->so.position.left + 7,
              text_focus_color);
      }
    }
  }
  if (pb->icon != NULL) {
    if (pb->default_button == TRUE) {
      DisplayIcon(pb->so.position.top + ((pb->so.position.height - 24) / 2),
                  pb->so.position.left + pb->so.position.width - 34,
                  (char far*)pb->icon);
    } else {
      DisplayIcon(pb->so.position.top + 3,
                  pb->so.position.left + (pb->so.position.width - 24) / 2,
                  (char far*)pb->icon);
    }
  }
  //get_font_loc(font_array[LGHT]);//reset to light font
  unhide_mouse();
}

void clear_button(int trow, int lcol, int brow, int rcol) {
  trow = (trow - 1) * 14 + 7;
  brow = (brow - 1) * 14 + 7;
  lcol = (lcol - 1) * 8 + 4;
  rcol = (rcol - 1) * 8 + 4;
  hide_mouse();
  area_clear(trow + 3, brow - 2, lcol + 3, rcol - 3, FG_WHT);
  unhide_mouse();
}

void BtnMouseDown(PUSH_BUTTON_T* pb) {
  switch (pb->btn_type) {
  case push:
    if (pb->state == up) {
      pb->so.got_focus = TRUE;
      pb->state = down;
      PutButton(pb);
    }
    break;
  case set:
    if (pb->so.got_focus != TRUE) {
      pb->so.got_focus = TRUE;
      PutButton(pb);
    }
    break;
  }
}

void BtnMouseUp(PUSH_BUTTON_T* pb) {
  switch (pb->btn_type) {
  case push:
    if (pb->state == down) {
      pb->state = up;
      PutButton(pb);
    }
    break;
  case set:
    if ((pb->state == up) && (MouseOver(&pb->so.position))) {
      pb->state = down;
      PutButton(pb);
    }
    break;
  }
}

void BtnClick(PUSH_BUTTON_T* pb) {
  int i = 0;
  PUSH_BUTTON_T* pb_last;

  //button type == push
  switch (pb->btn_type) {
  case push:
    pb->state = up;
    PutButton(pb);
    if (pb->pbf != NULL) {
      BtnLog(pb);
      (*pb->pbf)();
    }
    break;
  case set:
    for (i = 0; i < glbWindow->item_count; i++) {
      if (glbWindow->so[i]->type == PUSH_BUTTON) {
        pb_last = (PUSH_BUTTON_T*)glbWindow->so[i];
        if ((pb_last->btn_type == set) &&
            (pb_last->state == down) &&
            (glbWindow->focus.current != i)) {
          pb_last->state = up;
          PutButton(pb_last);
        }
      }
    }
    if (pb->state == up) {
      pb->state = down;
    }
    PutButton(pb);
    if (pb->pbf != NULL) {
      BtnLog(pb);
      (*pb->pbf)();
    }
    break;
  }
}

void BtnKeyPress(PUSH_BUTTON_T* pb, int c) {
  switch (c) {
  case CR: // enter
  case 32: // space
    BtnClick(pb);
    break;
  case 0:
    c = kbchar();
    switch (c) {
    case 77: // right arrow
    case 80:
      TabRight();
      break;
    case 75: // left arrow
    case 72:
      TabLeft();
      break;
    }
    break;
  }
}

static void BtnLog(PUSH_BUTTON_T* pb) {
  FILE* fbtnlog;
  struct tm* tod;

  time(&ltime);
  tod = localtime(&ltime);

  if ((fbtnlog = fopen("user.log", "at")) != NULL) {
    fprintf(fbtnlog, "%0.2d/%0.2d/%0.2d\t%0.2d:%0.2d:%0.2d\tBTN: %d\t\t%s\tWINDOW: %s\n",
            this_month, today, this_year,
            tod->tm_hour, tod->tm_min, tod->tm_sec,
            pb->btnlog,
            pb->text,
            glbWindow->title);
    fclose(fbtnlog);
  } else {
    printf("Error opening userlog.dat");
  }
}
