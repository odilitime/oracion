
#include "getopt2.h"

static ptri _strlen( chr * a )
{
	ptri r = 0;

	for(r = 0; a[r] != '\0'; ++r);

	return r;
}

static ptri _strleneqsign( chr * a )
{
	ptri r = 0;

	for(r = 0; a[r] != '\0' && a[r] != '='; ++r);

	return r;
}

static bl _strequ( chr * a, chr * b, ptri a_len, ptri b_len )
{
	ptri i;

	if(a_len != b_len)
	{
		return FALSE;
	}

	for(i = 0; i < a_len && a[i] == b[i]; ++i);

	return i >= a_len;
}

struct getopt2_args getopt2_normalise( int argc, char ** argv )
{
	struct getopt2_args r;

	r.argc = (ptri)argc;
	r.argv = (chr **)argv;

	return r;
}

static bl _islflag( chr * str )
{
	return str[0] == '-' && str[1] == '-' && str[2] != '\0';
}

static bl _isfstop( chr * str )
{
	return str[0] == '-' && str[1] == '-' && str[2] == '\0';
}

static bl _issflag( chr * str )
{
	return str[0] == '-' && str[1] != '-' && str[1] != '\0';
}

static bl _isstdin( chr * str )
{
	return str[0] == '-' && str[1] == '\0';
}

static chr * _getflagval(
	struct getopt2_args args,
	ptri i,
	bl equsign )
{
	const ptri sz = _strleneqsign( args.argv[i] );

	/* additional checking is needed as we may perform lookahead */
	if((equsign && i >= args.argc) || (!equsign && i + 1 >= args.argc))
	{
		return NULL;
	}

	/* several conditions are checked:
	 *  1. if equal sign flag-value delimiting is sought
	 *  2. if the equal sign is actually present in the flag
	 * if these hold, it returns an in-place substring of the value */
	if(equsign && args.argv[i][sz] == '=')
	{
		/* advance past the '=' and return that as a string */
		return &(args.argv[i][sz + 1]);
	}

	/* if we reach HERE, it means either equal sign delimiting isn't
	 * used, or that there was no equal sign detected if requested */

	/* return the next parameter as the value, if it is present.
	 * this function assumes the parameter is required, i.e. it does NOT
	 * check high-level flag structure to see if the flag given needs
	 * one */
	return i + 1 < args.argc
		? args.argv[i + 1]
		: NULL;
}

static bl _hnd_lflag(
	getopt2_modeopt_t modeopt,
	getopt2_flagopt_t flagopt,
	struct getopt2_flag flag,
	struct getopt2_args args,
	ptri i,
	struct getopt2_flagparam * ret )
{
	if(!_strequ( &(args.argv[i][2]), flag.l,
	(modeopt & GETOPT2_MODEOPT_MASK_LFLAGEQ)
		? _strleneqsign( &(args.argv[i][2]) )
		: _strlen( &(args.argv[i][2] ) ),
	_strlen( flag.l ) )
	|| !(modeopt & GETOPT2_MODEOPT_MASK_FLAGS))
	{
		return FALSE;
	}

	ret->found = 1;
	ret->afterverb = (modeopt & GETOPT2_MODEOPT_MASK_VERBS) && i > 1
		? 1 : 0;
	ret->value = _getflagval( args, i, TRUE );

	return TRUE;
}

static bl _hnd_sflag(
	getopt2_modeopt_t modeopt,
	getopt2_flagopt_t flagopt,
	struct getopt2_flag flag,
	struct getopt2_args args,
	ptri i,
	struct getopt2_flagparam * ret )
{
	const ptri sz = _strlen( args.argv[i] );
	ptri j;

	/* j = 1 to skip initial '-' character */
	for(j = 1; j < sz; ++j)
	{
		if(args.argv[i][j] == flag.s)
		{
			if((flagopt & GETOPT2_FLAGOPT_MASK_REQDVAL)
			&& j + 1 < sz)
			{
				return TRUE;
			}

			ret->found = 1;
			ret->afterverb = (modeopt & GETOPT2_MODEOPT_MASK_VERBS)
				&& i > 1 ? 1 : 0;

			return TRUE;
		}
	}

	return FALSE;
}

struct getopt2_flagparam getopt2_getflag(
	getopt2_modeopt_t modeopt,
	getopt2_flagopt_t flagopt,
	struct getopt2_flag flag,
	struct getopt2_args args )
{
	ptri i;
	struct getopt2_flagparam ret;

	ret.found = 0;
	ret.afterverb = 0;
	ret.value = NULL;

	if(!(modeopt & GETOPT2_MODEOPT_MASK_FLAGS)
	|| (modeopt & GETOPT2_MODEOPT_MASK_RESERVED)
	|| (flagopt & GETOPT2_FLAGOPT_MASK_RESERVED))
	{
		return ret;
	}

	/* i = 1 to skip program name, argv[0] */
	for(i = 1; i < args.argc; ++i)
	{
		const bl hit = _islflag( args.argv[i] )
			? _hnd_lflag( modeopt, flagopt, flag, args, i, &ret )
			: _issflag( args.argv[i] )
				? _hnd_sflag( modeopt, flagopt, flag, args, i, &ret )
				: 0;

		if(hit)
		{
			break;
		}
	}
}
