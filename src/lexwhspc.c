
#include "common.h"

struct bn3f_lexeme _bn3f_lex_whitespace( FILE * f )
{
	struct bn3f_lexeme r;
	int n;

	r.len   = 0;
	r.type  = BN3F_LEXEME_WHITESPACE;
	r.abort = 0;

	n = fgetc( f );

	/* WHY: see lexcomnt.c for why EOF must be checked before any
	 * fseek( -1 ) rewind on mismatch */
	if(n == EOF)
	{
		return r;
	}

	if(n != '\t' && n != '\n' && n != '\v'
	&& n != '\f' && n != '\r' && n != ' ')
	{
		fseek( f, -1, SEEK_CUR );

		return r;
	}

	/* count the previous character since it succeeded */
	r.len += 1;

	for(;;)
	{
		n = fgetc( f );

		/* WHY: break (not fseek) — the EOF byte itself was never
		 * consumed, so the stream position is already correct */
		if(n == EOF)
		{
			break;
		}

		if(n != '\t' && n != '\n' && n != '\v'
		&& n != '\f' && n != '\r' && n != ' ')
		{
			fseek( f, -1, SEEK_CUR );

			break;
		}

		r.len++;
	}

	return r;
}
