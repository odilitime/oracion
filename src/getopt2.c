
#include "getopt2.h"

struct getopt2_args getopt2_normalise( int argc, char ** argv )
{
	struct getopt2_args r;

	r.argc = (ptri)argc;
	r.argv = (chr **)argv;

	return r;
}

struct getopt2_flagparam getopt2_getflag(
	getopt2_modeopt_t modeopt,
	getopt2_flagopt_t flagopt,
	struct getopt2_flag flag,
	struct getopt2_args args )
{
	ptri i;

	for(i = 0; i < args.argc; ++i)
	{
	}
}
