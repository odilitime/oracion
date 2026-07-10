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

- **Kept** a `len`-only `struct bn3f_lexeme` (no `start`/`end` stream
  offsets). *Why:* absolute byte ranges are always recoverable from
  `.len` plus the higher-order context that already tracks the scan
  cursor; stamping offsets onto every lexeme duplicates that state,
  widens the encapsulation surface, and creates more places for the
  two representations to drift. Scanners take only a `FILE *`.
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
- **Hardened allocation failure paths in `lexloop.c`.** `realloc( )`
  and `malloc( )` failures now leave the existing lexeme array intact
  and return a nonzero status instead of overwriting `*lexemes` with
  `NULL` or `memcpy`-ing into a failed allocation — matching the
  documented partial-results ownership contract.
- **Stopped calling `ungetc( EOF )` in the scan loop.** Peeking the
  next character now returns status 1/2 immediately when `fgetc( )`
  yields EOF, rather than invoking undefined behaviour.
- **Fixed finite-repeat `}` acceptance** in `lexopfre.c`: the
  non-digit abort branch ran before the `}` check, so a well-formed
  `{N}` token could never close. Digits and `}` are now ordered to
  match `bn3f.c`.
- **Fixed EOF-safe partial rewinds** in `lexcomnt.c`, `lexopdef.c`,
  and `lexoprng.c`: when a second (or later) `fgetc( )` returns EOF
  without consuming a byte, only the bytes actually read are sought
  back — seeking a fixed `-2` / `-(i+1)` would walk one byte before
  the token start.

#### Verification

Verified with a disposable test harness (not committed — `src/main.c`
is deliberately left as the future CLI entry point, separate from
lexer testing) that calls `bn3f_lex( )` and frees every element per
the documented contract:

- Output (lexeme type, `len`, `abort`, and cumulative stream offset
  derived from successive `.len` values) matches `bn3f.c`'s token
  boundaries for `sample.os`.
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
