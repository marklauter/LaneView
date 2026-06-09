---
title: The energy-management domain
summary: Price-aware device scheduling, nibble-packed 16-level rate signals, demand-response credits and penalties, moods/scenes, and X-10 trigger automation — a residential energy system from the late 1990s.
tags: [laneview, energy, domain, automation, demand-response, note]
created: 2026-06-08
aliases: []
document.status: draft
---

# The Energy-Management Domain

Strip away the GUI and the billing interpreter, and what's left is a genuinely
forward‑looking **residential energy‑management system**. In the late 1990s — before
"smart home," before "smart grid," before Nest — LaneView modeled time‑of‑use pricing,
automated demand response, price‑aware scheduling, and scene/automation rules. The data
model lives in `2box.h`; the behavior lives in `device.c`, `event.c`, `trigger.c`,
`inhist.c`, and `WHMENU.C`.

## Devices

Up to 64 devices (`MAX_DEVICES`), each addressable over **X‑10** (house letter + unit
number) or **CEBus** (house code + unit + subunit). The `DEVICE` struct (`2box.h`) carries
a lot of domain knowledge:

```c
USHORT device_slot;
USHORT watts;            // volts * amps * power_factor
UCHAR  category;         // 1 lighting, 2 environmental, 3 pool/spa, 4 sprinkler, 5 alert
UCHAR  priority;         // for load shedding (demand response)
UCHAR  monitored;        // submetered?
...
UCHAR  x10_device_type;  // 0 relay, 1 dimmer, 2 annunciator, 3 RCS thermostat
UCHAR  cooling_setbacks[4];   // thermostat setpoint per rate level (low[0]..high[3])
UCHAR  heating_setbacks[4];
USHORT max_mins_day;     // smart-scheduling budget: max minutes of runtime per day
USHORT cost_day_hour;    // smart-scheduling budget: max cents per day (or per hour*10)
UCHAR  operate_hours[3]; // 24-bit mask: which hours smart scheduling may run
```

Two things stand out:

- **Thermostats set back by price.** A device of type 3 (RCS thermostat) stores four
  cooling and four heating setpoints indexed by the *current rate level* — so the house
  automatically eases the setpoint as electricity gets more expensive. That's
  price‑responsive HVAC, in firmware‑adjacent C, in the '90s.
- **"Smart scheduling" is a budget optimizer.** A device can be told "run at most N
  minutes/day, spend at most M cents/day, only during these hours" and the system shops
  for the cheapest time to run it. `smartscd.c` (`SMARTSCD`) implements the shopping.

Submeters (`IsSubmeter`) monitor up to four loads each (`cebus_subunit`), feeding the
per‑device history that the bill and load‑management screens read back.

## Events: schedules that respond to price, season, and occupancy

Up to 512 events (`MAX_EVENTS`). An `EVENT` (`2box.h`, masks in `event.h`) is not just
"do X at time T." It carries **three bitmask dimensions** that gate whether it fires:

```c
UCHAR event_type;   // 0 standard, 1 one-time, 2 dawn, 3 dusk, 4 random threshold
SSHORT offset_mins; // +/- minutes around dawn/dusk (or jitter window for type 4)

UCHAR mask1;        // season + mode:  Vacation/Normal | Heating/Cooling | Spring/Summer/Autumn/Winter
UCHAR mask2;        // cost + occupancy: Critical/High/Medium/Low cost | Home/Away
UCHAR mask3;        // reserved
```

So a single event can mean: *"Turn the pool pump on at dusk minus 30 minutes, on
weekdays, in summer, while occupied — but only when electricity is in the Low or Medium
cost tier."* The scheduler (`ScheduleEvents`, `AdjustEvents`) evaluates the masks against
the current season/mode/occupancy/**cost signal** to decide what actually runs. Dawn/dusk
events with offsets, and "random threshold" events (occupancy‑simulation jitter so a
vacant house looks lived‑in), are first‑class types.

The cost dimension is the interesting one. The utility broadcasts a **rate signal** (one
of several cost tiers); the house's automation reshapes itself around the price of power
in real time. That's the core idea of demand response and time‑of‑use, expressed as a
bit in an event mask.

## Rate signals: 16 levels, nibble-packed

Pricing isn't on/off‑peak — it's **16 levels** (0–15), per hour, for two pricing models
(time‑of‑use and "variable"/real‑time). To store 24 hourly 4‑bit levels in 12 bytes,
they're packed two‑per‑byte (`DAYS_RATES` in `2box.h`):

```c
UCHAR rtod_signal[12];   // 24 hours of TOD rate level, 2 hours per byte
UCHAR rsv_signal[12];    // 24 hours of variable/real-time rate level
ULONG rtod_rate[24];     // actual price per hour (100,000ths of a dollar)
ULONG rsv_rate[24];
ULONG credit_rate, penalty_rate;
```

Unpacking (from `CalculateBill`, [03-the-billing-language.md](03-the-billing-language.md)):
even hours live in the high nibble, odd hours in the low nibble. Prices are integers in
hundred‑thousandths of a dollar to avoid storing floats — a classic embedded‑systems
fixed‑point choice.

## Demand response: credits and penalties

When the grid is stressed, the utility issues **load‑control** signals to shed
high‑priority devices (the `priority` field). LaneView's load‑management screens
(`WHMENU.C`) let the homeowner *see* and *participate*:

- **Status** — which devices are currently under control, and the start/stop window.
- **Override** — keep a device running through a control event, with start/end times that
  the UI snaps to 15‑minute boundaries (`lmGetTime` / `lmdateBtnOkClick`), sent to the ICU
  via `send_load_ctrl_override`. **Emergency** controls (level 4) refuse override.
- **History + accounting** — `BuildLoadCtrlHistory` walks the per‑device history and tallies
  the **credit** earned for cooperating and the **penalty** owed for overriding:

```c
if (hist_tran.ht_flags & 0x040) { *lc_kwh_saved += ht_kwh; *lc_credit$  += ht_kwh_cost * ht_kwh; }
if (hist_tran.ht_flags & 0x020) { *lc_kwh_used  += ht_kwh; *lc_penalty$ += ht_kwh_cost * ht_kwh; }
```

This is a working consumer‑facing demand‑response loop: the utility asks the house to use
less, the house can say no, and the bill reflects the choice — exactly the model that
"smart grid" programs would standardize a decade later.

## The Rate Comparison screen

`RateDraw()` (`WHMENU.C`) is the consumer‑education payoff: a 24‑hour table, today or
month‑to‑date, showing each hour's kWh and what it cost under **three different rate
plans side by side** — Standard (flat), Time‑of‑Use, and Variable:

```
  HOUR    KWH  $-Std    $-Tod    $-Var
12- 1am   1.2   0.10     0.07     0.14
 1- 2am   0.9   0.08     0.05     0.06
 ...
TOTALS:  28.4   2.41     1.98     2.63
```

It lets a homeowner literally see whether switching to time‑of‑use pricing would save or
cost them money, hour by hour, on their own usage.

## Moods and triggers: IFTTT in 1996

A **mood** (`MOOD_T`, `moodmenu.c`) is a named scene — a set of device states you can
start and end. A mood can be **protected** from the schedule for N minutes or "forever,"
so "Movie Night" stays put even if a scheduled event would otherwise change the lights.

A **trigger** (`TRIGGER_T` in `2box.h`, engine in `trigger.c`) maps an **incoming X‑10
remote button** to an automation action. The struct is essentially an
event‑condition‑action rule:

```c
UCHAR  code_letter;     // X-10 house address (the remote button you press)
UCHAR  code_number;
UCHAR  incoming_state;  // 0 = Off, 1 = On
UCHAR  trigger_action;  // 1 start mood, 2 end mood, 3 end all moods, 4 set day type,
                        // 5 master mode, 6 HVAC mode, 7 season, 8 occupancy
USHORT mood_id;         // for actions 1/2
USHORT execute_type;    // protected? which mode value?
USHORT minutes;         // protection duration (0 = forever)
```

So: *press button "B‑3 On" on a cheap keychain X‑10 remote → start the "Away" mood,
protected for 8 hours.* Or *"D‑1 Off" → set occupancy to Temporarily Away,* which in turn
re‑gates every event whose `mask2` requires the Away bit. Press a remote, the house
re‑plans itself. The editor (`EditTrigger` in `trigger.c`) even guards against assigning a
trigger to an X‑10 address already owned by a device. This is "if this, then that" home
automation — the press‑a‑button‑run‑a‑scene pattern that became a consumer product
category twenty years later — shipped on an 8086.

## Why this matters

The UI and the interpreter are the flashy parts, but the domain model is the quiet
achievement: time‑of‑use and real‑time pricing, automated and economically‑accountable
demand response, price‑responsive thermostats, occupancy and seasonal modes, scene
automation, and remote‑triggered rules — a coherent residential energy platform, modeled
in C structs and bitmasks, years ahead of the market that eventually caught up to it.
