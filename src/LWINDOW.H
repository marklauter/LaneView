// Copyright 1996 TeCom Inc. All rights reserved
// File creation date 31 OCT 96
// Created by Mark Lauter : TMMSL
// Last modified:
#ifndef _LWINDOW_H
#define _LWINDOW_H

typedef struct
{
  COORDINATE_T win_pos;
  SCREEN_OBJECT_T** so;
  void(_far* Draw)(); //(* pbf)(); <- thats a call!!!
  int item_count;
  int cursor_pos;
  FOCUS_T focus;
  UCHAR* title;
  void(_far* OnKeyPress)(); //(* pbf)(); <- thats a call!!!
  int default_item;         //index to so
} LWINDOW_T;

extern void PutWindow(LWINDOW_T* thisWindow);
extern void ShowTitle(char* title);
extern void DefaultOnKeyPress(SCREEN_OBJECT_T* so, int c);

extern LWINDOW_T* glbWindow;

#endif
