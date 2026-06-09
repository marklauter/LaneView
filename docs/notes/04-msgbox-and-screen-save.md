---
title: The message box, text wrap, and screen save/restore
summary: msgbox() wraps on a '|' delimiter with vertical+horizontal centering, saves the pixels behind it with a far-pointer screen grab, and runs a fully reentrant modal event loop.
tags: [laneview, ui, msgbox, vga, far-pointers, note]
created: 2026-06-08
aliases: []
document.status: draft
---

# The Message Box: Text Wrap + Screen Save/Restore (`msgbox.c`)

`msgbox()` looks, from the call site, exactly like the Windows API it imitates:

```c
if (msgbox("Are you sure?", "Caution", MB_YES_NO) == MB_RESULT_YES) { ... }
msgbox("History file not opened.", "Warning", MB_OK);
msgbox("MS Windows must be running|to support this function.", "Help Error", MB_OK);
```

Three small features make it punch above its weight: a tidy **text‑wrap convention**,
**pixel‑perfect screen save/restore**, and a **fully reentrant modal loop**.

## Text wrap on the pipe character

There's no word‑wrap engine. Instead the caller marks line breaks with a vertical bar
`|` (ASCII 124), and `msgbox` splits the string into up to six lines:

```c
UCHAR buf[6][80];
for (i = 0; i < (signed)strlen(text); i++) {
  if (text[i] == 124) {            // '|'  -> start a new line
    strcat(buf[j], "\0");
    j++;
  } else {
    strncat(buf[j], &text[i], 1);  // accumulate this char into the current line
  }
}
```

So `"Unable to access ICU|Unable to clear Override"` becomes two centered lines. It's a
deliberately minimal convention that puts layout control in the caller's hands — and it
reads naturally in the source, where you can *see* the line breaks in the literal.

## Centering, computed from the font metrics

The font is a fixed 8×14 cell, so geometry is pure arithmetic. Vertical centering uses
the line count; each line is horizontally centered on its own length:

```c
v_center = (message_box->win_pos.height - ((j + 1) * 14)) / 2;   // 14 px per line

for (i = 0; i < j + 1; i++) {
  ptext(buf[i],
        message_box->win_pos.top  + v_center + (i * 14),
        message_box->win_pos.left + (message_box->win_pos.width
                                     - (strlen(buf[i]) * 8)) / 2,    // 8 px per char
        BG_TRANSPARENT + FG_BLK);
}
```

The *same* `|`‑split‑and‑center routine* is reused for multi‑line **button labels**
(`PutButton` in `PSHBTN.C` splits on `|` into `buf[3][25]`), which is why labels like
`"Load|Management|Menu"` and `"Main|Menu"` render as stacked, centered text. One idea,
two places.

## Screen save/restore with a far-pointer pixel grab

A modal dialog has to appear *over* whatever was on screen, then vanish without leaving a
hole — and there's no compositor, no z‑order, no backing store from the OS. LaneView does
it the direct way: **copy the VGA pixels behind the dialog into a buffer, then copy them
back on dismiss.**

```c
UCHAR far* saved_window = NULL;
...
tmpWindow    = glbWindow;             // remember the window we're covering
glbWindow    = message_box;
saved_window = get_window(9, 19, 44, 17);   // grab the rectangle of pixels behind us
PutWindow(glbWindow);                 // draw the dialog on top
...
// on dismiss:
glbWindow = tmpWindow;
put_window(saved_window);             // blit the original pixels back -> hole healed
```

`get_window()` returns a `UCHAR far*` — a `malloc`'d **far** buffer holding the captured
rectangle (far because in the large memory model a saved screen region can live anywhere
in the segmented 16‑bit address space). `put_window()` restores it. No repaint of the
underlying window is needed; the bytes that were there are simply put back. This is the
fast, correct, old‑school way to do overlapping UI, and it's why a message box can pop up
in front of the billing screen and leave it perfectly intact.

## A fully reentrant modal loop

The most important property: `msgbox()` does **not** return to `MainLoop` and wait for a
callback. It runs **its own complete copy of the event loop** until a button sets
`msg_result`:

```c
msg_result = 0;
while (1) {
  mouse_data(&mouse_info);
  /* ... the full cursor state machine ... */
  if (msg_result != 0) {                 // a button handler set this
    glbWindow = tmpWindow;
    put_window(saved_window);            // restore screen
    return msg_result;                   // <-- hand the answer back to the caller
  }
  /* ... press/drag/release dispatch ... */
  time(&ltime); tod = localtime(&ltime);
  if (LOGGED_IN) CheckCEBus();           // the house keeps talking while you decide
  if (MainLoopOpt && last_shown != ltime) UpdateTop(&last_shown);
  /* ... three-way keystroke routing, Tab/Shift-Tab/F10 ... */
  if (++item_index > glbWindow->item_count - 1) item_index = 0;
}
```

The button handlers are trivial — they just write the result the loop is watching for:

```c
void MsgBoxYes(void) { msg_result = MB_RESULT_YES; }
void MsgBoxNo (void) { msg_result = MB_RESULT_NO;  }
```

Because the modal loop is self‑contained and re‑entrant, you can call `msgbox()` from
*inside any event handler* — even one that was itself reached from another `msgbox` —
and the whole world keeps ticking underneath it: the caret blinks, the clock advances,
and `CheckCEBus()` still services load‑control signals from the utility. The dialog is
modal to the *user* but not to the *system*. That's a genuinely nice property to get out
of a single‑threaded DOS program, and it comes entirely from the loop being a value‑
returning function rather than a state machine.

## `inputbox()`: the same trick, plus a password mode

`inputbox()` is `msgbox()` with a `TEXT_BOX` added to the object list and a caret timer.
It returns a `UCHAR*` to the entered text (empty on Cancel). A neat touch: passing `"*"`
as the default text switches the box into password mode, which renders asterisks instead
of characters:

```c
if (strcmp(dflt_text, "*") == 0) { msgbox_txt.type = tb_pass_word; sprintf(dflt_text, ""); }
...
// in PutTxtBox(): password fields draw '*' for each character
if (tb->type == tb_pass_word && strlen(tb->text)) memset(PutTxtBuf, '*', strlen(tb->text));
```

Same screen‑save, same reentrant loop, same centering — composed from the parts already
built. (The text editing itself is covered in
[05-the-widget-toolkit.md](05-the-widget-toolkit.md).)
