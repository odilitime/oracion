
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char * help_text = "Oracion assembler\nCopyright (C) "
	"2023 Alexander Nicholi\n\nUsage:-\n\toracion [-h | --help]\n\nThe "
	"program reads from stdin and writes to stdout. Pass -h or --help "
	"to\nprint this text.";

enum
{
	LEX1_INITIAL,
	LEX1_COMMENT,
	LEX1_PUNCT,
	LEX1_IDENT,
	LEX1_WSPACE,
	LEX1_STRLIT,
	LEX1_CHRLIT,
	LEX1_NUMLIT,
	MAX_LEX1
};

struct lex1
{
	unsigned type : 3;
	uintptr_t offs;
};

/* ASCII code points 32-126 are valid */
static const uint8_t default_pass_change[32] = {
	0, 0, 0, 0,
	255, 255, 255, 255,
	255, 255, 255, 255,
	255, 255, 255, 254,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0
};

/* ASCII code points 32-126 and 128-255 are valid in comments */
static const uint8_t pass_comments[32] = {
	0, 0, 0, 0,
	255, 255, 255, 255,
	255, 255, 255, 255,
	255, 255, 255, 254,
	255, 255, 255, 255,
	255, 255, 255, 255,
	255, 255, 255, 255,
	255, 255, 255, 255
};

struct lex1 ** convert1( uint8_t * input )
{
	struct lex1 ** ret;
	uintptr_t ret_cap;
	uintptr_t ret_sz;
	/* this is a 256-bit bitfield selecting passable characters */
	uint8_t pass[32];
	/* this is a 256-bit bitfield selecting characters which may change
	 * state */
	uint8_t change[32];

	ret_cap = 16;
	ret_sz = 0;
	ret = malloc( sizeof(struct lex1 *) * ret_cap );

	memcpy( pass, default_pass_change, 32 );
	memcpy( change, default_pass_change, 32 );
}
