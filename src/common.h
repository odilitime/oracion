
#ifndef INC__ORACION_COMMON_H
#define INC__ORACION_COMMON_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minihn.h"

/** PREPROCESSOR FUNCTIONS */

/** ENUMERATIONS */

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
	/* offset of the first octet of this lexeme in the stream */
	ptri start;
	/* offset one past the last octet of this lexeme in the stream */
	ptri end;
	u64 len;
	s8 type;
	u8 abort;
};

/** FUNCTION POINTER TYPE DEFINITONS */

/* WHY: every scanner takes the current stream offset (not just the
 * FILE *) so it can stamp `start`/`end` on the lexeme it returns
 * without needing to `ftell( )` internally. */
typedef struct bn3f_lexeme (*_bn3f_scan_f)( FILE *, ptri );

/** SUBROUTINE FORWARD DECLARATIONS */

struct bn3f_lexeme _bn3f_lex_comment( FILE *, ptri );
struct bn3f_lexeme _bn3f_lex_whitespace( FILE *, ptri );
struct bn3f_lexeme _bn3f_lex_stringlit( FILE *, ptri );
struct bn3f_lexeme _bn3f_lex_characterlit( FILE *, ptri );
struct bn3f_lexeme _bn3f_lex_identifier( FILE *, ptri );
struct bn3f_lexeme _bn3f_lex_opdefine( FILE *, ptri );
struct bn3f_lexeme _bn3f_lex_opterminate( FILE *, ptri );
struct bn3f_lexeme _bn3f_lex_opgroupopen( FILE *, ptri );
struct bn3f_lexeme _bn3f_lex_opgroupclose( FILE *, ptri );
struct bn3f_lexeme _bn3f_lex_oprange( FILE *, ptri );
struct bn3f_lexeme _bn3f_lex_opsolidrepeat( FILE *, ptri );
struct bn3f_lexeme _bn3f_lex_ophollowrepeat( FILE *, ptri );
struct bn3f_lexeme _bn3f_lex_opoption( FILE *, ptri );
struct bn3f_lexeme _bn3f_lex_opfiniterepeat( FILE *, ptri );
struct bn3f_lexeme _bn3f_lex_opalternate( FILE *, ptri );

s8 _bn3f_lex_loopiter( FILE *, ptri *, struct bn3f_lexeme ***,
	ptri * );

/**
 * TITLE: Lex a full file into a lexeme array
 * DESCRIPTION: Tokenises the given file stream from the beginning to
 *              EOF (or the first unrecoverable error), returning a
 *              heap-allocated, NULL-terminated array of heap-allocated
 *              `struct bn3f_lexeme` pointers.
 * OWNERSHIP/CLEANUP: The returned array, and every non-NULL element
 *              within it, is owned by the caller and must be released
 *              by the caller. Free each element with `free( )`, then
 *              free the array itself with `free( )`. The array is
 *              always terminated by at least one `NULL` slot; iterate
 *              until a `NULL` element is reached rather than relying
 *              on a separately tracked length. On allocation failure
 *              mid-lex, previously produced elements remain valid and
 *              must still be freed by the caller.
 */
struct bn3f_lexeme ** bn3f_lex( FILE * );

/** STATIC DATA FORWARD DECLARATIONS */

/* WHY: `extern` here (defined once, in data.c) — this header is
 * included by every translation unit in the lexer. A non-extern array
 * definition here would be duplicated into every .o that includes it,
 * causing "multiple definition" errors at link time. */
extern const char * const _dbg_lexemes[BN3F_MAX_LEXEME];
extern _bn3f_scan_f _bn3f_scan[BN3F_MAX_LEXEME];

#endif /* INC__ORACION_COMMON_H */
