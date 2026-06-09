# TeCom's Interlane LaneView - a residential energy usage browser and home automation system from the 1990s

The client portion of a residential home automation and energy management system I worked on in the 1990s for TeCom, A TECO Energy Company.

It was my first project as a software engineer, written ~1996–1998 in 16-bit Microsoft C (the Visual C++ 1.x era — large memory model, 8086 codegen, DOS overlays). No framework, no GUI library, barely an OS underneath it. Below is the highlight reel — the stuff a modern developer reads and says "wait, you did *that* — in 16-bit C, with no framework and no Stack Overflow?" The full field notes, with code, live in [`docs/notes/`](docs/notes/README.md).

## The headline acts

- **A programming language, written to print a bill.** `billing.prg` is a real interpreted DSL — variables, `TBL_` arrays, nested `IF/ELSE/ENDIF` on a 100-deep stack, and a recursive expression evaluator that computes by rewriting the string in place. The C engine crunches the totals, injects them into the interpreter's namespace, and the script typesets the page: a compute-engine/template split across a language boundary, in 1997. → [the billing language](docs/notes/03-the-billing-language.md)
- **A full object-oriented GUI framework in plain C.** Classes, single inheritance via first-member layout, virtual dispatch with an explicit `this`, and RTTI type tags — precisely how a C++ compiler implements all of that under the hood, hand-built before I was writing C++. Adding a widget means embedding the base struct and writing five methods. → [object-oriented programming in plain C](docs/notes/01-object-oriented-c.md)
- **Reentrant modal dialogs that stay alive.** `msgbox()` runs its own complete copy of the event loop, so a dialog popped from deep inside any handler keeps the clock ticking, the caret blinking, and the home-automation bus serviced while it waits for a click — plus pixel-perfect screen save/restore via a far-pointer grab, with no compositor to lean on. → [the message box and screen save](docs/notes/04-msgbox-and-screen-save.md)
- **A word-wrap word processor.** The memo box does automatic greedy word wrapping and keeps a live two-way map between a flat character buffer and the wrapped display lines, so insert, delete, arrows, click-to-position, and scrolling all work. → [more cool algorithms](docs/notes/08-more-cool-algorithms.md)

## Ahead of its time

- **Smart-grid energy management, a decade early.** Sixteen-level time-of-use *and* real-time pricing (nibble-packed, two hours per byte), thermostats that set back by the current price tier, and automated demand response with the credits earned and penalties owed reconciled right on the bill. → [the energy-management domain](docs/notes/06-energy-management-domain.md)
- **IFTTT, in 1996.** Map a cheap X-10 remote button to start a "mood" (scene) or flip the house's season, occupancy, or HVAC mode, with address-collision checks — press a button, the house re-plans itself. → [the energy-management domain](docs/notes/06-energy-management-domain.md)
- **Built-in product analytics.** Every keystroke and click logged with a timestamp, the active window's title, and a stable button ID — session telemetry, hand-rolled into a DOS binary before that was a common idea. → [the message loop](docs/notes/02-the-message-loop.md)
- **A DOS app that cooperates with Windows.** It detects whether it's running under Windows (INT 2Fh) and shells out to `winhelp` to show a real help window — reaching up into the GUI it runs under. → [the message loop](docs/notes/02-the-message-loop.md)

## Craftsman details

- **A dual-Y-axis chart compositor** — kWh bars, a 15-minute interval line, and a cost line on its own scale, color-coded by demand-response state, auto-scaled with a nice-number ladder, fed by a keyed scan of the c-tree database. → [more cool algorithms](docs/notes/08-more-cool-algorithms.md)
- **An epoch-counting perpetual calendar** — the weekday of any date by counting days from "Jan 1, 1980 was a Tuesday," mod 7. → [more cool algorithms](docs/notes/08-more-cool-algorithms.md)
- **Function pointers used as return addresses** (`Sender`), so reusable screens know which menu to return to — a one-slot call stack made of a pointer. → [more cool algorithms](docs/notes/08-more-cool-algorithms.md)
- **Hand-drawn Windows-95 bevels**, built one light-and-shadow stroke at a time. → [the widget toolkit](docs/notes/05-the-widget-toolkit.md)
- **Hand-authored 16×16 AND/XOR mouse cursors**, and a pointer that shows a different mood face per screen quadrant on the intro screen. → [mouse cursors and easter eggs](docs/notes/07-mouse-cursors-and-easter-eggs.md)

## What it did

A homeowner ran LaneView on a PC that spoke CEBus and X-10 to an ICU — Intelligent Control Unit — that switched loads, read submeters, and took load-control signals from the electric utility. From one DOS binary you could watch your energy use hour by hour, estimate your bill under flat / time-of-use / real-time pricing, schedule devices around the price of power, automate scenes from a remote, and trade load-control participation for credits on your bill. The copyright headers name TeCom Inc.; the tax tables name Polk City and Oldsmar, Florida — a late-'90s TECO pilot.

## The field notes

Deep dives, with code, in [`docs/notes/`](docs/notes/README.md):

- [01 — Object-oriented programming in plain C](docs/notes/01-object-oriented-c.md)
- [02 — The message loop](docs/notes/02-the-message-loop.md)
- [03 — The billing language](docs/notes/03-the-billing-language.md)
- [04 — The message box, text wrap, and screen save/restore](docs/notes/04-msgbox-and-screen-save.md)
- [05 — The widget toolkit](docs/notes/05-the-widget-toolkit.md)
- [06 — The energy-management domain](docs/notes/06-energy-management-domain.md)
- [07 — Mouse cursors and easter eggs](docs/notes/07-mouse-cursors-and-easter-eggs.md)
- [08 — More cool algorithms](docs/notes/08-more-cool-algorithms.md)

## Building it in 2026

You can't, quite — and that's part of the charm. The formatting was normalized (LF, spaces, clang-format) on the `lauterm/cleanup` branch, but the logic is exactly as shipped. The `$Log$` and `$Workfile$` headers are original RCS keywords; the `J:\INTRLANE\...` paths in the makefiles point at a build server long gone. It needs a 16-bit compiler and three vendor libraries (c-tree, GCL520, VGACON) that aren't coming back — which is why it's worth writing down what it did.
