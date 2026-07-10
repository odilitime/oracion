
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

	/* WHY: see lexcomnt.c for why EOF must be checked before any
	 * fseek( -1 ) rewind on mismatch */
	if(n == EOF)
	{
		return r;
	}

	if(!_ident_startchar( n ))
	{
		fseek( f, -1, SEEK_CUR );

		return r;
	}

	r.len += 1;

	for(;;)
	{
		n = fgetc( f );

		/* WHY: check EOF / mismatch before bumping len — fgetc( )
		 * does not consume a byte at EOF, so counting it would make
		 * len overshoot the real token width */
		if(n == EOF)
		{
			break;
		}

		if(!_ident_char( n ))
		{
			/* put the mismatch back so it can match somewhere else */
			fseek( f, -1, SEEK_CUR );

			break;
		}

		r.len++;
	}

	return r;
}
