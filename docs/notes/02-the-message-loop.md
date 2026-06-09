---
title: The message loop
summary: LaneView's single-threaded cooperative event loop — round-robin hit-testing, a cursor state machine, three-way keystroke routing, and built-in click telemetry.
tags: [laneview, event-loop, ui, dos, note]
created: 2026-06-08
aliases: []
document.status: draft
---

# The message loop (`MLOOP.C`)

There's no operating system here to deliver events. No `GetMessage`, no interrupt-driven
queue, no scheduler. `MainLoop()` is a single `while (1)` that *is* the application — it
polls the mouse, polls the keyboard, polls the home-automation bus, updates the clock,
blinks the caret, and dispatches every UI event, forever, cooperatively.

What makes it interesting is how much structure rides in that one loop.

## Round-robin hit-testing

The naive way to find "what is the mouse over?" scans all objects every frame. LaneView
doesn't. It keeps a rotating cursor `item_index` and advances it one object per loop
iteration:

```c
if (glbWindow->so[item_index]->type != DROP_DOWN_BOX) {
  if (++item_index > glbWindow->item_count - 1)
    item_index = 0;
}
```

Each pass tests the mouse against one object — the current `item_index` — and against the
focused object. Because the loop runs thousands of times a second and windows hold a handful
of objects, every object gets hit-tested many times a second anyway, while each individual
iteration stays cheap and constant-time. It amortizes hit-testing across frames without a
spatial index, and it falls naturally out of the "do a little work each tick" philosophy.
Dropdown boxes pause the rotation while open, so the expanded list keeps focus.

`MouseOver()` (`MOUSE.C`) is the hit test — an AABB check against the object's
`COORDINATE_T`:

```c
int MouseOver(COORDINATE_T* pos) {
  return (mouse_info.x > pos->left)  && (mouse_info.x < pos->left + pos->width)
      && (mouse_info.y > pos->top)   && (mouse_info.y < pos->top  + pos->height);
}
```

## A cursor state machine

The pointer shows intent: an arrow over chrome, a pointing finger over anything clickable, an
I-beam over editable text. The loop tracks the current shape in `which_mouse` and reloads the
cursor bitmap only on a change, so the cursor never flickers from redundant updates:

```c
if (over a non-text object) {
  if (which_mouse != M_FINGER) { which_mouse = M_FINGER; mouse_cursor(&finger_ms); }
} else {                                  // over text
  if (which_mouse != M_TEXTBAR){ which_mouse = M_TEXTBAR; mouse_cursor(&bar_ms); }
}
...
if (nothing under cursor && which_mouse != M_ARROW) { ...; mouse_cursor(&arrow_ms); }
```

The `last_index` / `do_it` dance skips recomputing the cursor when the mouse hasn't left the
object it was already over. Those cursors are hand-drawn bitmaps — see
[[docs/notes/07-mouse-cursors-and-easter-eggs.md]].

## Press / drag-off / release — a real button model

The loop implements proper push-button semantics, the kind where dragging off a pressed
button cancels it:

- Button down over an object → set `mouse_down`, give it focus, call its `OnMouseDown` (the
  button visually depresses).
- Still held, but moved off → call `OnMouseUp` (the button pops back up) and clear
  `mouse_down`. The click cancels.
- Released while still over the object → call `OnClick` (the action fires).

```c
if (mouse_info.buttons == 0) {           // released
  if (mouse_down != -1) {
    mouse_down = -1;
    if (MouseOver(&so[focus]->position) && so[focus]->OnClick != NULL)
      (*so[focus]->OnClick)(so[focus]);  // fire only if released ON the object
  }
}
```

Scrollbars and time-entries take a different path (they act on press-and-hold with a repeat
timer gated by `ticks_check(9)`), which is why they're singled out by `type` throughout the
loop.

## Three-way keystroke routing

Key handling is layered, and the loop's own comment spells it out:

> *"system" keystrokes are handled here … "window" keystrokes are passed to the current
> window's OnKeyPress() … "object" keystrokes are passed by the window's OnKeyPress() to the
> object's OnKeyPress().*

```c
switch (c) {
  case 9:  TabRight(); break;                 // SYSTEM: Tab
  case 27: if (glbWindow == IntroWindow) GetPassword(); break;  // SYSTEM: Esc
  case 0:                                      // extended-key prefix
    c = kbchar();
    switch (c) {
      case 68: lv_stop();                      // SYSTEM: F10 quit
      case 15: TabLeft(); break;               // SYSTEM: Shift-Tab
      case 59: /* F1: Windows Help */ ...
      default: unkbchar(c);                    // put it back, hand to window
               (*glbWindow->OnKeyPress)(so[focus], 0);
    }
    break;
  default:                                     // WINDOW: everything else
    (*glbWindow->OnKeyPress)(so[focus], c);
}
```

Tab traversal (`TabRight` / `TabLeft`) walks the object array, skips invisible objects, and
wraps around, then moves focus and repaints both the old and new object so the focus
rectangle follows. Extended keys arrive as a `0` byte followed by a scan code — the classic
IBM PC BIOS keyboard convention — and `unkbchar()` pushes the byte back so the window handler
can re-read it.

## A DOS app that cooperates with Windows

The F1 help handler is a small marvel of pragmatism. It first asks "am I running inside
Windows?" via the standard multiplex interrupt:

```c
int IsWindowsRunning(void) {
  union REGS regs;
  regs.x.ax = 0x1600;          // INT 2Fh, AX=1600h: "Windows enhanced-mode install check"
  int86(0x2F, &regs, &regs);
  return (regs.x.ax == 0x1600) ? FALSE : TRUE;
}
```

If Windows is up, it reads a help-file name from `lvhlp.dat` and shells out to `winhelp` to
show a real Windows help window — a DOS program reaching up into the GUI it runs under. If
Windows is absent, it degrades to a message box.

## Built-in telemetry, in 1996

Every keystroke and every button click appends to `user.log`, timestamped, with the active
window's title:

```c
// KeyLog() in MLOOP.C, BtnLog() in PSHBTN.C
fprintf(fbtnlog, "%02d/%02d/%02d\t%02d:%02d:%02d\tBTN: %d\t\t%s\tWINDOW: %s\n",
        this_month, today, this_year, tod->tm_hour, tod->tm_min, tod->tm_sec,
        pb->btnlog, pb->text, glbWindow->title);
```

Each button even carries a stable numeric ID (`btnlog`) in its struct, precisely so the logs
can be analyzed independent of label text. This is product analytics — session-level usage
tracking — hand-rolled into a DOS binary years before the idea was common.

## Time, blink, and the bus

Three more things happen every iteration, interleaved with everything above:

```c
if (ticks_check(BlinkTimer)) BlinkCursor();   // caret blink (~1.5s) via a tick timer
show_mouse();                                 // the intro-screen "mood" easter egg
CheckCEBus();                                 // poll the home-automation bus
time(&ltime); tod = localtime(&ltime);        // keep the on-screen clock live
if (last_shown != ltime) UpdateTop(&last_shown);
```

`BlinkCursor()` draws and erases the text caret as a vertical line (`vline`) on a timer, and
special-cases the memo box. `CheckCEBus()` is the heartbeat that lets the house talk back —
incoming device states, load-control signals — while the user works.

## The pattern that repeats

This same loop body — mouse poll, focus/press/release dispatch, clock, CEBus, three-way key
routing — is copied wholesale into `msgbox()` and `inputbox()` so that modal dialogs stay
fully live (see [[docs/notes/04-msgbox-and-screen-save.md]]). There's no event abstraction
shared between them; the loop *is* the abstraction, duplicated where a modal context needs
its own copy. Crude by modern taste, but it means a message box popped from deep inside a
handler still blinks the caret, updates the clock, and services the automation bus while it
waits for you to click OK.
