
#include "minihn.h"

enum
{
	/* single-dash short flags and double-dash long flags are used. */
	GETOPT2_MODEOPT_MASK_FLAGS = 0x0,
	/* parameters (think file paths) end-to-start are used. */
	GETOPT2_MODEOPT_MASK_ENDPARAMS = 0x2,
	/* command verbs heading the command invocation are used. */
	GETOPT2_MODEOPT_MASK_VERBS = 0x4,
	/* one dash for stdin; two for flag parser termination. */
	GETOPT2_MODEOPT_MASK_DASHES = 0x8,
	/* mask of all reserved bits (must all be LOW). */
	GETOPT2_MODEOPT_MASK_RESERVED = 0xF0
};

struct getopt2_args
{
	ptri argc;
	chr ** argv;
};

struct getopt2_flagparam
{
	bl found;
	chr * value;
};

struct getopt2_args getopt2_normalise( int, char ** );

struct getopt2_flagparam getopt2_getflag( u8, chr,
	chr *, struct getopt2_args );

chr * getopt2_getverb( u8, struct getopt2_args );

chr * getopt2_getparam( u8, ptri, struct getopt2_args );
