
#include "common.h"

struct bn3f_lexeme _bn3f_lex_oprange( FILE * f )
{
	struct bn3f_lexeme r;
	int n;
	size_t i;

	r.len   = 0;
	r.type  = BN3F_LEXEME_OPRANGE;
	r.abort = 0;

	for(i = 0; i < 3; ++i)
	{
		n = fgetc( f );

		if(n != '.')
		{
			fseek( f, -(i + 1), SEEK_CUR );

			return r;
		}
	}

	r.len += 3;

	return r;
}
