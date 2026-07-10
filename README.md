# Oración assembler

Copyright © 2023-2024 Alexander Nicholi.
All rights reserved.

-----

## What this is

Oración is an assembler targeting a custom instruction set (originally
i286 and ARMv4T). Its assembly language is not line-oriented — `;`
terminates statements and braces delimit blocks, so `\r`/`\n` are just
whitespace like any other. See the
[XionKB article](https://wiki.xion.mt/Oración) for the full language
spec (directives, constant expressions, opcode structure).

This repository is early-stage. The planned two-pass pipeline is
documented below under **Usage**, but is not wired up yet. What
exists today:

* **`bn3f.c`** — a self-contained, working reference lexer for
  **BN3F**, a small BNF-like grammar notation used to describe
  Oración's own token grammar. Compiles standalone; see
  [`bn3f.c`](bn3f.c).
* **`src/`** — the same BN3F lexer, refactored into one file per
  scanner (`src/lex*.c`) behind a shared `bn3f_lex( )` entry point
  (`src/lexmain.c`). This is intended to eventually replace `bn3f.c`
  and grow into the actual `oracion abs`/`oracion bin` implementation.
  It now builds, links, and produces output identical to `bn3f.c` for
  the same input — see **Building** below.
* **`src/getopt2.*`** — a small, generic verb/flag argument parser
  already shaped to match the `oracion abs`/`oracion bin` CLI (see
  Usage), but not yet called from `src/main.c`.
* **`oracion.c`**, **`oracion.py`**, **`bn3f.l`** — earlier prototypes
  and stubs kept for reference; not part of the active build.

## Usage (target CLI — not yet implemented end-to-end)

```
oracion (h|help|-h|--help)
	Display help message and exit.

oracion abs [(-m|--map) <mapfile.ini>] (<input.os>|-) [<output.osa>]
	Translate high-level Oracion assembly (with labels, offsets and
	external symbols) to low-level "absolute assembly" (with only
	instructions, and relative or absolute numerics). If output.osa is
	not provided, stdout is used.

oracion bin (<input.osa>|-) [<output.bin>]
	Translate low-level "absolute assembly" into machine code. If
	output.bin is not provided, stdout is used.
```

## Building

There is no Makefile yet (see `CHANGELOG.md` for the reasoning behind
keeping the two lexer implementations separate for now). Both can be
built directly with any ANSI C89 compiler and no external
dependencies:

```sh
# reference lexer + built-in test harness
cc -o bn3f bn3f.c
./bn3f sample.os

# modular lexer (src/) — builds, but nothing calls bn3f_lex( ) from
# main( ) yet; getopt2 is parsed and then discarded. Link it into your
# own test harness by including src/common.h and calling bn3f_lex( ):
cc -c src/data.c src/lex*.c src/getopt2.c src/main.c
```

Both implementations tokenize `sample.os` identically, byte offset for
byte offset — this was verified as part of the `src/` refactor (see
`CHANGELOG.md`).

## Repository layout

```
oracion/
├── README.md, CHANGELOG.md
├── sample.os          # example BN3F input
├── bn3f.c / bn3f       # reference lexer, working, self-contained
├── oracion.c / .py     # early prototypes, not built
├── bn3f.l              # incomplete Flex grammar stub
└── src/                 # modular lexer + CLI scaffolding
    ├── common.h         # shared types, lexeme enum, scanner table
    ├── minihn.h         # portable integer/pointer typedefs (u8, ptri, ...)
    ├── data.c           # scanner dispatch table + debug names
    ├── lexmain.c        # bn3f_lex( ) — top-level driver
    ├── lexloop.c        # per-iteration scan-and-collect logic
    ├── lex*.c           # one scanner per BN3F token type
    ├── getopt2.{c,h}    # CLI argument parser (verbs + flags)
    └── main.c           # entry point (getopt2 wired up; CLI dispatch TBD)
```
