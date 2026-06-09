---
title: More cool algorithms
summary: The gems beyond the headline features — nice-number axis scaling, an epoch-counting perpetual calendar, a dual-axis chart compositor, a greedy word-wrap memo editor, the X-10 house-code scramble, and a function-pointer "return address" pattern.
tags: [laneview, algorithms, graphics, text-wrap, note]
created: 2026-06-09
aliases: []
document.status: draft
---

# More Cool Algorithms

The billing interpreter, the OO toolkit, and the modal/screen‑save tricks get their own
notes. This one collects the smaller algorithmic gems scattered through the rest of the
code — the kind of thing you write once, get right, and reuse everywhere.

## 1. "Nice number" axis auto-scaling (`GRAPHS.C`)

Every chart needs a Y‑axis maximum that's a *round* number, so the gridlines read
cleanly (0, 5, 10, …) instead of (0, 4.7, 9.4, …). `FindMax()` snaps a measured peak up
to the next value on a 1 / 2.5 / 5 / 10 ladder:

```c
double FindMax(double peak) {
  double max;
  if (peak <= 0.05) max = 0.05;
  if (peak <= 0.10) max = 0.10;
  if (peak <= 0.25) max = 0.25;
  else if (peak <= 0.50)    max = 0.50;
  else if (peak <= 1.00)    max = 1.00;
  else if (peak <= 2.50)    max = 2.50;
  else if (peak <= 5.00)    max = 5.00;
  else if (peak <= 10.00)   max = 10.00;
  ... up to ...
  else                      max = 50000.00;
  return max;
}
```

It's the hand‑unrolled version of the "nice numbers for graph labels" algorithm every
plotting library implements. Because the ladder spans 0.05 kWh to 50,000 kWh, the same
chart code auto‑scales from a single light bulb to a whole substation. The axis labels
are then just `max`, `max*.8`, `max*.6`, `max*.4`, `max*.2`, `0` — five evenly‑spaced
round numbers, for free. A "Fix Vertical Axis" checkbox freezes the scale so you can
compare days without the bars rescaling under you.

## 2. An epoch-counting perpetual calendar (`calendar.c`)

`day_of_week()` computes the weekday of any date by **counting days from a known anchor**
— January 1, 1980, which was a Tuesday — and taking the result mod 7:

```c
int day_of_week(int month, int day, int year) {
  int i, leap;
  day += 1;                               // Jan 1, 1980 was a Tuesday -> bias the count
  for (i = 80; i < year; ++i)             // add up whole years since 1980
    day += ((i % 4) == 0) ? 366 : 365;
  leap = ((year % 4) == 0) ? 1 : 0;
  for (i = 1; i < month; ++i)             // add up whole months this year
    day += day_tab[leap][i];              // days-in-month table, leap-aware
  return (day % 7);                       // 0..6
}
```

No Zeller's congruence, no lookup of doomsday — just "how many days since the anchor,
modulo 7." It pairs with two small lookup tables that show up all over the codebase:

```c
USHORT days_tab[2][13] = {{0,31,59,90,120,151,181,212,243,273,304,334,365},   // cumulative, common
                          {0,31,60,91,121,152,182,213,244,274,305,335,366}};  // cumulative, leap
int    day_tab [2][13] = {{0,31,28,31,30,31,30,31,31,30,31,30,31},            // per-month, common
                          {0,31,29,31,30,31,30,31,31,30,31,30,31}};           // per-month, leap
```

`display_calendar()` then lays out the month grid with a tidy running `(weekday, week)`
cursor: place day `i` at column `weekday`, row `week`, advance `weekday`, and when it
passes Saturday reset to Sunday and bump `week`. It even cross‑references the
**dawn/dusk** table to print sunrise/sunset under the current month — because the
scheduler needs civil‑twilight times for dawn/dusk events (see
[06-energy-management-domain.md](06-energy-management-domain.md)).

The same two tables drive `next_day()` / `prev_day()` (`inutil.c`), which roll a
`(month, day, year)` triple forward or backward one day with correct month and
leap‑year carry — the iterator the bill calculator walks the history with.

## 3. A dual-axis chart compositor (`GRAPHS.C`, `DailyUsageOverLay`)

The daily‑usage graph is genuinely sophisticated for a hand‑rolled VGA routine. On one
set of axes it overlays **three different series with two different Y scales**:

- **Vertical bars** — hourly kWh (24 bars), scaled to the left axis (`dumax`).
- **A fine line graph** — the 96 fifteen‑minute interval kWh readings, same left axis.
- **A second line graph** — price (¢/kWh) per hour, scaled to a *separate right axis*
  (`dumax1`), so cost and consumption share the plot without sharing units.

The screen‑mapping is the classic "flip Y and scale into the plot box":

```c
row = 300 - (int)((work / dumax) * DOTS_HIGH);   // value -> pixel row (y grows downward)
if (row < TOP + 1) row = TOP + 1;                // clamp to the top of the plot
area_clear((int)row, TOP + DOTS_HIGH, col + 1, col + 19, color);   // draw the bar
```

The 15‑minute line is **color‑coded by load‑control state**, so you can see demand
response happening hour by hour:

```c
switch (load_ctrl[i]) {
  case 1: color = FG_LGRN; break;  // load-control CREDIT (you let the utility shed it)
  case 2: color = FG_RED;  break;  // load-control PENALTY (you overrode and kept drawing)
  case 3: color = FG_MAG;  break;  // load control active
  default:color = FG_BLU;  break;  // normal
}
```

The data behind it comes from the **c‑tree history file**, read as a *keyed sequential
scan*. Records are keyed by a fixed‑width string of device slot (hex) + date, and the
code seeds the key then walks forward until the key prefix stops matching:

```c
sprintf(found_key, "%04X%3d%2d%2d    ", device_id, do_year, do_mon, do_day);  // device+date key
strcpy(id_string, found_key);
if (next_history(found_key) == 0) {
  while (1) {
    if (memcmp(found_key, id_string, 11) != 0) break;        // left this device/day
    i = hist_tran.ht_hr * 4 + hist_tran.ht_qthr;             // hour*4 + quarter -> 0..95
    *(interval_usage + i)      += hist_tran.ht_kwh;          // bin the kWh
    *(interval_usage + i + 96) += hist_tran.ht_kwh_cost;     // bin the cost (parallel array)
    if (next_history(found_key)) break;
  }
}
```

That `hour*4 + quarter` is a clean flattening of a 2‑D (hour, quarter) index into a
96‑bucket day, and storing cost in the *same* buffer offset by 96 is a tidy
parallel‑array trick to keep kWh and dollars side by side without a second allocation.
The whole day's interval data lives in a reused scratch buffer `f_buffer` — there's no
heap churn per redraw.

## 4. A greedy word-wrap memo editor (`MemoBox.C`)

The message box wraps on an explicit `|`
([04-msgbox-and-screen-save.md](04-msgbox-and-screen-save.md)). The **memo box** is the
real thing: a multi‑line text editor that performs **automatic greedy word wrapping** and
keeps a live mapping between the flat character buffer and the wrapped on‑screen lines.

`FindAWord()` returns the length of the next "word" (run of non‑spaces plus the trailing
spaces that fit), and — cleverly — returns a **negative** length to signal it hit a line
break, folding the newline detection into the same return value:

```c
if (memo->text[current_byte] == LF) { ...; word_len++; word_len *= -1; }   // CR/LF -> negative
else if (memo->text[current_byte] == CR) { memo->text[current_byte] = LF; word_len++; word_len *= -1; }
else {
  while (text[cb] != ' ' && word_len < display_cols && text[cb] && text[cb]!=LF && text[cb]!=CR)
    { cb++; word_len++; }     // the word itself
  while (text[cb] == ' ' && word_len < display_cols && ...)
    { cb++; word_len++; }     // trailing spaces that belong with it
}
```

`MemoBoxPaint()` is the wrap engine: accumulate words into `text_line` until the next
word won't fit (`line_len + word_len - 1 >= display_cols`), emit the line, and start a
new one — standard greedy line breaking. As it goes it records, for every visual line,
where it started in the flat buffer and how long it is:

```c
memo->line_start[current_line + 1] = current_byte;   // where the next line begins in text[]
memo->line_length[current_line]    = line_len;       // how many chars are on this line
```

Those two arrays are the bridge between **(row, col)** cursor coordinates and a **flat
byte offset**, which is what makes editing work. Insert, delete, and backspace operate on
the flat buffer with `memmove`, fix up the affected `line_start[]` offsets, repaint
(which re‑wraps), and then convert the new byte position back to a cursor cell:

```c
// InsertChar(): make room, drop the char in, re-wrap, then map the offset back to (row,col)
memmove(&memo->text[position + 1], &memo->text[position], memo->max_length - (line_start + col + 1));
memo->text[line_start + col] = c;
MemoBoxPaint(memo);                 // re-wrap everything
RepositionCursor(position + 1, memo);
```

`RepositionCursor()` is the inverse map — given a flat offset, find the line whose
`[line_start, line_start+line_length]` span contains it and set `(cursor_row,
cursor_col)`. Arrow keys, Home/End, PgUp/PgDn, click‑to‑position, a scrolling viewport
(`display_top`), insert/overwrite, and a blinking caret all fall out of keeping that
two‑way mapping honest. It's a small word processor, and it scrolls a document taller
than the box via `display_top` + `MemoRefreshAll()`/repaint. (This is the
`MLOOP.C` rev 1.16 reason the scrollbar learned to *attach* to another control — so it
could drive this memo box; see [05-the-widget-toolkit.md](05-the-widget-toolkit.md).)

## 5. The X-10 house-code scramble (`inutil.c`)

X‑10 house letters A–P don't map to 0–15 in order — the powerline chips use a specific
non‑sequential 4‑bit encoding. LaneView just keeps the hardware's table verbatim:

```c
char house_code[16] = {
  0x06,/*A*/ 0x0E,/*B*/ 0x02,/*C*/ 0x0A,/*D*/ 0x01,/*E*/ 0x09,/*F*/ 0x05,/*G*/ 0x0D,/*H*/
  0x07,/*I*/ 0x0F,/*J*/ 0x03,/*K*/ 0x0B,/*L*/ 0x00,/*M*/ 0x08,/*N*/ 0x04,/*O*/ 0x0C,/*P*/
};
```

Look closely and it's almost a **Gray code** — adjacent letters differ in few bits — which
is exactly the kind of encoding powerline signaling uses for noise immunity. A reminder
that a lot of "magic tables" in embedded code are magic because the *hardware* said so,
and the right move is to copy the spec exactly and comment it, which is what they did.

## 6. Fixed-point money, everywhere

There's a deliberate, consistent fixed‑point scheme for currency and energy so the
on‑disk records never store a `double`:

- **Energy**: kWh × 10,000 (`/ 10000.0` on the way out).
- **Money**: dollars × 100,000 (`/ 100000.0` on the way out) — hundred‑thousandths, fine
  enough for sub‑cent per‑kWh rates.

A small `round(value, digits)` helper does explicit decimal rounding before display so
totals foot correctly. Integer storage with a fixed scale is the standard embedded
answer to "floats are slow and don't serialize portably," and it's applied uniformly
across the meter, history, and billing structures.

## 7. Function pointers as "return addresses" (`Sender`)

Several screens are reusable sub‑menus that can be reached from more than one place —
"View Rates," the calendar, the trigger editor. How does the Return button know where to
go *back* to? A global function pointer acts as a **continuation / return address**: the
caller stores itself in `Sender` before transferring control, and the reusable screen's
Return handler simply calls it.

```c
// caller (BILLING.C): remember "me" before jumping to the shared rates screen
Sender = BM_addr;          // BM_addr == &BillingMenu
ShowDaysRates();

// reusable screen (trigger.c): "go back to whoever sent me here"
static void ReturnClick(void) { (*Sender)(); }

// and the label adapts to the caller, too:
if (Sender == IntroScreen) trigger_menu_btn[3].text = "Main|Menu";
else                       trigger_menu_btn[3].text = "Mood|Setup";
```

It's a one‑slot call stack made of a function pointer — the same idea as a return address
or a continuation, hand‑built because there's no framework to manage navigation. The
return button even relabels itself based on who the `Sender` was, so the shared screen
feels native to whichever menu summoned it.

---

These aren't the flashiest parts of LaneView, but they're the parts a working engineer
recognizes: the small, correct, reusable kernels — axis scaling, calendar math, a
flatten‑and‑bin data reducer, a two‑way text/line mapping, a hardware table copied
faithfully, fixed‑point money, and a pointer used as a return address — that everything
else is quietly built on.
