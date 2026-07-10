
#include "common.h"

struct bn3f_lexeme _bn3f_lex_opfiniterepeat( FILE * f )
{
	struct bn3f_lexeme r;
	int n;

	r.len   = 0;
	r.type  = BN3F_LEXEME_OPFINITEREPEAT;
	r.abort = 0;

	n = fgetc( f );

	if(n == EOF)
	{
		return r;
	}

	if(n != '{')
	{
		fseek( f, -1, SEEK_CUR );

		return r;
	}

	r.len += 1;

	for(;;)
	{
		n = fgetc( f );

		if(n == EOF)
		{
			r.abort = 1;

			break;
		}

		/* WHY: accept '}' before the non-digit abort path — '}' is
		 * not a digit, so checking digits first permanently rejected
		 * the closing brace and always aborted */
		if(n == '}')
		{
			r.len += 1;

			break;
		}

		if(n >= '0' && n <= '9')
		{
			r.len += 1;

			continue;
		}

		/* mismatch: put the whole tentative token back (including
		 * the bad character) so another scanner can try */
		fseek( f, -(long)(r.len + 1), SEEK_CUR );

		r.len   = 0;
		r.abort = 0;

		break;
	}

	return r;
}
