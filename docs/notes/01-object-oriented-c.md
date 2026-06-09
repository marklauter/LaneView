---
title: Object-oriented programming in plain C
summary: How LaneView builds classes, single inheritance, virtual dispatch, and RTTI out of structs and function pointers — no C++ required.
tags: [laneview, oop, c, architecture, note]
created: 2026-06-08
aliases: []
document.status: draft
---

# Object-oriented programming in plain C

LaneView has no C++. The toolchain (`ORIGIN = MSVC, ORIGIN_VER = 1.00`) barely had a
usable C++ in it. And yet the entire GUI runs on a real class system — objects with data
and methods, single inheritance, virtual dispatch, and a form of RTTI — assembled from
nothing but `struct`, function pointers, and one deep truth about the C memory model.

## The base class

`SCRNOBJT.H` defines the root of the hierarchy. Read it as a class declaration:

```c
typedef struct
{
  // data...
  COORDINATE_T position;
  char  got_focus;
  enum OBJECT_TYPE {
    PUSH_BUTTON, TEXT_BOX, LIST_BOX, CHECK_BOX,
    SCROLL_BARV, SCROLL_BARH, DROP_DOWN_BOX, TIME_ENTRY, MEMO_BOX
  } type;
  int visible;

  // methods...
  void (_far* OnMouseDown)();
  void (_far* OnMouseUp)();
  void (_far* OnClick)();        //(* pbf)(); <- thats a call!!!
  void (_far* OnKeyPress)();
  void (_far* DisplayObject)();
} SCREEN_OBJECT_T;
```

Three things are happening:

1. State — `position`, `got_focus`, `visible`.
2. A type tag — `enum OBJECT_TYPE type`. This is hand-rolled RTTI: every object knows what
   it is at runtime.
3. A vtable, inlined — five `_far` function pointers. These are the virtual methods.
   `DisplayObject` is the paint method; the rest are event handlers.

The giddy comment `//(* pbf)(); <- thats a call!!!` is the sound of someone realizing that
function pointers let you store behavior in data. That realization is the whole toolkit.

## Inheritance via first-member layout

Every widget "subclass" embeds `SCREEN_OBJECT_T` as its first member. From `PSHBTN.H`:

```c
typedef struct
{
  SCREEN_OBJECT_T so;     // <-- base class, FIRST
  char  default_button;
  UCHAR* text;
  enum BTN_TYPE { set, push } btn_type;
  enum STATE { up, down, flat } state;
  UCHAR* icon;
  void (*pbf)();          // the button's action callback
  int   btnlog;
} PUSH_BUTTON_T;
```

`SCROLL_BAR_T` (`scrolbar.h`), `TEXT_BOX_T`, `LISTBOX_T`, `DROPDOWN_BOX_T`, `SPINNER_T`,
`CHECK_BOX_T`, `TIME_ENTRY_T`, `MEMO_BOX_T` — all start the same way.

The C standard guarantees a struct's first member sits at offset 0 with no leading padding.
So a `PUSH_BUTTON_T*` and a `SCREEN_OBJECT_T*` point at the same address. That's exactly how
a C++ compiler implements single inheritance. LaneView does by hand what
`class PushButton : public ScreenObject` would do for you.

- Upcast (subclass → base): take the address of the embedded member.
  ```c
  billSO[i] = &billBtnList[i].so;     // PUSH_BUTTON_T*  ->  SCREEN_OBJECT_T*
  ```
- Downcast (base → subclass): a plain pointer cast, guarded by the `type` tag.
  ```c
  sbptr = (SCROLL_BAR_T*)glbWindow->so[item_index];   // MLOOP.C
  ddb   = (DROPDOWN_BOX_T*)(glbWindow->so[item_index]);
  ```

## Virtual dispatch

A window (`LWINDOW_T`, `LWINDOW.H`) holds a polymorphic array of base pointers:

```c
SCREEN_OBJECT_T** so;
```

It never needs each object's concrete type to paint them. It calls the virtual method,
passing the object as an explicit `this`:

```c
// PutWindow(), lwindow.c
for (i = 0; i < thisWindow->item_count; i++) {
  if (thisWindow->so[i]->visible == TRUE) {
    (*thisWindow->so[i]->DisplayObject)(thisWindow->so[i]);
  }
}
```

`(*obj->DisplayObject)(obj)` is `obj->DisplayObject()` in C++ terms — a virtual call plus
the hidden `this` argument made visible. The message loop dispatches the event methods the
same way:

```c
(*glbWindow->so[focus]->OnMouseDown)(glbWindow->so[focus]);
(*glbWindow->so[focus]->OnClick)    (glbWindow->so[focus]);
(*glbWindow->OnKeyPress)(glbWindow->so[focus], c);
```

Each subclass wires its own implementations at construction. A button is "constructed" as a
brace-initializer that fills the vtable slots with the button methods:

```c
PUSH_BUTTON_T billBtnList[8] = {
  419, 70, 89, 33,   // position  (the SCREEN_OBJECT_T base fields...)
  FALSE, PUSH_BUTTON, TRUE,
  BtnMouseDown,      // OnMouseDown   <- vtable
  BtnMouseUp,        // OnMouseUp
  BtnClick,          // OnClick
  BtnKeyPress,       // OnKeyPress
  PutButton,         // DisplayObject
  ...                // ...then the PUSH_BUTTON_T-specific fields
};
```

So `BtnClick` is `PushButton::OnClick`, `PutButton` is `PushButton::DisplayObject`, and the
static initializer is the constructor. Polymorphism falls out for free: the loop calls
`DisplayObject`, and a button paints a button while a scrollbar paints a scrollbar.

## RTTI in practice

The `type` tag is not decoration — the framework reads it to make decisions a pure-OO design
would handle with more virtuals, but which here are easier as a switch:

```c
// MLOOP.C — pick the cursor based on the object under the mouse
if ((so[item_index]->type != TEXT_BOX) && (so[item_index]->type != MEMO_BOX))
    which_mouse = M_FINGER;     // clickable thing -> pointing hand
else
    which_mouse = M_TEXTBAR;    // editable text   -> I-beam

// scrollbars and time-entries are excluded from normal focus capture
if ((so->type != SCROLL_BARH) && (so->type != SCROLL_BARV) && (so->type != TIME_ENTRY))
    ...
```

And it gates every downcast, so the cast stays type-safe:

```c
if (glbWindow->so[item_index]->type != DROP_DOWN_BOX) { ... }
else {
  ddb = (DROPDOWN_BOX_T*)(glbWindow->so[item_index]);
  if (ddb->dropped == FALSE) { ... }   // safe: we checked the tag first
}
```

## Windows are objects too

`LWINDOW_T` is itself a class with virtuals:

```c
typedef struct {
  COORDINATE_T     win_pos;
  SCREEN_OBJECT_T** so;        // its child objects
  void (_far* Draw)();         // virtual: custom static painting
  int   item_count;
  int   cursor_pos;
  FOCUS_T focus;
  UCHAR* title;
  void (_far* OnKeyPress)();   // virtual: keystroke handler
  int   default_item;
} LWINDOW_T;
```

`PutWindow()` paints the frame, loops the children's `DisplayObject`, then calls the window's
own `Draw()` for any bespoke decoration (`RateDraw`, `TriggerWindowDraw`, …).
`DefaultOnKeyPress()` is the base implementation most windows inherit; it forwards the key to
the focused child's `OnKeyPress`. A window that wants different behavior points `OnKeyPress`
somewhere else. That's method overriding.

## Why this is worth remembering

This is a complete, internally consistent object model — encapsulation, inheritance,
polymorphism, RTTI, virtual dispatch with an explicit `this` — implemented by someone working
in a language that offers none of it, on a compiler from 1991. Once the base class and the
dispatch convention exist, adding the ninth widget costs almost nothing: embed
`SCREEN_OBJECT_T so` first, write five methods, fill the slots. Every window, the message
loop, the modal dialogs, and the scrollbar-drives-listbox machinery then work on the new
widget without a single change. That's the payoff of getting the abstraction right, and they
got it right.

See also [[docs/notes/02-the-message-loop.md]] for how these objects are driven, and
[[docs/notes/05-the-widget-toolkit.md]] for the subclasses.
