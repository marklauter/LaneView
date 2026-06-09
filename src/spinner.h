#ifndef _spinner_h
#define _spinner_h

// contains up to 5 text box chunks
// contains a spinner control

typedef struct
{
  SCREEN_OBJECT_T so;
  int value;
  int max;
  int min;
  int increment;
  void(_far* OnChange)();
} SPINNER_T;

extern void DisplaySpinner(SPINNER_T* sp);
extern void SpinnerClick(SPINNER_T* sp);
extern void SpinnerKeyPress(SPINNER_T* sp, int key);
extern void SpinnerMouseDown(SPINNER_T* sp);
extern void SpinnerMouseUp(SPINNER_T* sp);

#endif
