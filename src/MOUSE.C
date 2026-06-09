#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <dos.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include "2box.h"
#include "vga.h"
#include "mouse.h"

//////////////////////
#include "vga.h"
#include "inutil.h"
#include "scrnobjt.h"
#include "pshbtn.h"
#include "lwindow.h"
#include "ismenu.h"
/////////////////////

MOUSE_INFO_T mouse_info;
static USHORT r_modifier = 28;
static USHORT c_modifier = 2;
static UCHAR far* msaved_window = NULL;

///////////////////
extern PUSH_BUTTON_T isBtnList[14];
static char icon[298];
extern void LoadIcon(char* icon, char* filename);

extern void DisplayIcon(int top, int left, char far* icon);
extern LWINDOW_T IntroWindow[1];
///////////////////
static UCHAR tmp[15];
void show_mouse(void) {
  if ((ticks_check(7) == FALSE) || (glbWindow != IntroWindow) || (mouse_info.buttons != 0)) {
    return;
  }
  ticks_set(7, 8L);
  if ((mouse_info.y < 206) && (mouse_info.x > 379)) {
    LoadIcon(icon, "mood2.ico");
    DisplayIcon(isBtnList[7].so.position.top + 3,
                isBtnList[7].so.position.left + (isBtnList[7].so.position.width - 24) / 2, icon);
  } else if ((mouse_info.y < 206) && (mouse_info.x < 379)) {
    LoadIcon(icon, "mood1.ico");
    DisplayIcon(isBtnList[7].so.position.top + 3,
                isBtnList[7].so.position.left + (isBtnList[7].so.position.width - 24) / 2, icon);
  } else if ((mouse_info.y > 206) && (mouse_info.x > 379)) {
    LoadIcon(icon, "mood3.ico");
    DisplayIcon(isBtnList[7].so.position.top + 3,
                isBtnList[7].so.position.left + (isBtnList[7].so.position.width - 24) / 2, icon);
  } else if ((mouse_info.y > 206) && (mouse_info.x < 379)) {
    LoadIcon(icon, "mood4.ico");
    DisplayIcon(isBtnList[7].so.position.top + 3,
                isBtnList[7].so.position.left + (isBtnList[7].so.position.width - 24) / 2, icon);
  }
#if 0
   if (msaved_window == NULL)
   {
      msaved_window = get_window(r_modifier, c_modifier, 8, 6);
   }
   if (mouse_info.buttons == 2)
   {
      put_window(msaved_window);
      r_modifier = (mouse_info.y / 14) + 1;
      c_modifier = (mouse_info.x / 8) + 1;
      msaved_window = get_window(r_modifier, c_modifier, 8, 6);
   }
   position(r_modifier, c_modifier);
   sprintf(tmp, "BTN %3d", mouse_info.buttons);
   text(tmp, BG_BLU + FG_HWHT);

   position(r_modifier + 1, c_modifier);
   sprintf(tmp, "X   %3d", mouse_info.x);
   text(tmp, BG_BLU + FG_HWHT);

   position(r_modifier + 2, c_modifier);
   sprintf(tmp, "Y   %3d", mouse_info.y);
   text(tmp, BG_BLU + FG_HWHT);

   position(r_modifier + 3, c_modifier);
   sprintf(tmp, "Row %3d", (mouse_info.y / 14) + 1);
   text(tmp, BG_BLU + FG_HWHT);

   position(r_modifier + 4, c_modifier);
   sprintf(tmp, "Col %3d", (mouse_info.x / 8) + 1);
   text(tmp, BG_BLU + FG_HWHT);
   ticks_set(7, 2L);
#endif
}

int MouseOver(COORDINATE_T* pos) {
  int result = FALSE;
  if ((mouse_info.x > pos->left) &&
      (mouse_info.x < pos->left + pos->width) &&
      (mouse_info.y > pos->top) &&
      (mouse_info.y < pos->top + pos->height)) {
    result = TRUE;
  }
  return result;
}

int im_busy;

CURSOR_DEF_T arrow_ms = {
    1,
    1,

    0x9FFF,
    0x8FFF,
    0x87FF,
    0x83FF,
    0x81FF,
    0x80FF,
    0x807F,
    0x803F,
    0x801F,
    0x800F,
    0x80FF,
    0x887F,
    0x987F,
    0xBC3F,
    0xFC3F,
    0xFE3F,

    0x0000,
    0x2000,
    0x3000,
    0x3800,
    0x3C00,
    0x3E00,
    0x3F00,
    0x3F80,
    0x3FC0,
    0x3E00,
    0x3600,
    0x2300,
    0x0300,
    0x0180,
    0x0180,
    0x0000};

CURSOR_DEF_T hourglass_ms = {
    1,
    1,

    0xE003,
    0xE003,
    0xE003,
    0xF007,
    0xF007,
    0xF007,
    0xF80F,
    0xFC1F,
    0xFC1F,
    0xF80F,
    0xF007,
    0xF007,
    0xF007,
    0xE003,
    0xE003,
    0xE003,

    0x0000,
    0x0FF8,
    0x0000,
    0x07F0,
    0x07F0,
    0x0550,
    0x02A0,
    0x0140,
    0x01C0,
    0x0360,
    0x06B0,
    0x0550,
    0x07F0,
    0x0000,
    0x0FF8,
    0x0000};

CURSOR_DEF_T finger_ms = {
    5,
    1,

    0xF3FF,
    0xE1FF,
    0xE1FF,
    0xE1FF,
    0xE07F,
    0xE00F,
    0x8001,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x8000,
    0x8000,
    0xC001,
    0xC001,
    0xC001,

    0x0000,
    0x0C00,
    0x0C00,
    0x0C00,
    0x0C00,
    0x0D80,
    0x0DB0,
    0x6DB6,
    0x6DB6,
    0x6FFE,
    0x7FFE,
    0x3FFE,
    0x3FFE,
    0x0000,
    0x1FFC,
    0x0000};

CURSOR_DEF_T bar_ms = {
    4,
    8,

    0xFFFF,
    0x11FF,
    0xEFFF,
    0xEFFF,
    0xEFFF,
    0xEFFF,
    0xEFFF,
    0xEFFF,
    0xEFFF,
    0xEFFF,
    0xEFFF,
    0xEFFF,
    0xEFFF,
    0xEFFF,
    0x11FF,
    0xFFFF,

    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
};

int which_mouse; //0 = arrow, 1 = finger... from mloop.c

void Busy(void) {
  //   show_logo(0, 77, 3, 24, busy_v + 10);
  mouse_cursor(&hourglass_ms);
  im_busy = TRUE;
}

void NotBusy(void) {
  //   show_logo(0, 77, 3, 24, notbusy_v + 10);
  switch (which_mouse) {
  case M_ARROW:
    mouse_cursor(&arrow_ms);
    break;
  case M_FINGER:
    mouse_cursor(&finger_ms);
    break;
  case M_TEXTBAR:
    mouse_cursor(&bar_ms);
    break;
  }
  im_busy = FALSE;
}
