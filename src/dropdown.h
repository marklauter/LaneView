#ifndef _drop_down_box_h
#define _drop_down_box_h

typedef struct
{
  SCREEN_OBJECT_T so;
  UCHAR** strings;
  int selected,
      dropped,
      count;
  void(_far* OnChange)();
} DROPDOWN_BOX_T;

extern void DisplayDropDown(DROPDOWN_BOX_T* ddb);
extern void DropDownClick(DROPDOWN_BOX_T* ddb);
extern void ShowDDList(DROPDOWN_BOX_T* ddb);
extern void DropDownKeyPress(DROPDOWN_BOX_T* ddb, int c);
extern void DropDownMouseDown(DROPDOWN_BOX_T* ddb);
extern void DropDownMouseUp(DROPDOWN_BOX_T* ddb);

#endif
