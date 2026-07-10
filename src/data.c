
#include "common.h"

/** STATIC DATA DEFINITIONS */

const char * const _dbg_lexemes[BN3F_MAX_LEXEME] = {
	"COMMENT",
	"WHITESPACE",
	"STRINGLIT",
	"CHARACTERLIT",
	"IDENTIFIER",
	"OPDEFINE",
	"OPTERMINATE",
	"OPGROUPOPEN",
	"OPGROUPCLOSE",
	"OPRANGE",
	"OPSOLIDREPEAT",
	"OPHOLLOWREPEAT",
	"OPOPTION",
	"OPFINITEREPEAT",
	"OPALTERNATE"
};

/* WHY: order must exactly track the BN3F_LEXEME_* enum in common.h —
 * `_bn3f_lex_loopiter( )` indexes this table by lexeme type ordinal,
 * not by name, so a reordering here silently mismatches the debug
 * names above and runs the wrong scanner for a given type. */
_bn3f_scan_f _bn3f_scan[BN3F_MAX_LEXEME] = {
	_bn3f_lex_comment,
	_bn3f_lex_whitespace,
	_bn3f_lex_stringlit,
	_bn3f_lex_characterlit,
	_bn3f_lex_identifier,
	_bn3f_lex_opdefine,
	_bn3f_lex_opterminate,
	_bn3f_lex_opgroupopen,
	_bn3f_lex_opgroupclose,
	_bn3f_lex_oprange,
	_bn3f_lex_opsolidrepeat,
	_bn3f_lex_ophollowrepeat,
	_bn3f_lex_opoption,
	_bn3f_lex_opfiniterepeat,
	_bn3f_lex_opalternate
};
