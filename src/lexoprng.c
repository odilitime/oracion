
#include "common.h"

struct bn3f_lexeme _bn3f_lex_oprange( FILE * f )
{
	struct bn3f_lexeme r;
	int n;
	ptri i;

	r.len   = 0;
	r.type  = BN3F_LEXEME_OPRANGE;
	r.abort = 0;

	for(i = 0; i < 3; ++i)
	{
		n = fgetc( f );

		if(n == EOF)
		{
			/* WHY: EOF is not consumed, so only rewind the dots that
			 * were actually read (i of them). Seeking -(i+1) would
			 * walk one byte before the first '.' */
			if(i > 0)
			{
				fseek( f, -(long)i, SEEK_CUR );
			}

			return r;
		}

		if(n != '.')
		{
			fseek( f, -(long)(i + 1), SEEK_CUR );

			return r;
		}
	}

	r.len += 3;

	return r;
}
