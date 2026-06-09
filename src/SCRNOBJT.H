#ifndef _screen_object_h
#define _screen_object_h

typedef struct
{
  // data...
  COORDINATE_T position;
  char got_focus;
  enum OBJECT_TYPE {
    PUSH_BUTTON,
    TEXT_BOX,
    LIST_BOX,
    CHECK_BOX,
    SCROLL_BARV,
    SCROLL_BARH,
    DROP_DOWN_BOX,
    TIME_ENTRY,
    MEMO_BOX
  } type;
  int visible; //use TRUE or FALSE

  // methods...
  void(_far* OnMouseDown)();
  void(_far* OnMouseUp)();
  void(_far* OnClick)(); //(* pbf)(); <- thats a call!!!
  void(_far* OnKeyPress)();
  void(_far* DisplayObject)();
  //void   (_far * SetFocus)();

} SCREEN_OBJECT_T;

#endif
