//   Copyright 1996 TeCom Inc. All rights reserved
//  File creation date 30 OCT 96
//  Created by Mark Lauter : TMMSL
//   Last modified:

#ifndef _MOUSE_H
#define _MOUSE_H

#include "vga.h"

typedef struct
{
  int buttons;
  int y;
  int x;
} MOUSE_INFO_T;

#define M_ARROW 0
#define M_TIMER 1
#define M_FINGER 3
#define M_TEXTBAR 4

extern void Busy(void);
extern void NotBusy(void);
typedef struct {
  SSHORT hot_col;
  SSHORT hot_row;

  USHORT mask[16];

  USHORT cursor[16];
} CURSOR_DEF_T;

extern MOUSE_INFO_T mouse_info;
extern mouse_data(MOUSE_INFO_T* mouse_info);
extern void show_mouse(void);            //shows mouse x,y,button values for debugging
extern int MouseOver(COORDINATE_T* pos); //returns T if mouse over selected object
extern int mouse_pressed(void);
extern void hide_mouse(void);
extern void unhide_mouse(void);
extern CURSOR_DEF_T finger_ms;
extern CURSOR_DEF_T arrow_ms;
extern CURSOR_DEF_T bar_ms;

#endif
