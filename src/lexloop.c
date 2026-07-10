
#include "common.h"

static offs _find_slot(
	struct bn3f_lexeme ** lexemes,
	ptri lexemes_sz )
{
	ptri i;

	for(i = 0; i < lexemes_sz; ++i)
	{
		if(lexemes[i] == NULL)
		{
			return i;
		}
	}

	return -1;
}

s8 _bn3f_lex_loopiter(
	FILE * f,
	ptri * streamoffs,
	struct bn3f_lexeme *** lexemes,
	ptri * lexemes_sz )
{
	struct bn3f_lexeme l;
	struct bn3f_lexeme ** grown;
	struct bn3f_lexeme * slot;
	offs i;
	int hit;

	for(i = 0, hit = 0; i < BN3F_MAX_LEXEME; ++i)
	{
		const int c = fgetc( f );

		/* WHY: ungetc( ) must not be passed EOF — that is undefined
		 * behaviour. Bail with the same status codes the !hit path
		 * already uses (1 = EOF, 2 = stream error). */
		if(c == EOF)
		{
			return feof( f ) ? 1 : 2;
		}

		fprintf( stderr, "pos=%lu ch0='%c' (%i) fn=%s ... ",
			(unsigned long)*streamoffs, c, c, _dbg_lexemes[i] );
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
		const ptri oldsz = *lexemes_sz;

		/* WHY: `sizeof(*lexemes)` is the size of one array element,
		 * i.e. a `struct bn3f_lexeme *` (a pointer), NOT the size of
		 * the pointed-to struct. Using sizeof(**lexemes) here would
		 * over-allocate/mis-stride the array and corrupt the heap
		 * once more than the initial capacity is stored. */
		grown = realloc( *lexemes,
			sizeof(*lexemes) * (oldsz << 1) ); /* *= 2 */

		/* WHY: assigning realloc's NULL failure directly onto
		 * *lexemes would leak the old buffer and then crash in
		 * memset below. Leave *lexemes intact so the caller can
		 * free the partial results per bn3f_lex( )'s contract. */
		if(grown == NULL)
		{
			return 2;
		}

		*lexemes = grown;

		/* zero out the new half of the array (in pointer-sized units)
		 * so that _find_slot( ) can keep using NULL as "empty" */
		memset( (u8 *)(*lexemes) + (sizeof(*lexemes) * oldsz), 0,
			sizeof(*lexemes) * oldsz );

		i = oldsz;

		*lexemes_sz = oldsz << 1; /* *= 2 */
	}

	slot = malloc( sizeof(struct bn3f_lexeme) );

	if(slot == NULL)
	{
		return 2;
	}

	(*lexemes)[i] = slot;

	memcpy( (*lexemes)[i], &l, sizeof l );

	return 0;
}
