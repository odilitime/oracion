
#include "common.h"

struct bn3f_lexeme _bn3f_lex_stringlit( FILE * f )
{
	struct bn3f_lexeme r;
	int n, esc;

	r.len   = 0;
	r.type  = BN3F_LEXEME_STRINGLIT;
	r.abort = 0;

	n = fgetc( f );

	if(n != '"')
	{
		fseek( f, -1, SEEK_CUR );

		return r;
	}

	r.len += 1;
	esc    = 0;

	for(;;)
	{
		n = fgetc( f );

		r.len++;

		if(n == '\\')
		{
			esc = ~esc & 1;
		}
		else if(n == '"' && !esc)
		{
			break;
		}
		else if(n == EOF)
		{
			r.abort = 1;
			r.len--;

			fseek( f, -1, SEEK_CUR );

			break;
		}
	}

	return r;
}
