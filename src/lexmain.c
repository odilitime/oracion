
#include "common.h"

struct bn3f_lexeme ** bn3f_lex( FILE * f )
{
	int r;
	ptri streamoffs = 0;
	struct bn3f_lexeme ** lexemes;
	ptri lexemes_sz = 16;

	r = fseek( f, 0, SEEK_SET );

	if(r)
	{
		fprintf( stderr,
		"Error: file stream given is not seekable (are you using stdin?)\n" );
	}

	lexemes = calloc( lexemes_sz, sizeof(struct bn3f_lexeme *) );

	for(;;)
	{
		r = _bn3f_lex_loopiter( f, &streamoffs, &lexemes, &lexemes_sz );

		if(r)
		{
			break;
		}
	}

	return lexemes;
}
