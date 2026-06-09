#ifndef _listbox_h
#define _listbox_h

typedef struct
{
  //data
  SCREEN_OBJECT_T so;
  int item_count,
      item_selected, // 0 is first item on list
      top_item,
      display_count;

  void** sub_list;

  void(_far* DisplayItem)(); // calls the appropriate diplay routine for the items on the list
  void(_far* OnChange)();
  //void         (_far * OnScroll)();   //(* OnScroll)(); <- thats a call!!!
} LISTBOX_T;

extern void ListBoxScroll(LISTBOX_T* lb);
extern void PutListBox(LISTBOX_T* lb);
extern void ListBoxMouseDown(LISTBOX_T* lb);
extern void ListBoxMouseUp(LISTBOX_T* lb);
extern void ListBoxClick(LISTBOX_T* lb);
extern void ListBoxKeyPress(LISTBOX_T* lb, int c);
extern void PutListBox(LISTBOX_T* lb);

extern void DownArrow(LISTBOX_T* lb);
extern void UpArrow(LISTBOX_T* lb);
extern void PageUp(LISTBOX_T* lb);
extern void PageDown(LISTBOX_T* lb);

#endif
