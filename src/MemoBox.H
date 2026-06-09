#ifndef _memobox_h
#define _memobox_h

#ifndef _scrolbar_h
#include "scrolbar.h"
#endif

#define MEMO_MAX_LINES 256

typedef struct
{
  SCREEN_OBJECT_T so;

  int max_length;
  int enabled;

  UCHAR* text;

  // attached scroll bar, if any
  SCROLL_BAR_T* sbobj;

  void(_far* OnChange)();

  int length;

  int line_start[MEMO_MAX_LINES];
  int line_length[MEMO_MAX_LINES];
  UCHAR cursor_col; // current column
  UCHAR cursor_row; // current row
  UCHAR rows;
  // Displayed area variables
  int display_top;
  int display_height;
} MEMOBOX_T;

// function call example
//void            (_far * OnChange)();   //(* OnScroll)(); <- thats a call!!!

extern void MemoBoxScroll(MEMOBOX_T* memo_box, int direction);
extern void MemoBoxPaint(MEMOBOX_T* memo_box);
extern void MemoBoxMouseDown(MEMOBOX_T* memo_box);
extern void MemoBoxMouseUp(MEMOBOX_T* memo_box);
extern void MemoBoxClick(MEMOBOX_T* memo_box);
extern void MemoBoxKeyPress(MEMOBOX_T* memo_box, int c);

extern void MemoDownArrow(MEMOBOX_T* memo_box);
extern void MemoUpArrow(MEMOBOX_T* memo_box);
extern void MemoPageUp(MEMOBOX_T* memo_box);
extern void MemoPageDown(MEMOBOX_T* memo_box);
extern void MemoRefreshAll(MEMOBOX_T* memo);

#endif
