
#include "common.h"

/** STATIC DATA DEFINITIONS */

static const char * const _dbg_lexemes[BN3F_MAX_LEXEME] = {
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
