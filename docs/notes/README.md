---
title: LaneView field notes — index
summary: Tour of the genuinely cool engineering in LaneView, TeCom's 1996–98 DOS residential energy-management viewer.
tags: [laneview, legacy, dos, overview, note]
created: 2026-06-08
aliases: []
document.status: draft
edge.indexes: [docs/notes/01-object-oriented-c.md, docs/notes/02-the-message-loop.md, docs/notes/03-the-billing-language.md, docs/notes/04-msgbox-and-screen-save.md, docs/notes/05-the-widget-toolkit.md, docs/notes/06-energy-management-domain.md, docs/notes/07-mouse-cursors-and-easter-eggs.md, docs/notes/08-more-cool-algorithms.md, docs/notes/billing-language-reference.md]
---

# LaneView — field notes on a remarkable little DOS program

> *"It was just a text editor with a green build button. lol."*

These notes tour the cool engineering buried in LaneView, the DOS viewer for TeCom Inc.'s
"Home Manager" residential energy-management system, written ~1996–1998 in 16-bit
Microsoft C (the VC++ 1.x era — large memory model, 8086 codegen, DOS overlays).

It's an ambitious piece of software for its time and toolchain: a hand-rolled,
mouse-driven, object-oriented GUI toolkit, an interpreted billing language, real-time
time-of-use and demand-response energy billing, and a home-automation rules engine — all
in a single-threaded DOS binary that paints VGA pixels by hand and talks to a
power-line/CEBus control bus. This folder documents the parts worth remembering.

## What the program is

A homeowner sat in front of a PC running LaneView. The PC spoke over CEBus and X-10 (see
`cebus.h`, `CheckCEBus()`) to an ICU — Intelligent Control Unit — that switched loads,
read submeters, and received load-control (demand-response) signals from the electric
utility. LaneView was the human face of all that:

- See your energy use hour-by-hour and what it costs under flat, time-of-use, and
  real-time pricing (`WHMENU.C` → Rate Comparison).
- Estimate your bill for any period, formatted by a utility-customizable billing program
  (`BILLING.C` plus `billing.prg`).
- Schedule devices with events that respond to season, occupancy, HVAC mode, and the
  current price of electricity (`event.h`, `EVENT` in `2box.h`).
- Automate the house: map an X-10 remote button to a "mood" (scene) or mode change
  (`trigger.c`).
- Manage load control: view utility curtailment events, override them, and see the
  credits earned or penalties incurred (`WHMENU.C` load-management section).

Copyright headers name TeCom Inc.; the city-tax table names Polk City and Oldsmar (both in
Florida), and the VCS user `tmges` plus author initials `MSL` (Mark Lauter) and `GES`
(Gary Speegle) date it to a late-'90s Florida utility pilot.

## The notes

- [01 — Object-oriented programming in plain C](01-object-oriented-c.md): a working
  class/inheritance/virtual-dispatch system built from structs and function pointers.
  Struct-first-member inheritance, function-pointer vtables, RTTI tags, upcasting and
  downcasting.
- [02 — The message loop](02-the-message-loop.md): a cooperative, single-threaded event
  loop that round-robins hit-testing, runs a cursor state machine, routes keystrokes three
  ways, shells out to Windows Help, and logs every click.
- [03 — The billing language](03-the-billing-language.md): the star. A complete
  interpreted DSL (`billing.prg`) with variables, arrays, `IF/ELSE/ENDIF`, and a recursive
  string-rewriting expression evaluator. The C engine computes the numbers and injects them
  into the interpreter's namespace; the script typesets the bill.
- [04 — The message box, text wrap, and screen save/restore](04-msgbox-and-screen-save.md):
  the `|`-delimited text wrap with vertical and horizontal centering, pixel-accurate screen
  save/restore via far pointers, and a reentrant modal loop you can summon from any handler.
- [05 — The widget toolkit](05-the-widget-toolkit.md): hand-drawn Win95-style 3-D bevel
  buttons, a scrollbar with thumb math and an attach-to-another-control trick, and a full
  in-place text editor.
- [06 — The energy-management domain](06-energy-management-domain.md): price-aware
  scheduling, nibble-packed 16-level rate signals, demand-response credits and penalties,
  moods/scenes, and X-10 trigger automation. IFTTT, in 1996.
- [07 — Mouse cursors and easter eggs](07-mouse-cursors-and-easter-eggs.md): hand-authored
  AND/XOR cursor bitmaps, a mood-face-by-quadrant easter egg, and the funniest comments in
  the codebase.
- [08 — More cool algorithms](08-more-cool-algorithms.md): nice-number axis scaling, an
  epoch-counting perpetual calendar, a dual-axis chart compositor, a greedy word-wrap memo
  editor, the X-10 house-code scramble, fixed-point money, and function pointers used as
  return addresses.
- [The billing language — grammar reference](billing-language-reference.md): the formal spec
  for `billing.prg` — lexical rules, an EBNF grammar, the statement and expression reference,
  the host-provided variable namespace, limits, and parser gotchas. Companion to note 03.

## Reading this code in 2026

The formatting was normalized (LF, spaces, clang-format) on the `lauterm/cleanup` branch,
but the logic is exactly as shipped. The `$Log$` and `$Workfile$` headers are original RCS
keywords. The hardcoded `J:\INTRLANE\...` paths in the `.MAK` files point at the build
server of a company that no longer exists. None of it builds today without a 16-bit
compiler and three vendor libraries (c-tree, GCL520, VGACON) that aren't coming back —
which is why it's worth writing down what it did.
