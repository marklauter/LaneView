//   Copyright 1996 TeCom Inc. All rights reserved
//  File creation date 30 OCT 96
//  Created by Mark Lauter : TMMSL
//   Last modified:

#ifndef _TEXTBOX_H
#define _TEXTBOX_H

#define BLINK_RATE 7L // cursor blink rate
#define BlinkTimer 4
#define MAX_TEXT 81

typedef struct
{
  // data...
  SCREEN_OBJECT_T so;
  char row, col, box_len; //box_len is in characters
  enum TYPE { tb_alpha,
              tb_int,
              tb_float,
              tb_date,
              tb_time,
              tb_pass_word,
              tb_hex } type;
  UCHAR text[MAX_TEXT];
  // methods...
  void(_far* OnChange)();
} TEXT_BOX_T;

//extern int      MAX_TXT;
extern int cursor_pos;
extern UCHAR over_write;

extern void BlinkCursor(void);
extern void TxtKeyPress(TEXT_BOX_T* tb, int c);
extern void PutTxtBox(TEXT_BOX_T* tb);
extern void SetTxtPos(TEXT_BOX_T* tb, int top, int left);
extern void TxtClick(TEXT_BOX_T* tb);
extern void TxtMouseDown(TEXT_BOX_T* tb);
extern void TxtMouseUp(TEXT_BOX_T* tb);
#if LOAD_CONTROL_TECO
extern int IsValid(TEXT_BOX_T* tb);
#endif

#endif
