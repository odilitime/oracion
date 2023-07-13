
#include "common.h"

ssize_t _find_slot( struct bn3f_lexeme ** lexemes, size_t lexemes_sz )
{
	size_t i;

	for(i = 0; i < lexemes_sz; ++i)
	{
		if(lexemes[i] == NULL)
		{
			return i;
		}
	}

	return -1;
}

int8_t _bn3f_lex_loopiter( FILE * f, size_t * streamoffs,
struct bn3f_lexeme *** lexemes, size_t * lexemes_sz )
{
	struct bn3f_lexeme l;
	ssize_t i;
	int hit;

	for(i = 0, hit = 0; i < BN3F_MAX_LEXEME; ++i)
	{
		const int c = fgetc( f );

		fprintf( stderr, "pos=%llu ch0='%c' (%i) fn=%s ... ", *streamoffs, c,
			c, _dbg_lexemes[i] );
		fflush( stderr );

		ungetc( c, f );

		l = _bn3f_scan[i]( f );

		if(l.len > 0)
		{
			hit = 1;
			*streamoffs += l.len;

			fprintf( stderr, "hit!\n" );

			break;
		}

		fprintf( stderr, "nope.\n" );
	}

	if(!hit)
	{
		/* 1 for EOF, 2 for error, 3 otherwise */
		return feof( f ) ? 1 : ferror( f ) ? 2 : 3;
	}

	i = _find_slot( *lexemes, *lexemes_sz );

	/* allocate space if necessary */
	if(i == -1)
	{
		*lexemes = realloc( *lexemes,
			sizeof(*lexemes) * ((*lexemes_sz) << 1) ); /* *= 2 */

		/* zero out the new half of the array */
		memset( (uint8_t *)(*lexemes) + (sizeof(void *) * (*lexemes_sz)), 0,
			*lexemes_sz );

		i = *lexemes_sz;

		*lexemes_sz <<= 1; /* *= 2 */
	}

	(*lexemes)[i] = malloc( sizeof(struct bn3f_lexeme) );

	memcpy( (*lexemes)[i], &l, sizeof l );

	return 0;
}
