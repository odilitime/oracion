
#include "common.h"

struct bn3f_lexeme _bn3f_lex_opfiniterepeat( FILE * f )
{
	struct bn3f_lexeme r;
	int n;

	r.len   = 0;
	r.type  = BN3F_LEXEME_OPFINITEREPEAT;
	r.abort = 0;

	n = fgetc( f );

	if(n != '{')
	{
		fseek( f, -1, SEEK_CUR );

		return r;
	}

	r.len += 1;

	for(;;)
	{
		n = fgetc( f );

		r.len += 1;

		if(n < '0' || n > '9')
		{
			r.abort = 1;

			break;
		}
		else if(n == '}')
		{
			break;
		}
	}

	return r;
}
