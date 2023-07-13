
#ifndef INC__ORACION_COMMON_H
#define INC__ORACION_COMMON_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** PREPROCESSOR FUNCTIONS */

/** ENUMERATIONS  */

enum
{
	BN3F_LEXEME_COMMENT = 0,
	BN3F_LEXEME_WHITESPACE,
	BN3F_LEXEME_STRINGLIT,
	BN3F_LEXEME_CHARACTERLIT,
	BN3F_LEXEME_IDENTIFIER,
	BN3F_LEXEME_OPDEFINE,
	BN3F_LEXEME_OPTERMINATE,
	BN3F_LEXEME_OPGROUPOPEN,
	BN3F_LEXEME_OPGROUPCLOSE,
	BN3F_LEXEME_OPRANGE,
	BN3F_LEXEME_OPSOLIDREPEAT,
	BN3F_LEXEME_OPHOLLOWREPEAT,
	BN3F_LEXEME_OPOPTION,
	BN3F_LEXEME_OPFINITEREPEAT,
	BN3F_LEXEME_OPALTERNATE,
	BN3F_MAX_LEXEME
};

/** STRUCTURE DEFINITIONS */

struct bn3f_lexeme
{
	uint64_t len;
	int8_t type;
	uint8_t abort;
};

/** FUNCTION POINTER TYPE DEFINITONS */

typedef struct bn3f_lexeme (*_bn3f_scan_f)( FILE * );

/** SUBROUTINE FORWARD DECLARATIONS */

struct bn3f_lexeme _bn3f_lex_comment( FILE * );
struct bn3f_lexeme _bn3f_lex_whitespace( FILE * );
struct bn3f_lexeme _bn3f_lex_stringlit( FILE * );
struct bn3f_lexeme _bn3f_lex_characterlit( FILE * );
struct bn3f_lexeme _bn3f_lex_identifier( FILE * );
struct bn3f_lexeme _bn3f_lex_opdefine( FILE * );
struct bn3f_lexeme _bn3f_lex_opterminate( FILE * );
struct bn3f_lexeme _bn3f_lex_opgroupopen( FILE * );
struct bn3f_lexeme _bn3f_lex_opgroupclose( FILE * );
struct bn3f_lexeme _bn3f_lex_oprange( FILE * );
struct bn3f_lexeme _bn3f_lex_opsolidrepeat( FILE * );
struct bn3f_lexeme _bn3f_lex_ophollowrepeat( FILE * );
struct bn3f_lexeme _bn3f_lex_opoption( FILE * );
struct bn3f_lexeme _bn3f_lex_opfiniterepeat( FILE * );
struct bn3f_lexeme _bn3f_lex_opalternate( FILE * );

int8_t _bn3f_lex_loopiter( FILE *, size_t *, struct bn3f_lexeme ***,
	size_t * );

struct bn3f_lexeme ** bn3f_lex( FILE * );

/** STATIC DATA FORWARD DECLARATIONS */

static const char * const _dbg_lexemes[BN3F_MAX_LEXEME];
_bn3f_scan_f _bn3f_scan[BN3F_MAX_LEXEME];

#endif /* INC__ORACION_COMMON_H */
