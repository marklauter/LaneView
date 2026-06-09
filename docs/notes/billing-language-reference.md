---
title: The billing language — grammar reference
summary: An EBNF grammar, statement and expression reference, host-provided variable namespace, limits, and parser gotchas for billing.prg, reconstructed from BILLING.C.
tags: [laneview, billing, dsl, grammar, ebnf, reference, note]
created: 2026-06-09
aliases: []
document.status: draft
edge.specifies: [docs/notes/03-the-billing-language.md]
---

# The billing language — grammar reference

This is the formal write-up of the language interpreted by `BILLING.C` from a text file named
`billing.prg`. The prose tour — how the evaluator works and why it matters — is in
[[docs/notes/03-the-billing-language.md]]; this document is the spec: lexical rules, grammar,
statements, expressions, the variables the host program provides, the limits, and the parser's
sharp edges.

Everything here is reconstructed from the interpreter source, not from a vendor manual. Where
the grammar idealizes the language, the [parser gotchas](#parser-gotchas) section records what
the implementation actually does.

## Lexical structure

The interpreter normalizes each line before parsing (`Clean()`):

- A semicolon `;` starts a comment that runs to the end of the line.
- All whitespace outside double quotes is removed. Spacing is decorative — `NEWLINE 2` and
  `NEWLINE2` parse identically, and `%a% <= %b% + 1` and `%A%<=%B%+1` are the same statement.
- Text outside double quotes is upper-cased, so keywords and variable names are
  case-insensitive. Text inside double quotes keeps its case and spaces verbatim.

One line holds one statement. The interpreter reads a line, cleans it, and runs the single
statement it finds.

## Grammar (EBNF)

Whitespace is insignificant (stripped before parsing), so the productions below describe the
token stream after normalization. String literals are the only place spaces survive.

```ebnf
program        = { line } ;
line           = [ statement ] [ comment ] ;
comment        = ";" , { character - newline } ;

statement      = newline-stmt
               | goto-stmt
               | line-stmt
               | print-stmt
               | if-stmt
               | else-stmt
               | endif-stmt
               | assignment ;

newline-stmt   = "NEWLINE" , [ integer ] ;            (* advance N print rows; default 1 *)
goto-stmt      = "GOTO" , integer ;                   (* move to column 1..78 *)
line-stmt      = "LINE" , integer , "," , integer ;   (* horizontal rule, column c1..c2 *)

print-stmt     = "PRINT" , print-arg ;
print-arg      = string-literal                       (* prints the literal text *)
               | "%MONTHNAME%"                         (* prints month named by %EndMonth% *)
               | integer , "," , integer , "," , expression ;   (* width , precision , value *)

if-stmt        = "IF" , condition ;
condition      = expression , relop , expression
               | expression ;                          (* bare form: true when value == 0 *)
relop          = "EQUALS" | "ISNOTEQUAL" | "ISLESSTHAN" | "ISGREATERTHAN" ;
else-stmt      = "ELSE" ;
endif-stmt     = "ENDIF" ;

assignment     = scalar-assign | table-assign ;
scalar-assign  = "%" , name , "%" , "<=" , expression ;
table-assign   = "%" , table-name , "%" , index , "<=" , expression ;
index          = expression ;                          (* terminated by the '<' of "<=" *)

expression     = term , { ( "+" | "-" ) , term } ;
term           = factor , { ( "*" | "/" ) , factor } ;
factor         = number
               | "(" , expression , ")"
               | var-ref
               | table-ref ;

var-ref        = "%" , name , "%" ;
table-ref      = "%" , table-name , "%" , "%" , name , "%"    (* index is a variable *)
               | "%" , table-name , "%" , integer ;           (* index is a constant *)

table-name     = "TBL_" , name ;
name           = name-char , { name-char } ;           (* <= 20 chars, no '%' *)
name-char      = letter | digit | "_" | "$" | "@" ;    (* host names use all of these *)
string-literal = '"' , { character - '"' } , '"' ;
number         = [ "-" ] , digit , { digit } , [ "." , { digit } ] ;
integer        = digit , { digit } ;
```

## Statements

### `NEWLINE [n]`

Advance the print cursor down `n` rows (default 1) and return to column 1. The cursor starts at
row 6 — the first six rows belong to the system header — and a statement that pushes it past row
27 is a syntax error.

### `GOTO n`

Move the print cursor to column `n` on the current row. `n` ranges 1..78.

### `LINE c1,c2`

Draw a horizontal rule on the current row from column `c1` to column `c2`. Constraints:
`1 <= c1 <= 78` and `c1 <= c2 <= 78`.

### `PRINT` — three forms

- `PRINT "text"` — print the literal characters between the quotes, case and spaces preserved.
- `PRINT %MONTHNAME%` — print the English month name selected by the host variable `%EndMonth%`
  (1 = January). The argument is fixed; it reads `%EndMonth%`, not a parameter you pass.
- `PRINT width,precision,expression` — evaluate `expression` and print it with C `printf`
  `%<width>.<precision>f` formatting. The interpreter builds that format string at runtime from
  the two integers, so `PRINT 10,2,%MonthKWH%` prints the kWh total right-justified in 10
  columns with 2 decimals.

### `IF` / `ELSE` / `ENDIF`

`IF condition` opens a block; `ELSE` inverts it; `ENDIF` closes it. Blocks nest (about 100 deep).
A condition takes one of two forms:

- `expression relop expression` — compare two values with `EQUALS`, `ISNOTEQUAL`, `ISLESSTHAN`,
  or `ISGREATERTHAN`. Comparisons run on `double` values.
- `expression` alone — the bare form is true when the expression evaluates to exactly `0`. (Yes,
  zero is the true case here. Prefer the explicit relational form.)

### Assignment — `%Name% <= expression`

The assignment operator is `<=`, not `=`. The left side is a variable reference; the right side
is any expression. A variable is created on first assignment.

- Scalar: `%Discount% <= %MonthSTD$% * 0.1`
- Table: `%TBL_HourCost% %Hour% <= %Price%` defines (or updates) row `%Hour%` of table
  `TBL_HourCost`. Table names carry the `TBL_` prefix; the index sits between the name's closing
  `%` and the `<=`, and may itself be an expression.

## Expressions

`Evaluate()` supports `+ - * /`, parentheses, numeric literals, variable references, and table
references. Precedence is the usual two tiers — parentheses, then `*` and `/`, then `+` and `-`
— enforced by the order the evaluator reduces the string (see
[[docs/notes/03-the-billing-language.md]] for the mechanism). Values are `double`.

Reading a variable splices its value into the expression: `%Rate%` becomes its number. Reading a
table cell takes a constant or variable index — `%TBL_HourCost%3` or `%TBL_HourCost%%Hour%`.

The `operators` table in the source lists `%` alongside the four arithmetic operators, but the
evaluator implements only `+ - * /`; there is no working modulo, and `%` serves solely as the
variable delimiter.

## Host-provided variables

Before `billing.prg` runs, `CalculateBill()` walks the c-tree history for the billing period and
pre-populates a set of variables. The script reads these to typeset the bill; they are ordinary
scalars (names are case-insensitive).

Period (set from the selected billing dates):

- `%StartDay%`, `%StartMonth%`, `%StartYear%` — period start (year is four digits, e.g. 1998).
- `%EndDay%`, `%EndMonth%`, `%EndYear%` — period end. `%EndMonth%` also drives `PRINT %MONTHNAME%`.

Account:

- `%RateType%` — 0 = standard/flat, 1 = time-of-use, 2 = variable/real-time.
- `%FranchiseFeeType%`, `%CityTaxType%` — jurisdiction indices.

Energy and demand:

- `%MonthKWH%` — total kWh for the period.
- `%PeakDemand%` — sliding peak demand over the period.

Cost totals (dollars):

- `%MonthSTD$%` — cost under the standard/flat rate.
- `%MonthTOD$%` — cost under time-of-use.
- `%MonthVAR$%` — cost under variable/real-time pricing.

Per rate level, for each level `n` in 0..15:

- `%MonthTodKWH@Level<n>%`, `%MonthVarKWH@Level<n>%` — kWh billed at that TOD / variable level.
- `%MonthTOD$@Level<n>%`, `%MonthVAR$@Level<n>%` — dollars at that level.

Load control (demand response):

- `%LoadControlKwhSaved%`, `%LoadControlCredit$%` — energy shed and the credit earned.
- `%LoadControlKwhUsed%`, `%LoadControlPenalty$%` — energy used through an override and the penalty.

The `TBL_` table feature is for the bill author's own scratch tables; the host populates scalars
only, using the `@Level<n>` naming rather than `TBL_` arrays.

## Limits

- Print area: rows 6..27, columns 1..78.
- Variables: 250 scalars; names up to 20 characters.
- Tables: 5 distinct `TBL_` tables, 256 rows each.
- `IF` nesting: ~100 levels.
- Keywords: `NEWLINE`, `GOTO`, `PRINT`, `LINE`, `ENDIF`, `IF`, `ELSE`, plus `%` for assignment.

## Parser gotchas

The interpreter is small and string-based, which buys a few sharp edges worth knowing:

- Keywords are matched as substrings anywhere on the line, scanned in a fixed order (`NEWLINE`,
  `GOTO`, `PRINT`, `LINE`, `ENDIF`, `IF`, `ELSE`, `%`) — not by position. The first keyword found
  wins. So a keyword that appears inside a string literal or a variable name can hijack the line.
  `PRINT "GOTO the store"` parses as a `GOTO`, and a variable named `%NOTIFY%` parses as an `IF`.
  Keep literals and names clear of the reserved words. This is why the host variable names avoid
  keyword substrings.
- Assignment uses `<=`, and the table index is read up to the `<` — an index expression cannot
  contain `<`, which is fine since no operator uses it.
- The bare `IF expression` form treats `0` as true. Use the relational forms for clarity.
- Comparisons use exact `double` equality, so `EQUALS` on computed fractional values is fragile;
  compare integers or pre-rounded values.
- Each line runs exactly one statement; there is no statement separator.

## A worked example

```
; ---- billing.prg (excerpt) ----
; The host has already set StartMonth/Day/Year, EndMonth/Day/Year,
; MonthKWH, MonthSTD$, MonthTOD$, MonthVAR$, RateType, PeakDemand.

GOTO 30
PRINT "Estimated Electric Bill"
NEWLINE 2

PRINT "Service month: "
PRINT %MONTHNAME%                 ; named by %EndMonth%
PRINT " "
PRINT 4,0,%EndYear%
NEWLINE 1
LINE 1,78
NEWLINE 1

PRINT "Energy used (kWh): "
GOTO 40
PRINT 10,2,%MonthKWH%
NEWLINE 1

IF %RateType% EQUALS 0
  PRINT "Plan: Standard"
  NEWLINE 1
  PRINT "Amount due: $"
  GOTO 40
  PRINT 10,2,%MonthSTD$%
ELSE
  IF %RateType% EQUALS 1
    PRINT "Plan: Time-of-Use"
    NEWLINE 1
    PRINT "Amount due: $"
    GOTO 40
    PRINT 10,2,%MonthTOD$%
  ELSE
    PRINT "Plan: Real-Time"
    NEWLINE 1
    PRINT "Amount due: $"
    GOTO 40
    PRINT 10,2,%MonthVAR$%
  ENDIF
ENDIF
NEWLINE 2

; a value the script derives for itself:
%AvgCents% <= ( %MonthTOD$% / %MonthKWH% ) * 100
PRINT "Avg cents/kWh: "
GOTO 40
PRINT 8,3,%AvgCents%
```

This exercises column positioning (`GOTO`), all three `PRINT` forms, rules (`LINE`), row advance
(`NEWLINE`), nested `IF/ELSE/ENDIF`, and a scalar assignment with parenthesized arithmetic over
host-provided variables.
