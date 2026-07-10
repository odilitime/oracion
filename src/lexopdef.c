
#include "common.h"

struct bn3f_lexeme _bn3f_lex_opdefine( FILE * f, ptri streamoffs )
{
	struct bn3f_lexeme r;
	int n;

	r.start = streamoffs;
	r.end   = streamoffs;
	r.len   = 0;
	r.type  = BN3F_LEXEME_OPDEFINE;
	r.abort = 0;

	n = fgetc( f );

	if(n == EOF)
	{
		return r;
	}

	if(n != ':')
	{
		fseek( f, -1, SEEK_CUR );

		return r;
	}

	n = fgetc( f );

	if(n != '=')
	{
		fseek( f, -2, SEEK_CUR );

		return r;
	}

	r.len += 2;
	r.end += 2;

	return r;
}
