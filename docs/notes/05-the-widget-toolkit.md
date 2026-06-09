---
title: The widget toolkit
summary: Hand-drawn 3-D bevel buttons, a scrollbar with thumb math and an attach-to-another-control trick, and a full in-place text editor with per-type input filtering.
tags: [laneview, ui, widgets, vga, note]
created: 2026-06-08
aliases: []
document.status: draft
---

# The widget toolkit

Every control is a `SCREEN_OBJECT_T` subclass (see
[[docs/notes/01-object-oriented-c.md]]). The toolkit ships push buttons, text boxes, list
boxes, check boxes, drop-downs, spinners, time entries, memo boxes, and scrollbars — all
painted by hand into VGA memory with two primitives, `hline()` and `vline()`, plus
`area_clear()` and `ptext()`. No GDI, no widget library. Here are the three most interesting.

## Push buttons: hand-drawn 3-D bevels

There's no "draw a 3-D button" call in DOS. The Windows-95 raised/sunken look is constructed
one edge at a time, with light on the top-left and shadow on the bottom-right — inverted when
pressed. From `pbutton_body()` in `PSHBTN.C`:

```c
if (type == PB_UP) {                    // raised
  hline(top,            left,   left+width, FG_WHT);    // top    highlight
  hline(top+1,          left+1, left+width, FG_HWHT);   // top    inner highlight
  hline(top+height-1,   left,   left+width, FG_GRY);    // bottom shadow
  hline(top+height,     left,   left+width, FG_BLK);    // bottom outer shadow
  vline(left,           top,    top+height-1, FG_WHT);  // left   highlight
  vline(left+width-1,   top+1,  top+height-1, FG_GRY);  // right  shadow
  vline(left+width,     top,    top+height,   FG_BLK);  // right  outer shadow
} else {                                /* PB_DN: swap highlight<->shadow = sunken */ }
```

Four highlight strokes, four shadow strokes, and the eye reads a bevel. `PutButton()` adds
three states — `up`, `down` (pressed: shifts the label down-right by 2 px for a tactile
"give"), and `flat` (a hover/flat look) — plus a focus rectangle in a contrasting color, an
optional 24×24 icon, and the `|`-delimited multi-line label centering borrowed from the
message box.

Buttons come in two behaviors via `btn_type`:

- `push` — momentary. Down on press, up plus action on release.
- `set` — a radio/toggle. Clicking one `set` button in a window pops all the other `set`
  buttons in that window back up, so a group acts as mutually exclusive options:

```c
// BtnClick(), set behavior
for (i = 0; i < glbWindow->item_count; i++) {
  if (glbWindow->so[i]->type == PUSH_BUTTON) {
    pb_last = (PUSH_BUTTON_T*)glbWindow->so[i];
    if (pb_last->btn_type == set && pb_last->state == down && glbWindow->focus.current != i) {
      pb_last->state = up; PutButton(pb_last);     // release the previously-selected one
    }
  }
}
```

The Load-Management device selector (`WHMENU.C`) uses a column of `set` buttons exactly this
way — pick a device, the others deselect.

## Scrollbars: thumb math and the "attach" trick

`DisplayScrollBar()` (`scrolbar.c`) positions the thumb by mapping `thumb_pos` over the
`[min,max]` range onto the track length, minus the room taken by the end buttons:

```c
// vertical:
sb_cursor_pos = sb->so.position.top
              + (int)((sb->so.position.height - 37) *
                      (sb->thumb_pos / (double)(sb->max - sb->min)))
              + 14;
```

Clicks are classified by where they land (`ScrollBarOnClick`): the end buttons emit a line
step (`small_chg`), and clicking the track above or below the thumb emits a page step
(`large_chg`) in that direction. Every gesture reports through one callback the owner supplies:

```c
void (_far* OnScroll)();          // (direction, how_far, thumb_pos)
(*sb->OnScroll)(SCROLL_DOWN, SCROLL_LARGE, sb->thumb_pos);
```

The owner's handler typically drives a list box and then re-syncs the bar — for example
`PageDown(&list_box); ListBoxScroll(&list_box);`.

The clever bit is `int attached_object;` (`scrolbar.h`). A scrollbar can bind to another
control so that releasing the bar hands focus back to the thing it scrolls. The message loop
special-cases it — this is the feature called out in the `MLOOP.C` rev 1.16 changelog,
"Mark's changes to support attaching a scroll bar to other items. i.e, Memoboxes &
Listboxes":

```c
// MLOOP.C, on scrollbar mouse-up
sbptr = (SCROLL_BAR_T*)glbWindow->so[glbWindow->focus.current];
if (so[focus]->type == SCROLL_BARV && sbptr->attached_object != 0) {
  ... repaint the bar without focus ...
  glbWindow->focus.current = sbptr->attached_object - 1;   // give focus to the listbox/memobox
  if (so[focus]->got_focus != TRUE) {
    so[focus]->got_focus = TRUE;
    (*so[focus]->DisplayObject)(so[focus]);
  }
}
```

So a list box and its scrollbar feel like one compound control even though they're two
independent `SCREEN_OBJECT_T`s — the toolkit's composition story, told without a container
widget.

## Text boxes: a real in-place line editor

`TxtKeyPress()` in `TEXTBOX.C` is a complete single-line text editor. It supports:

- Caret movement — Home, End, Left, Right arrows (`glbWindow->cursor_pos`).
- Insert vs. overwrite — the Insert key toggles `over_write` and updates an `INS`/`OVR`
  indicator on screen; insert mode splices the new char into the middle of the string,
  overwrite mode replaces.
- Delete and Backspace — both handle mid-string edits by rebuilding the string around the
  cursor with `strncpy`/`strcat` into scratch buffers.
- A live `OnChange` callback — fired on every edit, so dependent UI updates as you type.

```c
case 82: // Insert key: toggle insert/overwrite + on-screen INS/OVR badge
  if (over_write == 0) { over_write = 1; ptext("OVR", 460, 605, BG_WHT + FG_BLU); }
  else                 { over_write = 0; ptext("INS", 460, 605, BG_WHT + FG_BLU); }
  return;
case 83: // Delete: stitch the string back together around the caret
  strncpy(KeyPressBuf[0], tb->text, cursor - col + 1);
  strcpy (KeyPressBuf[1], &tb->text[cursor - col + 2]);
  strcat (KeyPressBuf[0], KeyPressBuf[1]);
  strcpy (tb->text, KeyPressBuf[0]);
  if (tb->OnChange) (*tb->OnChange)();
  PutTxtBox(tb);
  return;
```

The caret itself is drawn by `BlinkCursor()` (`MLOOP.C`) as a blinking `vline` at the column
position, on the same tick timer as everything else.

### Per-type input filtering

A text box carries a `type`, and the editor rejects keystrokes that don't belong to that type
before they're ever inserted — so an integer field holds no letters and a hex field
auto-uppercases:

```c
switch (tb->type) {
  case tb_int:   if ((c < '0' || c > '9') && c != '-' && c != '+') return; break;
  case tb_float: if ((c < '0' || c > '9') && c != '.') return; break;
  case tb_hex:   if (c > 0x60 && c < 0x67) c -= 0x20;            // a-f -> A-F
                 if ((c < '0' || c > '9') && (c < 'A' || c > 'F')) return; break;
  case tb_date:  /* digits, '/', '-' */ ...
  case tb_time:  /* digits, ':', a/p/A/P */ ...
}
```

For dates and times a companion `IsValid()` parses the field and range-checks it (months 1–12,
leap-year-aware day counts via `day_tab[2][13]`, hours 1–12, minutes 0–59) before the value is
accepted. The Load-Management override screen (`WHMENU.C`) uses date/time boxes with exactly
this validation, and even snaps minutes to `00/15/30/45` quarter-hour boundaries to match the
metering interval.

## The rest of the zoo

The same pattern — embed `SCREEN_OBJECT_T so` first, supply five methods, fill the
brace-initializer — gives the remaining controls cheaply:

- `SPINNER_T` — a value with `min`/`max`/`increment` and up/down arrows; used for the bill
  statement day and trigger durations.
- `DROPDOWN_BOX_T` — a combo that freezes the loop's `item_index` rotation while `dropped`, so
  it keeps focus.
- `LISTBOX_T` — holds a `sub_list` of arbitrary items plus a `DisplayItem` function pointer, so
  it renders anything polymorphically (dates, moods, triggers, schedule modes) without knowing
  the element type.
- `CHECK_BOX_T`, `TIME_ENTRY_T`, `MEMO_BOX_T` — round out the set.

The list box earns a closing nod: by storing a `void(*DisplayItem)(item, top, left, color)` it
stays fully generic. `trigger.c` points it alternately at `DisplayMood` and `DisplayMode`;
`BILLING.C` points it at `DisplayDateEntry`. One list box, many element types — polymorphism
again, all the way down.
