---
title: The billing language interpreter
summary: billing.prg is a real interpreted DSL — variables, tables, IF/ELSE/ENDIF, and a recursive string-rewriting expression evaluator — that the C engine feeds computed values into to typeset a utility bill.
tags: [laneview, interpreter, dsl, billing, algorithm, note]
created: 2026-06-08
aliases: []
document.status: draft
---

# The billing language (`BILLING.C` plus `billing.prg`)

This is the most surprising thing in the codebase. The estimated-bill screen isn't laid out
in C. It's laid out by a small interpreted programming language whose source lives in a
plain-text file called `billing.prg` that ships next to the executable. A utility could
change how the bill reads, computes, and prints — fonts of math included — by editing a text
file, with no recompile.

`BILLING.C` is a complete little language implementation: a cleaner/tokenizer, a statement
interpreter with control flow, a variable and array store, and — the gem — a recursive
expression evaluator that works by rewriting the string in place.

For the formal spec — an EBNF grammar, the statement and expression reference, the
host-provided variable namespace, and the parser's gotchas — see
[[docs/notes/billing-language-reference.md]]. This note walks the implementation; that one
pins down the language.

## The shape of the language

From the interpreter, the language offers:

```c
#define MAX_KEYWORDS  8
UCHAR* keywords[MAX_KEYWORDS] = {
  "NEWLINE",  // 0  advance N print lines
  "GOTO",     // 1  move to column N on the current line
  "PRINT",    // 2  print a literal, a month name, or a formatted number
  "LINE",     // 3  draw a horizontal rule between two columns
  "ENDIF",    // 4
  "IF",       // 5
  "ELSE",     // 6
  "%"         // 7  variable assignment:  %name% <= expr
};
```

A `billing.prg` line looks like one of:

```
; comment to end of line
%MonthCharge% <= %MonthKWH% * %Rate% + 7.50      ; assignment + arithmetic
IF %RateType% EQUALS 2                            ; conditional
  PRINT "Time-of-Use Plan"
  PRINT 10,2,%MonthTOD$%                          ; width 10, 2 decimals, a value
ELSE
  PRINT "Standard Plan"
ENDIF
NEWLINE 2
LINE 1,78                                         ; horizontal rule across the page
```

So: variables (`%name%`), tables/arrays (`%TBL_...%`), arithmetic with precedence and
parentheses, comparisons (`EQUALS`, `ISNOTEQUAL`, `ISLESSTHAN`, `ISGREATERTHAN`), nested
`IF/ELSE/ENDIF`, and three flavors of `PRINT`. That's a language.

## The pipeline

`BillingStatement()` drives it. For each line of `billing.prg`:

```c
Read(line, prgfile);              // fgets one line
strcpy(in_line, line);            // keep the original for error messages
if (Clean(line) != 0)             // strip comments + whitespace, uppercase
  if (Process(line) == P_FAILURE) // interpret it
    msgbox("Syntax error at line # %d|%.25s", ...);  // report with the raw line
```

### 1. `Clean()` — the tokenizer/normalizer

Strips `;` comments, removes all whitespace except inside double quotes, and uppercases
everything outside quotes (so keywords and variable names are case-insensitive while quoted
print strings keep their case):

```c
if ((tok = strstr(line, ";")) != NULL) *tok = 0x00;   // kill comment
for (i = 0, j = 0; i < strlen(line); i++) {
  if (quote == 0) {
    if (line[i] > ' ') {                 // drop spaces/control chars
      if (line[i] == '"') quote = 1;
      newline[j++] = toupper(line[i]);
    }
  } else {                               // inside quotes: copy verbatim
    if (line[i] == '"') quote = 0;
    newline[j++] = line[i];
  }
}
```

### 2. `Process()` — the statement interpreter and the IF stack

`GetKeyWord()` finds which keyword the line contains, then a `switch` executes it. The clever
part is control flow. There's a 100-deep boolean stack:

```c
static UCHAR process_if[100];   // process_if[level] == "are we executing at this depth?"
static UCHAR if_level;
```

- `IF` pushes a level, evaluates the condition, sets `process_if[if_level]`.
- `ELSE` flips the current level with XOR: `process_if[if_level] ^= TRUE;`
- `ENDIF` pops a level.

When the current level is false (a skipped branch), `Process` runs a reduced switch that
still tracks `IF`/`ELSE`/`ENDIF` nesting — so an `IF` nested inside a not-taken branch is
correctly skipped, and an `ELSE` re-enables its block only if the parent level is itself
true:

```c
} else {                       // process_if[if_level] == FALSE: skipping
  switch (kw) {
    case 5: if_level++; process_if[if_level] = FALSE; break;          // nested IF
    case 6: if (process_if[if_level - 1] == TRUE)                      // ELSE
              process_if[if_level] ^= TRUE; break;
    case 4: process_if[if_level] = TRUE; if_level--; break;           // ENDIF
  }
}
```

That's a correct, properly nested conditional interpreter in about forty lines.

### 3. Variables and tables

A flat symbol table, up to 250 entries, auto-created on first assignment:

```c
typedef struct {
  UCHAR  name[26];
  UCHAR  type;    // 0 = scalar, 1 = array/table member
  UCHAR  index;   // which column of arrays[][]
  double value;
} VARIABLE;
VARIABLE variable[MAX_VARIABLES];      // 250 scalars
double   arrays[256][MAX_ARRAYS];      // up to 5 tables of 256 rows
```

Scalars are `%Name%`. Tables carry a `TBL_` prefix and an index — `%TBL_TodCost% <= ...` to
define the column, `%TBL_TodCost[%Hour%]%` to read row `Hour`. `ProcessVariable()` detects
the prefix, allocates an array column on first use, and evaluates the index expression to
pick the row.

## The crown jewel: a string-rewriting evaluator

`Evaluate()` (credited to GES, Gary Speegle) computes the value of an expression string. It
builds no AST and no token stream. It repeatedly rewrites the string in place, each pass
replacing one sub-expression with its numeric value printed back as text, until only a number
remains — then `atof()`s it. It's a calculator implemented as iterated search-and-replace.

It runs four phases, in precedence order.

Phase 1 — variable substitution. Find `%name%`, look up the value, splice the number into the
string:

```c
*cp1 = 0; ++cp1;                       // split at the first %
cp2 = strchr(cp1, '%');                // find the closing %
*cp2 = 0; ++cp2;
accumulator = GetValue(cp1);           // look up the variable
sprintf(work, "%f", accumulator);      // its value as text
strcat(cp, work); strcat(cp, cp2);     // "...VALUE<rest>"
```

So `%Rate%*100` becomes `0.085000*100` as a literal string. Tables resolve their index the
same way (the index itself can be a `%var%`, evaluated recursively).

Phase 2 — parentheses, recursively. Find the first `(`, scan for its matching `)` with a depth
counter, recursively `Evaluate()` the inside, and replace `(...)` with the result:

```c
for (cp2 = cp1 + 1, level = 0; *cp2 != 0; ++cp2) {
  if (*cp2 == '(') { ++level; continue; }
  else if (*cp2 == ')') {
    if (level > 0) { --level; continue; }     // inner paren
    *cp2 = 0; *cp1 = 0;                        // isolate the inside
    accumulator = Evaluate(cp1 + 1);           // <-- recursion
    sprintf(work, "%f", accumulator);
    strcat(cp, work); strcat(cp, cp2 + 1);
    break;
  }
}
```

Phase 3 — `MultiplyDivide()`, then Phase 4 — `AddSubtract()`. Each scans for its operators left
to right, pulls the operand on each side (walking backward to the previous operator to find
the left operand), computes, and rewrites operator-plus-operands as the result. Doing all `*`
and `/` before any `+` and `-` is how operator precedence is enforced — no grammar needed,
just ordering the passes.

Sign hygiene — `Simplify()`. Because results splice back as signed numbers, you get sequences
like `5+-3` or `5--3`. `Simplify()` runs between phases and collapses them:

```c
// "+-"  ->  "-"        "--"  ->  "+"
if (*cp1 == '+' && *(cp1+1) == '-') { strcpy(cp1, cp1 + 1); }
if (*cp1 == '-' && *(cp1+1) == '-') { strcpy(cp1, cp1 + 1); *cp1 = '+'; }
```

It's quadratic string-thrashing and would never survive a CS exam on parsing — but for a
one-line bill formula it's clear, correct, debuggable (you can watch the string shrink), and
completely adequate. There's a real charm to a calculator you could single-step by printing
one variable.

## `PRINT`: three modes, including a runtime-built format string

`Print()` decides what to do from the argument:

```c
if (str[5] == '"') {                         // 1. quoted literal -> print char by char
  ...
} else if (strcmp(str + 5, "%MONTHNAME%") == 0) {   // 2. month name lookup
  strcpy(work, "%ENDMONTH%");
  text(month_name[(int)Evaluate(work) - 1], BG_WHT + FG_BLK);
} else {                                      // 3. PRINT width,precision,expr
  bm  = strstr(str, ",") + 1;                 // precision field
  var = strstr(bm,  ",") + 1;                 // the expression
  value = Evaluate(var);
  sprintf(buf, "%%%d.%df", atoi(&str[5]), atoi(bm));  // BUILD a printf format at runtime
  sprintf(output, buf, value);                         // ...then use it
  text(output, BG_WHT + FG_BLK);
}
```

Mode 3 is delightful: from `PRINT 10,2,%expr%` it constructs the format string `"%10.2f"` at
runtime and feeds it to `sprintf`. The language lets the bill designer choose column width and
decimal places per number, and the interpreter manufactures the exact `printf` spec to honor
it.

## Engine and template, cleanly split

Here's the architectural punchline. `CalculateBill()` is the number-crunching half. It walks
the c-tree energy-history file day by day across the billing cycle, reads each interval's kWh,
and accumulates totals — flat, time-of-use, and variable-rate costs, per-level kWh, peak
demand, load-control credits and penalties. Then it prints nothing. Instead it injects every
result into the interpreter's variable namespace by synthesizing assignment statements and
running them through `Process()`:

```c
sprintf(line, "%%MonthKWH%%<=%f",   month_kwh);     Process(strupr(line));
sprintf(line, "%%MonthSTD$%%<=%f",  rs_total_cost);  Process(strupr(line));
sprintf(line, "%%MonthTOD$%%<=%f",  rst_total_cost); Process(strupr(line));
sprintf(line, "%%PeakDemand%%<=%f", peak);           Process(strupr(line));
for (i = 0; i < 16; i++) {                           // 16 rate levels -> table columns
  sprintf(line, "%%MonthTodKWH@Level%d%%<=%f", i, tod_kwh_at[i]);  Process(strupr(line));
  ...
}
```

So the flow is: C computes the facts → stuffs them into the DSL's symbol table → `billing.prg`
reads those variables and typesets the page. The engine and the presentation are decoupled
across a language boundary, in a 16-bit DOS program. That separation — compute in fast native
code, lay out in an editable data-driven template — is the same idea behind every report
engine and templating system since. Finding it hand-built here, in 1997, is the best surprise
in the repo.

## Domain detail worth keeping

`CalculateBill()` also shows how the metering data is encoded (more in
[[docs/notes/06-energy-management-domain.md]]):

- 16 price levels, nibble-packed. `rtod_signal[12]` stores 24 hourly rate levels (0–15), two
  per byte: even hours in the high nibble, odd hours in the low nibble.
  ```c
  rate_signal = days_rates.rtod_signal[hist_tran.ht_hr / 2];
  if (hist_tran.ht_hr & 1) rate_signal &= 0x0F;   // odd hour: low nibble
  else                     rate_signal /= 16;      // even hour: high nibble
  ```
- Rates in 100,000ths of a dollar (`work * rate / 100000.0`) to keep integer precision without
  floats in storage.
- Sliding peak demand tracked across consecutive intervals:
  `if (work + last_kwh > peak) peak = work + last_kwh;`
- Demand-response accounting: history flags mark credit (`0x40`) when a load was shed and
  penalty (`0x20`) when the customer overrode and kept drawing power during a control event.
