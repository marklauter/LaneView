#ifndef _check_box_h
#define _check_box_h

typedef struct
{
  SCREEN_OBJECT_T so;
  //UCHAR    text[51];
  UCHAR* text;
  int checked;       // use TRUE or FALSE
  void(_far* cbf)(); // cbf() calls the check box's action function
} CHECK_BOX_T;

extern void DisplayCheckBox(CHECK_BOX_T* cb);
extern void CheckBoxOnKeyPress(CHECK_BOX_T* cb, int c);
extern void CheckBoxOnClick(CHECK_BOX_T* cb);
extern void CheckBoxOnMouseDown(CHECK_BOX_T* cb);
extern void CheckBoxOnMouseUp(CHECK_BOX_T* cb);

#endif
