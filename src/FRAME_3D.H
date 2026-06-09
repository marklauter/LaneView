#ifndef _frame_3d_h
#define _frame_3d_h

#define FT_RAISED 0
#define FT_PRESSED 1
#define FT_RAISED_LINE 2
#define FT_RECESSED_LINE 3
#define FT_FLAT 4

extern void frame_3d(int top, int left, int height, int width, char type);
extern void text_3d(UCHAR* text, int top, int left);
extern void GroupBox(UCHAR* text, int top, int left, int height, int width);

#endif
