
#include "common.h"

struct bn3f_lexeme _bn3f_lex_oprange( FILE * f, ptri streamoffs )
{
	struct bn3f_lexeme r;
	int n;
	ptri i;

	r.start = streamoffs;
	r.end   = streamoffs;
	r.len   = 0;
	r.type  = BN3F_LEXEME_OPRANGE;
	r.abort = 0;

	for(i = 0; i < 3; ++i)
	{
		n = fgetc( f );

		if(i == 0 && n == EOF)
		{
			return r;
		}

		if(n != '.')
		{
			fseek( f, -(1 + i), SEEK_CUR );

			return r;
		}
	}

	r.len += 3;
	r.end += 3;

	return r;
}
