---
title: Mouse cursors and easter eggs
summary: Hand-authored 16x16 AND/XOR cursor bitmaps for INT 33h, a mood-face-by-quadrant easter egg on the intro screen, and the funniest comments in the codebase.
tags: [laneview, mouse, vga, easter-eggs, note]
created: 2026-06-08
aliases: []
document.status: draft
---

# Mouse cursors and easter eggs (`MOUSE.C`)

The smaller delights. None of this was strictly necessary; all of it shows someone enjoying
the work.

## Cursors are hand-authored bitmaps

The DOS mouse driver (INT 33h) renders a cursor from two 16×16 bitmaps: an AND mask (what to
clear) and an XOR mask (what to flip). LaneView defines four cursors as raw hex, one 16-bit
word per row, by hand. `CURSOR_DEF_T` starts with the hotspot, then the two planes. The arrow:

```c
CURSOR_DEF_T arrow_ms = {
  1, 1,                 // hotspot x, y
  0x9FFF, 0x8FFF, 0x87FF, 0x83FF, 0x81FF, 0x80FF, 0x807F, 0x803F,   // AND mask
  0x801F, 0x800F, 0x80FF, 0x887F, 0x987F, 0xBC3F, 0xFC3F, 0xFE3F,
  0x0000, 0x2000, 0x3000, 0x3800, 0x3C00, 0x3E00, 0x3F00, 0x3F80,   // XOR mask
  0x3FC0, 0x3E00, 0x3600, 0x2300, 0x0300, 0x0180, 0x0180, 0x0000
};
```

Squint at the XOR mask in binary and the arrow shape is right there in the bit pattern. There
are four of them, each drawn pixel by pixel in hex:

- `arrow_ms` — the default pointer.
- `finger_ms` — a pointing hand, shown over clickable objects.
- `bar_ms` — a text I-beam, shown over editable text.
- `hourglass_ms` — the busy cursor, set by `Busy()` and cleared by `NotBusy()`.

The cursor in use is tracked in `which_mouse` and swapped only on change (see
[[docs/notes/02-the-message-loop.md]]), so the pointer changes shape as it moves between
buttons and text fields, exactly like a modern GUI — built from four hand-typed bitmaps and a
state variable.

## The mood-face easter egg

On the intro screen, when the system is idle, `show_mouse()` displays one of four "mood" face
icons depending on which quadrant of the screen the mouse is in. Move the pointer around the
intro screen and the face changes:

```c
void show_mouse(void) {
  if (ticks_check(7) == FALSE || glbWindow != IntroWindow || mouse_info.buttons != 0)
    return;                                   // only on the intro screen, only when idle
  ticks_set(7, 8L);
  if      (mouse_info.y < 206 && mouse_info.x > 379) { LoadIcon(icon, "mood2.ico"); DisplayIcon(...); }
  else if (mouse_info.y < 206 && mouse_info.x < 379) { LoadIcon(icon, "mood1.ico"); DisplayIcon(...); }
  else if (mouse_info.y > 206 && mouse_info.x > 379) { LoadIcon(icon, "mood3.ico"); DisplayIcon(...); }
  else if (mouse_info.y > 206 && mouse_info.x < 379) { LoadIcon(icon, "mood4.ico"); DisplayIcon(...); }
}
```

The function the loop calls every iteration is commented `//do the smiley eye balls =)`. It's
gated to fire only on the intro screen, only when no button is pressed, and only every few
ticks — a cosmetic flourish someone had fun building.

Tucked right below it, under `#if 0`, is the debug HUD it replaced: a live readout of the
mouse's buttons, X, Y, row, and column, repositionable with the right mouse button — the
developer's own instrumentation, left in the source as a fossil.

## The comments

Half the personality of this codebase is in the margins. A sampler:

- `SCRNOBJT.H`, on discovering function pointers are callable:
  > `void (_far* OnClick)();  //(* pbf)(); <- thats a call!!!`

- `MOUSE.C`:
  > `show_mouse(); //do the smiley eye balls =)`

- `BILLING.C`, on the off-by-one month indexing, with a developer's weary confidence:
  > ```
  > // Please note: This month's est. bill is in bill_month_index == 0
  > //            LAST month's est. bill is in bill_month_index == 1
  > //            ... To get to the next month, SUBTRACT one ... Trust me.
  > ```

- `MLOOP.C`, a debug breadcrumb left in a scrollbar branch:
  > `//Status("Kilroy was here");`

- `viewmail.c`, on why mail is capped at 20 items — a real 16-bit memory-model constraint,
  explained plainly:
  > `// cannot access more than 31 mails without supporting a __huge array and __huge`
  > `// pointers which is not compatible with current listbox...`

- And a `static int my_hokey_flag;` in `BILLING.C`, named with full honesty.

There's also a commented-out, half-built `WEATHER` struct in `2box.h` (sunrise/sunset, temp,
wind, pollen, pollution, a UV "tanning_idx") and an `EMAIL` struct — ambitions for the platform
the schedule never reached. They're a window into where the product was headed:
weather-aware scheduling and in-home messaging, sketched in a header and left for later.

## The takeaway

The cursors, the quadrant faces, the left-in debug HUD, and the running commentary are what
make this read like software written by a person, not a process. Thirty years on, they're also
what make it fun to revisit — the engineering impresses, but the fingerprints are what stick.
