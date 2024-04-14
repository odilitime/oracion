# Oración assembler

Copyright © 2023-2024 Alexander Nicholi.
All rights reserved.

-----

Usage:

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
