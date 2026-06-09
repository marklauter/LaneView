#ifndef _scroll_bar_h
#define _scroll_bar_h

typedef struct
{
  SCREEN_OBJECT_T so;
  int min;
  int max;
  int small_chg;
  int large_chg;
  int thumb_pos;
  int type;

  void(_far* OnScroll)();

  int attached_object;

} SCROLL_BAR_T;

#define SB_VERTICAL 0
#define SB_HORIZONTAL 1
#define SCROLL_LEFT 0
#define SCROLL_RIGHT 1
#define SCROLL_UP 0
#define SCROLL_DOWN 1
#define SCROLL_SMALL 0
#define SCROLL_LARGE 1

extern void DisplayScrollBar(SCROLL_BAR_T* sb);
extern void ScrollBarOnClick(SCROLL_BAR_T* sb);
extern void ScrollBarOnKeyPress(SCROLL_BAR_T* sb, int c);
extern void ScrollBarOnMouseDown(SCROLL_BAR_T* sb);
extern void ScrollBarOnMouseUp(SCROLL_BAR_T* sb);
extern void BlinkScrollBar(void);

#endif
