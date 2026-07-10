
#include "common.h"

struct bn3f_lexeme _bn3f_lex_ophollowrepeat( FILE * f, ptri streamoffs )
{
	struct bn3f_lexeme r;
	int n;

	r.start = streamoffs;
	r.end   = streamoffs;
	r.len   = 0;
	r.type  = BN3F_LEXEME_OPHOLLOWREPEAT;
	r.abort = 0;

	n = fgetc( f );

	if(n == EOF)
	{
		return r;
	}

	if(n != '*')
	{
		fseek( f, -1, SEEK_CUR );

		return r;
	}

	r.len += 1;
	r.end += 1;

	return r;
}
