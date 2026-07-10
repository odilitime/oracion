
#include "common.h"

static int _ident_startchar( int c )
{
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}

static int _ident_char( int c )
{
	return (c >= '0' && c <= '9') || _ident_startchar( c );
}

struct bn3f_lexeme _bn3f_lex_identifier( FILE * f, ptri streamoffs )
{
	struct bn3f_lexeme r;
	int n;

	r.start = streamoffs;
	r.end   = streamoffs;
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
	r.end += 1;

	for(;;)
	{
		n = fgetc( f );

		r.len++;
		r.end++;

		/* WHY: break (not fseek) — the EOF byte itself was never
		 * consumed, so the stream position is already correct */
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
	}

	return r;
}
