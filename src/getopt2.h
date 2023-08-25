
#include "minihn.h"

typedef u8 modeopt_t;

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
	/* whether the form --flag=value is recognised. */
	GETOPT2_MODEOPT_MASK_LFLAGEQ = 0x10,
	/* whether short flags stack together -likeso. */
	GETOPT2_MODEOPT_MASK_SFLAGSTACK = 0x20,
	/* whether successive flags override previous ones left-to-right
	 * (it is otherwise treated as an error to have multiple conflicting
	 * flags) */
	GETOPT2_MODEOPT_MASK_LTROVR = 0x40,
	/* mask of all reserved bits (must all be LOW). */
	GETOPT2_MODEOPT_MASK_RESERVED = 0x80
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

struct getopt2_args getopt2_normalise( int argc, char ** argv )
{
	struct getopt2_args r;

	r.argc = (ptri)argc;
	r.argv = (chr **)argv;

	return r;
}

struct getopt2_flagparam getopt2_getflag( modeopt_t modeopt, chr sflag,
	chr * lflag, struct getopt2_args args )
{
	ptri i;

	for(i = 0; i < args.argc; ++i)
	{}
}

chr * getopt2_getverb( u8, struct getopt2_args );

chr * getopt2_getparam( u8, ptri, struct getopt2_args );
