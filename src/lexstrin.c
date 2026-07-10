
#include "common.h"

struct bn3f_lexeme _bn3f_lex_stringlit( FILE * f )
{
	struct bn3f_lexeme r;
	int n, esc;

	r.len   = 0;
	r.type  = BN3F_LEXEME_STRINGLIT;
	r.abort = 0;

	n = fgetc( f );

	if(n == EOF)
	{
		return r;
	}

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

		if(n == EOF)
		{
			/* fgetc( ) does not consume a byte at EOF, so the stream
			 * position is already correct — do not count it in len */
			r.abort = 1;

			break;
		}

		r.len++;

		if(n == '\\')
		{
			esc = ~esc & 1;
		}
		else if(n == '"' && !esc)
		{
			break;
		}

		/* WHY: `esc` must be cleared after any non-backslash character,
		 * not just left toggled by the last '\\' seen. Without this,
		 * a single escape sequence anywhere in the string would leave
		 * every subsequent '"' treated as escaped, so the scanner
		 * would consume the rest of the file looking for a close
		 * quote that it would never accept */
		if(n != '\\')
		{
			esc = 0;
		}
	}

	return r;
}
