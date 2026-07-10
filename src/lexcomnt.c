
#include "common.h"

struct bn3f_lexeme _bn3f_lex_comment( FILE * f, ptri streamoffs )
{
	struct bn3f_lexeme r;
	int n[2];

	r.start = streamoffs;
	r.end   = streamoffs;
	r.len   = 0;
	r.type  = BN3F_LEXEME_COMMENT;
	r.abort = 0;

	n[0] = fgetc( f );

	/* WHY: fgetc( ) does not consume a byte when the stream is already
	 * at EOF, so there is nothing to push back; falling through to the
	 * fseek( ) below would rewind onto the last real byte instead and
	 * cause this scanner (and the outer lex loop) to spin forever */
	if(n[0] == EOF)
	{
		return r;
	}

	if(n[0] != '/')
	{
		fseek( f, -1, SEEK_CUR );

		return r;
	}

	n[1] = fgetc( f );

	if(n[1] == EOF || n[1] != '*')
	{
		fseek( f, -2, SEEK_CUR );

		return r;
	}

	/* default state is meaningless to the logic of the loop that follows */
	n[0] = '\0';
	n[1] = '\0';

	/* count the opening characters */
	r.len += 2;
	r.end += 2;

	for(;;)
	{
		n[r.len & 1] = fgetc( f );

		if(n[r.len & 1] == EOF)
		{
			r.abort = 1;

			return r;
		}

		if(n[r.len & 1] == '/' && n[(r.len - 1) & 1] == '*')
		{
			r.len++;
			r.end++;

			break;
		}

		r.len++;
		r.end++;
	}

	return r;
}
