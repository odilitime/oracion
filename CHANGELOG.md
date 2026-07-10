# Changelog

All notable changes to this project are documented here. This project
does not yet follow a formal release schedule (pre-1.0, no tags), so
entries are grouped by change set rather than version number.

## Unreleased

### Fixed `src/` modular BN3F lexer — build, correctness, runtime

The `src/` tree is a work-in-progress refactor of the single-file
`bn3f.c` reference lexer into one scanner per file. Before this
change, it did not compile as a whole program, and once made to
compile, it hung indefinitely on real input. This change makes it
build, link, and run to completion with output identical to `bn3f.c`
for the same input file.

**Why fix this instead of just using `bn3f.c`?** `bn3f.c` works but is
an ~800-line monolith with no natural seams for the next stage of work
(a parser for Oración's directives and opcodes). The `src/` layout —
one file per scanner, a shared dispatch table, a documented ownership
contract — is the intended long-term shape of the codebase. Fixing it
now, while it's still "just a lexer" and easy to verify against the
working reference, is much cheaper than fixing it later once a parser
depends on it.

#### Link errors

- **Renamed** `_bn3f_lexeme_opgroupopen` to `_bn3f_lex_opgroupopen` in
  `src/lexopgop.c`. *Why:* every other scanner follows the
  `_bn3f_lex_*` naming convention declared in `common.h` and indexed
  by name in `data.c`'s dispatch table; the mismatched name meant the
  symbol referenced by that table never existed at link time.
- **Changed** `_dbg_lexemes[]` and `_bn3f_scan[]` from in-header
  (`static`/plain) array *definitions* to `extern` *declarations* in
  `src/common.h`, with the real definitions moved to `src/data.c` only.
  *Why:* `common.h` is `#include`d by every translation unit in the
  lexer. A non-`extern` array definition in a header is duplicated
  into every object file that includes it, which the linker correctly
  rejects as "multiple definition". `static` in the header made things
  worse in the other direction: each translation unit got its own
  private, all-empty copy, silently breaking anything that read
  `_dbg_lexemes` outside of `data.c`.
- **Aligned** integer types across `common.h`, `lexloop.c`, and
  `lexmain.c` (`ptri`/`s8`/`offs` from `minihn.h`, replacing bare
  `size_t`/`int8_t`/`ssize_t`). *Why:* this codebase's own convention
  (see `minihn.h`) is to use its portable typedefs everywhere instead
  of the platform's own integer types, and the header declarations
  already used `ptri`/`s8` — the `.c` files just hadn't caught up,
  which is a latent portability/consistency bug even though it
  happened to compile.

#### Correctness — data model

- **Added** `start`/`end` stream-offset fields to `struct bn3f_lexeme`
  (`src/common.h`), populated by every scanner from a new `streamoffs`
  parameter passed into each `_bn3f_scan_f`. *Why:* `bn3f.c` already
  tracks these and needs them for future error messages that point at
  a specific byte range in the source file; the `src/` refactor had
  dropped them in favor of a `len`-only representation. Kept `len`
  alongside `start`/`end` rather than removing it, since existing hit
  detection (`l.len > 0` in `lexloop.c`) already depends on it and
  `end - start == len` always holds — `len` can be removed later
  without touching any scanner's control flow.
- **Documented** the ownership/cleanup contract for `bn3f_lex( )`
  directly on its declaration in `common.h`: the returned array and
  every non-`NULL` element are heap-allocated and owned by the caller;
  the array is `NULL`-terminated (iterate to `NULL`, don't assume a
  separately tracked length); partial results remain valid and must
  still be freed even if lexing stopped early. *Why:* this was already
  the de facto behavior, just unwritten — anyone calling `bn3f_lex( )`
  had to read `lexmain.c` and `lexloop.c` to figure out how to safely
  free the result.

#### Correctness — runtime bugs (found while verifying against `bn3f.c`)

These were not visible before, because nothing in `src/` had ever
actually called `bn3f_lex( )` end-to-end.

- **Fixed an infinite loop at end-of-file.** Nearly every scanner's
  mismatch path did `fseek( f, -1, SEEK_CUR )` unconditionally,
  including when the character just read was `EOF`. Since `fgetc( )`
  does not consume a byte once the stream is at EOF, that seek rewinds
  onto the *previous real byte* instead of doing nothing — so the next
  lex iteration reads that same byte again, matches whatever consumed
  it before, and the outer loop in `lexmain.c` never reaches EOF.
  Every scanner now checks `n == EOF` immediately after each `fgetc( )`
  and returns/breaks without seeking in that case, matching the
  pattern already used (correctly) throughout `bn3f.c`.
- **Fixed string/character-literal escape handling.** The `esc` flag
  used to detect an escaped quote (`\"` inside a string, `\'` inside a
  character literal) was toggled on `\\` but never reset on any other
  character. This meant a single backslash escape anywhere in a
  literal would leave `esc` set for the rest of the scan, so every
  later `"` (or `'`) looked escaped and was never accepted as the
  closing quote — the scanner would consume the remainder of the file
  looking for a close it would never take. Fixed by clearing `esc`
  whenever the current character is not a backslash, matching
  `bn3f.c`'s `backslash = 0` reset.
- **Fixed a heap corruption bug** introduced in `lexloop.c`'s array
  growth logic while porting it to use `ptri` types: the `realloc( )`
  call and the following `memset( )` used `sizeof(**lexemes)` (the
  size of one `struct bn3f_lexeme`) instead of `sizeof(*lexemes)` (the
  size of one array element, a `struct bn3f_lexeme *`). This
  under-allocated the pointer array once the lexeme count exceeded the
  initial capacity of 16, corrupting the heap on any input long enough
  to need a resize (`sample.os` itself triggers this).

#### Verification

Verified with a disposable test harness (not committed — `src/main.c`
is deliberately left as the future CLI entry point, separate from
lexer testing) that calls `bn3f_lex( )` and frees every element per
the documented contract:

- Output (lexeme type, `len`, `start`, `end`, `abort`) is identical to
  `bn3f.c`'s trace for `sample.os`, at every token boundary.
- Re-ran with the initial `lexemes_sz` forced down to `2` to exercise
  the `realloc( )` growth path under stress; output was unchanged.
- `cc -Wall` on every touched `src/` file reports zero warnings (the
  only remaining warnings in the tree are in untouched
  `src/getopt2.c`/`src/main.c`).

#### Out of scope for this change

- `src/main.c` does not yet call `bn3f_lex( )` or dispatch to
  `abs`/`bin` via `getopt2`. Wiring the CLI is a larger, separate
  effort — `getopt2` already supports the verb/flag shape the
  `README.md` usage text describes, but there is no parser, symbol
  table, or code emitter yet to hand tokens off to.
- `bn3f.c` was left untouched as the known-good reference
  implementation until `src/` is confirmed to be a full replacement.
