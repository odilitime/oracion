
#include "common.h"

struct bn3f_lexeme _bn3f_lex_characterlit( FILE * f, ptri streamoffs )
{
	struct bn3f_lexeme r;
	int n, esc;

	r.start = streamoffs;
	r.end   = streamoffs;
	r.len   = 0;
	r.type  = BN3F_LEXEME_CHARACTERLIT;
	r.abort = 0;

	n = fgetc( f );

	if(n == EOF)
	{
		return r;
	}

	if(n != '\'')
	{
		fseek( f, -1, SEEK_CUR );

		return r;
	}

	r.len += 1;
	r.end += 1;
	esc    = 0;

	for(;;)
	{
		n = fgetc( f );

		r.len++;
		r.end++;

		if(n == '\\')
		{
			esc = ~esc & 1;
		}
		else if(n == '\'' && !esc)
		{
			break;
		}
		else if(n == EOF)
		{
			/* fgetc( ) does not consume a byte at EOF, so the stream
			 * position is already correct; undo the speculative
			 * increment above without seeking */
			r.abort = 1;
			r.len--;
			r.end--;

			break;
		}

		/* WHY: see lexstrin.c — `esc` must be cleared after any
		 * non-backslash character or one escape sequence anywhere in
		 * the literal would make every later quote look escaped */
		if(n != '\\')
		{
			esc = 0;
		}
	}

	return r;
}
