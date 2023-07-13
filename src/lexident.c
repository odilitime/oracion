
#include "common.h"

static int _ident_startchar( int c )
{
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}

static int _ident_char( int c )
{
	return (c >= '0' && c <= '9') || _ident_startchar( c );
}

struct bn3f_lexeme _bn3f_lex_identifier( FILE * f )
{
	struct bn3f_lexeme r;
	int n;

	r.len   = 0;
	r.type  = BN3F_LEXEME_IDENTIFIER;
	r.abort = 0;

	n = fgetc( f );

	if(!_ident_startchar( n ))
	{
		fseek( f, -1, SEEK_CUR );

		return r;
	}

	r.len += 1;

	for(;;)
	{
		n = fgetc( f );

		r.len++;

		if(!_ident_char( n ))
		{
			/* put the mismatch back so it can match somewhere else */
			fseek( f, -1, SEEK_CUR );

			break;
		}
	}

	return r;
}
