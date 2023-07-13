
#include "common.h"

struct bn3f_lexeme _bn3f_lex_opterminate( FILE * f )
{
	struct bn3f_lexeme r;
	int n;

	r.len   = 0;
	r.type  = BN3F_LEXEME_OPTERMINATE;
	r.abort = 0;

	n = fgetc( f );

	if(n != ';')
	{
		fseek( f, -1, SEEK_CUR );

		return r;
	}

	r.len += 1;

	return r;
}
