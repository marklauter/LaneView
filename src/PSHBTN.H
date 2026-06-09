//   Copyright 1996 TeCom Inc. All rights reserved
//  File creation date 30 OCT 96
//  Created by Mark Lauter : TMMSL
//   Last modified:

#ifndef _PUSH_BUTTON_T_H
#define _PUSH_BUTTON_T_H

typedef struct
{
  SCREEN_OBJECT_T so;
  char default_button;
  UCHAR* text;
  enum BTN_TYPE { set,
                  push } btn_type;
  enum STATE { up,
               down,
               flat } state;
  UCHAR* icon;   //allows 32x32 16 color icon, ununsed at this time
                 //oid    (_far * pbf)();   // pbf() calls the button's action function
  void (*pbf)(); // pbf() calls the button's action function
  int btnlog;
} PUSH_BUTTON_T;

#define PB_UP 0
#define PB_DN 1
extern void pbutton(int top, int left, int height, int width, char type, int fill_color);
extern void pbutton_nc(int top, int left, int height, int width, char type, int fill_color);
extern void clear_button(int top_row, int left_col, int bottom_row, int right_col);

extern void BtnClick(PUSH_BUTTON_T* pb);
extern void BtnKeyPress(PUSH_BUTTON_T* pb, int c);
extern void BtnMouseDown(PUSH_BUTTON_T* pb);
extern void BtnMouseUp(PUSH_BUTTON_T* pb);
extern void PutButton(PUSH_BUTTON_T* pb);

#endif
