
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum
{
	BUF_SZ = 65536
};

enum
{
	LEXEME_COMMENT,
	LEXEME_WHITESPACE,
	LEXEME_STRINGLIT,
	LEXEME_CHARACTERLIT,
	LEXEME_IDENTIFIER,
	LEXEME_OP_DEFINE,
	LEXEME_OP_TERMINATE,
	LEXEME_OP_GROUPOPEN,
	LEXEME_OP_GROUPCLOSE,
	LEXEME_OP_RANGE,
	LEXEME_OP_SOLIDREPEAT,
	LEXEME_OP_HOLLOWREPEAT,
	LEXEME_OP_OPTION,
	LEXEME_OP_FINITEREPEAT,
	LEXEME_OP_ALTERNATE,
	MAX_LEXEME
};

typedef uint8_t bn3f_lexeme_t;
typedef uint16_t bn3f_bufoffs_t;
typedef uint64_t bn3f_streamoffs_t;

struct bn3f_lexeme
{
	bn3f_lexeme_t type;
	bn3f_streamoffs_t start, end;
};

typedef struct bn3f_lexeme (*bn3f_lexeme_scan_f)( FILE *,
bn3f_streamoffs_t );

static struct bn3f_lexeme bn3f_lex_eof( FILE *, bn3f_streamoffs_t );
static struct bn3f_lexeme bn3f_lex_comment( FILE *, bn3f_streamoffs_t );
static struct bn3f_lexeme bn3f_lex_whitespace( FILE *, bn3f_streamoffs_t );
static struct bn3f_lexeme bn3f_lex_stringlit( FILE *, bn3f_streamoffs_t );
static struct bn3f_lexeme bn3f_lex_characterlit( FILE *, bn3f_streamoffs_t );
static struct bn3f_lexeme bn3f_lex_identifier( FILE *, bn3f_streamoffs_t );
static struct bn3f_lexeme bn3f_lex_op_define( FILE *, bn3f_streamoffs_t );
static struct bn3f_lexeme bn3f_lex_op_terminate( FILE *, bn3f_streamoffs_t );
static struct bn3f_lexeme bn3f_lex_op_groupopen( FILE *, bn3f_streamoffs_t );
static struct bn3f_lexeme bn3f_lex_op_groupclose( FILE *, bn3f_streamoffs_t );
static struct bn3f_lexeme bn3f_lex_op_range( FILE *, bn3f_streamoffs_t );
static struct bn3f_lexeme bn3f_lex_op_solidrepeat( FILE *, bn3f_streamoffs_t );
static struct bn3f_lexeme bn3f_lex_op_hollowrepeat( FILE *,
bn3f_streamoffs_t );
static struct bn3f_lexeme bn3f_lex_op_option( FILE *, bn3f_streamoffs_t );
static struct bn3f_lexeme bn3f_lex_op_finiterepeat( FILE *,
bn3f_streamoffs_t );
static struct bn3f_lexeme bn3f_lex_op_alternate( FILE *, bn3f_streamoffs_t );

static const char * const _dbg_lexemes[MAX_LEXEME] = {
	"LEXEME_COMMENT",
	"LEXEME_WHITESPACE",
	"LEXEME_STRINGLIT",
	"LEXEME_CHARACTERLIT",
	"LEXEME_IDENTIFIER",
	"LEXEME_OP_DEFINE",
	"LEXEME_OP_TERMINATE",
	"LEXEME_OP_GROUPOPEN",
	"LEXEME_OP_GROUPCLOSE",
	"LEXEME_OP_RANGE",
	"LEXEME_OP_SOLIDREPEAT",
	"LEXEME_OP_HOLLOWREPEAT",
	"LEXEME_OP_OPTION",
	"LEXEME_OP_FINITEREPEAT",
	"LEXEME_OP_ALTERNATE"
};

static bn3f_lexeme_scan_f bn3f_lexeme_scan[MAX_LEXEME] = {
	bn3f_lex_comment,
	bn3f_lex_whitespace,
	bn3f_lex_stringlit,
	bn3f_lex_characterlit,
	bn3f_lex_identifier,
	bn3f_lex_op_define,
	bn3f_lex_op_terminate,
	bn3f_lex_op_groupopen,
	bn3f_lex_op_groupclose,
	bn3f_lex_op_range,
	bn3f_lex_op_solidrepeat,
	bn3f_lex_op_hollowrepeat,
	bn3f_lex_op_option,
	bn3f_lex_op_finiterepeat,
	bn3f_lex_op_alternate
};

static struct bn3f_lexeme bn3f_lex_comment( FILE * f,
bn3f_streamoffs_t streamoffs )
{
	struct bn3f_lexeme ret;
	/* our two character state buffer */
	int n[2];
	/* this is used for counting the comment along
	 * its LSB is also used to index the state buffer above */
	bn3f_streamoffs_t i;

	ret.type = LEXEME_COMMENT;
	ret.start = streamoffs;
	ret.end = streamoffs;

	n[0] = fgetc( f );

	if(n[0] == EOF)
	{
		return ret;
	}

	if(n[0] != '/')
	{
		fseek( f, -1, SEEK_CUR );

		return ret;
	}

	n[1] = fgetc( f );

	if(n[1] == EOF || n[1] != '*')
	{
		fseek( f, -2, SEEK_CUR );

		return ret;
	}

	/* default state is meaningless to the logic of the loop that follows */
	n[0] = '\0';
	n[1] = '\0';

	/* count the first two characters */
	i = 2;

	for(;;)
	{
		n[i & 1] = fgetc( f );

		if(n[i & 1] == EOF)
		{
			/* stop. back track */
			fseek( f, -i, SEEK_CUR );

			/* make them match so the scanner knows it's not a hit */
			ret.end = ret.start;

			return ret;
		}

		if(n[(i - 1) & 1] == '*' && n[i & 1] == '/')
		{
			i++;

			break;
		}

		i++;
	}

	ret.end += i;

	return ret;
}

static struct bn3f_lexeme bn3f_lex_whitespace( FILE * f,
bn3f_streamoffs_t streamoffs )
{
	struct bn3f_lexeme ret;
	int n;

	ret.type = LEXEME_WHITESPACE;
	ret.start = streamoffs;
	ret.end = streamoffs;

	n = fgetc( f );

	if(n == EOF)
	{
		return ret;
	}

	if(n != '\t' && n != '\n' && n != '\v' && n != '\f' &&
	n != '\r' && n != ' ')
	{
		/* we have to put the stream back if it failed */
		fseek( f, -1, SEEK_CUR );

		return ret;
	}

	/* so the above succeeded */
	ret.end++;

	for(;;)
	{
		n = fgetc( f );

		if(n == EOF)
		{
			break;
		}

		if(n != '\t' && n != '\n' && n != '\v' && n != '\f' &&
		n != '\r' && n != ' ')
		{
			/* not ours, put it back */
			fseek( f, -1, SEEK_CUR );

			break;
		}

		ret.end++;
	}

	return ret;
}

static struct bn3f_lexeme bn3f_lex_stringlit( FILE * f,
bn3f_streamoffs_t streamoffs )
{
	struct bn3f_lexeme ret;
	int n;
	unsigned backslash;
	bn3f_streamoffs_t i;

	ret.type = LEXEME_STRINGLIT;
	ret.start = streamoffs;
	ret.end = streamoffs;

	n = fgetc( f );

	if(n == EOF)
	{
		return ret;
	}

	if(n != '"')
	{
		fseek( f, -1, SEEK_CUR );

		return ret;
	}

	/* count the first character */
	i = 1;
	backslash = 0;

	for(;;)
	{
		n = fgetc( f );

		if(n == '\\')
		{
			backslash = (~backslash) & 1;
		}
		else if(n == '"' && !backslash)
		{
			i++;

			break;
		}
		else if(n == EOF)
		{
			/* string did not end cleanly */
			fseek( f, -i, SEEK_CUR );

			return ret;
		}

		if(n != '\\')
		{
			backslash = 0;
		}

		i++;
	}

	ret.end += i;

	return ret;
}

static struct bn3f_lexeme bn3f_lex_characterlit( FILE * f,
bn3f_streamoffs_t streamoffs )
{
	struct bn3f_lexeme ret;
	int n;
	unsigned backslash;
	bn3f_streamoffs_t i;

	ret.type = LEXEME_CHARACTERLIT;
	ret.start = streamoffs;
	ret.end = streamoffs;

	n = fgetc( f );

	if(n == EOF)
	{
		return ret;
	}

	if(n != '\'')
	{
		fseek( f, -1, SEEK_CUR );

		return ret;
	}

	/* count the first character */
	i = 1;
	backslash = 0; /* even, 1 = odd */

	for(;;)
	{
		n = fgetc( f );

		if(n == '\\')
		{
			backslash = (~backslash) & 1;
		}
		else if(n == '\'' && !backslash)
		{
			i++;

			break;
		}
		else if(n == EOF)
		{
			/* string did not end cleanly */
			fseek( f, -i, SEEK_CUR );

			return ret;
		}

		if(n != '\\')
		{
			backslash = 0;
		}

		i++;
	}

	ret.end += i;

	return ret;
}

static struct bn3f_lexeme bn3f_lex_identifier( FILE * f,
bn3f_streamoffs_t streamoffs )
{
	struct bn3f_lexeme ret;
	int n;

	ret.type = LEXEME_IDENTIFIER;
	ret.start = streamoffs;
	ret.end = streamoffs;

	n = fgetc( f );

	if(n == EOF)
	{
		return ret;
	}

	if(!((n >= 'A' && n <= 'Z') || (n >= 'a' && n <= 'z') || n == '_'))
	{
		/* we have to put the stream back if it failed */
		fseek( f, -1, SEEK_CUR );

		return ret;
	}

	/* so the above succeeded */
	ret.end++;

	for(;;)
	{
		n = fgetc( f );
		ret.end++;

		if(n == EOF)
		{
			break;
		}

		if(!((n >= 'A' && n <= 'Z') ||
		(n >= 'a' && n <= 'z') || (n >= '0' && n <= '9') || n == '_'))
		{
			/* not ours, put it back */
			fseek( f, -1, SEEK_CUR );

			break;
		}
	}

	return ret;
}

static struct bn3f_lexeme bn3f_lex_op_define( FILE * f,
bn3f_streamoffs_t streamoffs )
{
	struct bn3f_lexeme ret;
	int n;

	ret.type = LEXEME_OP_DEFINE;
	ret.start = streamoffs;
	ret.end = streamoffs;

	n = fgetc( f );

	if(n == EOF)
	{
		return ret;
	}

	if(n != ':')
	{
		fseek( f, -1, SEEK_CUR );

		return ret;
	}

	n = fgetc( f );

	if(n != '=')
	{
		fseek( f, -2, SEEK_CUR );

		return ret;
	}

	ret.end += 2;

	return ret;
}

static struct bn3f_lexeme bn3f_lex_op_terminate( FILE * f,
bn3f_streamoffs_t streamoffs )
{
	struct bn3f_lexeme ret;
	int n;

	ret.type = LEXEME_OP_TERMINATE;
	ret.start = streamoffs;
	ret.end = streamoffs;

	n = fgetc( f );

	if(n == EOF)
	{
		return ret;
	}

	if(n != ';')
	{
		fseek( f, -1, SEEK_CUR );

		return ret;
	}

	ret.end++;

	return ret;
}

static struct bn3f_lexeme bn3f_lex_op_groupopen( FILE * f,
bn3f_streamoffs_t streamoffs )
{
	struct bn3f_lexeme ret;
	int n;

	ret.type = LEXEME_OP_GROUPOPEN;
	ret.start = streamoffs;
	ret.end = streamoffs;

	n = fgetc( f );

	if(n == EOF)
	{
		return ret;
	}

	if(n != '(')
	{
		fseek( f, -1, SEEK_CUR );

		return ret;
	}

	ret.end++;

	return ret;
}

static struct bn3f_lexeme bn3f_lex_op_groupclose( FILE * f,
bn3f_streamoffs_t streamoffs )
{
	struct bn3f_lexeme ret;
	int n;

	ret.type = LEXEME_OP_GROUPCLOSE;
	ret.start = streamoffs;
	ret.end = streamoffs;

	n = fgetc( f );

	if(n == EOF)
	{
		return ret;
	}

	if(n != ')')
	{
		fseek( f, -1, SEEK_CUR );

		return ret;
	}

	ret.end++;

	return ret;
}

static struct bn3f_lexeme bn3f_lex_op_range( FILE * f,
bn3f_streamoffs_t streamoffs )
{
	struct bn3f_lexeme ret;
	int n;
	bn3f_streamoffs_t i;

	ret.type = LEXEME_OP_RANGE;
	ret.start = streamoffs;
	ret.end = streamoffs;

	for(i = 0; i < 3; ++i)
	{
		n = fgetc( f );

		if(i == 0 && n == EOF)
		{
			return ret;
		}

		if(n != '.')
		{
			fseek( f, -(1 + i), SEEK_CUR );

			return ret;
		}
	}

	ret.end += i;

	return ret;
}

static struct bn3f_lexeme bn3f_lex_op_solidrepeat( FILE * f,
bn3f_streamoffs_t streamoffs )
{
	struct bn3f_lexeme ret;
	int n;

	ret.type = LEXEME_OP_SOLIDREPEAT;
	ret.start = streamoffs;
	ret.end = streamoffs;

	n = fgetc( f );

	if(n == EOF)
	{
		return ret;
	}

	if(n != '+')
	{
		fseek( f, -1, SEEK_CUR );

		return ret;
	}

	ret.end++;

	return ret;
}

static struct bn3f_lexeme bn3f_lex_op_hollowrepeat( FILE * f,
bn3f_streamoffs_t streamoffs )
{
	struct bn3f_lexeme ret;
	int n;

	ret.type = LEXEME_OP_HOLLOWREPEAT;
	ret.start = streamoffs;
	ret.end = streamoffs;

	n = fgetc( f );

	if(n == EOF)
	{
		return ret;
	}

	if(n != '*')
	{
		fseek( f, -1, SEEK_CUR );

		return ret;
	}

	ret.end++;

	return ret;
}

static struct bn3f_lexeme bn3f_lex_op_option( FILE * f,
bn3f_streamoffs_t streamoffs )
{
	struct bn3f_lexeme ret;
	int n;

	ret.type = LEXEME_OP_OPTION;
	ret.start = streamoffs;
	ret.end = streamoffs;

	n = fgetc( f );

	if(n == EOF)
	{
		return ret;
	}

	if(n != '?')
	{
		fseek( f, -1, SEEK_CUR );

		return ret;
	}

	ret.end++;

	return ret;
}

static struct bn3f_lexeme bn3f_lex_op_finiterepeat( FILE * f,
bn3f_streamoffs_t streamoffs )
{
	struct bn3f_lexeme ret;
	int n;
	bn3f_streamoffs_t i;

	ret.type = LEXEME_OP_FINITEREPEAT;
	ret.start = streamoffs;
	ret.end = streamoffs;

	n = fgetc( f );

	if(n == EOF)
	{
		return ret;
	}

	if(n != '{')
	{
		fseek( f, -1, SEEK_CUR );

		return ret;
	}

	/* counting the opening brace */
	i = 1;

	for(;;)
	{
		n = fgetc( f );

		if(n >= '0' && n <= '9')
		{
			i++;

			continue;
		}
		else if(n == '}')
		{
			i++;

			break;
		}
		else
		{
			fseek( f, -(i + 1), SEEK_CUR );

			return ret;
		}
	}

	ret.end += i;

	return ret;
}

static struct bn3f_lexeme bn3f_lex_op_alternate( FILE * f,
bn3f_streamoffs_t streamoffs )
{
	struct bn3f_lexeme ret;
	int n;

	ret.type = LEXEME_OP_ALTERNATE;
	ret.start = streamoffs;
	ret.end = streamoffs;

	n = fgetc( f );

	if(n == EOF)
	{
		return ret;
	}

	if(n != '|')
	{
		fseek( f, -1, SEEK_CUR );

		return ret;
	}

	ret.end++;

	return ret;
}

static unsigned bn3f_lex_scanloop( FILE * f, bn3f_streamoffs_t * streamoffs,
struct bn3f_lexeme ** lexemes, size_t * lexemes_sz, size_t * lexemes_cap )
{
	struct bn3f_lexeme lexeme;
	unsigned i;
	unsigned hit = 0;

	for(i = 0; i < MAX_LEXEME; ++i)
	{
		const int c = fgetc( f );

		fprintf( stderr, "Position: %llu | First char: '%c' (%i) | ", *streamoffs, c, c );
		fflush( stderr );

		ungetc( c, f );

		/* run through the possible candidates */
		lexeme = bn3f_lexeme_scan[i]( f, *streamoffs );

		fprintf( stderr, "Attempting token %s... ", _dbg_lexemes[i] );
		fflush( stderr );

		if(lexeme.start != lexeme.end)
		{
			hit = 1;
			*streamoffs += lexeme.end - lexeme.start;

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

	/* allocate space if necessary */
	if(*lexemes_sz >= *lexemes_cap)
	{
		*lexemes = realloc( *lexemes, (*lexemes_cap) << 1 ); /* *= 2 */

		/* zero out the new half of the array */
		memset( (void *)((uintptr_t)(*lexemes) + ((*lexemes_cap) *
		sizeof(struct bn3f_lexeme))), 0, (*lexemes_cap) *
		sizeof(struct bn3f_lexeme) );

		*lexemes_cap <<= 1; /* *= 2 */
	}

	(*lexemes)[(*lexemes_sz)++] = lexeme;

	return 0;
}

int bn3f_lex( FILE * f )
{
	unsigned r;
	bn3f_streamoffs_t streamoffs = 0;
	struct bn3f_lexeme * lexemes;
	size_t lexemes_sz = 0;
	size_t lexemes_cap = 16;

	fseek( f, 0, SEEK_SET );

	lexemes = calloc( lexemes_cap, sizeof(struct bn3f_lexeme) );

	for(;;)
	{
		r = bn3f_lex_scanloop( f, &streamoffs, &lexemes, &lexemes_sz,
		&lexemes_cap );

		if(r)
		{
			break;
		}
	}

	free( lexemes );

	return r >= 1 ? r - 1 : 0;
}

/** TEST SUITE */
int main( int ac, char ** av )
{
	int r;
	FILE * f;

	if( ac < 2 )
	{
		fprintf( stderr, "No file provided\n" );

		return 127;
	}

	f = fopen( av[1], "rb" );

	if( f == NULL )
	{
		fprintf( stderr, "Failed to open file '%s'\n", av[1] );

		return 127;
	}

#if 0
	r = fgetc( f );
	fprintf( stderr, "1: Got char '%c' (%i)\n", r, r );

	fseek( f, -1, SEEK_CUR );
	fprintf( stderr, "fseek( -1 )\n" );

	r = fgetc( f );
	fprintf( stderr, "2: Got char '%c', (%i)\n", r, r );

#endif /* 0 */
	r = bn3f_lex( f );

	fprintf( stderr, "bn3f_lex( ) returned %i\n", r );

	return 0;
}
