
#ifndef INC_API__GETOPT2_H
#define INC_API__GETOPT2_H

#include "minihn.h"

typedef u8 getopt2_modeopt_t;
typedef u8 getopt2_flagopt_t;

enum getopt2_modeopts
{
	/* Single-dash short flags and double-dash long flags are used. */
	GETOPT2_MODEOPT_MASK_FLAGS = 0x1,
	/* Parameters (think file paths) end-to-start are used. */
	GETOPT2_MODEOPT_MASK_ENDPARAMS = 0x2,
	/* Command verbs heading the command invocation are used. */
	GETOPT2_MODEOPT_MASK_VERBS = 0x4,
	/* One dash for stdin; two for flag parser termination. */
	GETOPT2_MODEOPT_MASK_DASHES = 0x8,
	/* Whether the form --flag=value is recognised. Otherwise only the
	 * form "--flag value" is. Ignored if GETOPT2_MODEOPT_MASK_VALUED is
	 * LOW. */
	GETOPT2_MODEOPT_MASK_LFLAGEQ = 0x10,
	/* Whether short flags stack together -likeso. */
	GETOPT2_MODEOPT_MASK_SFLAGSTACK = 0x20,
	/* Whether successive flags override previous ones left-to-right
	 * (it is otherwise treated as an error to have multiple conflicting
	 * flags). */
	GETOPT2_MODEOPT_MASK_LTROVR = 0x40,
	/* Mask of all reserved bits (must all be LOW). */
	GETOPT2_MODEOPT_MASK_RESERVED = 0x80
};

enum getopt2_flagopts
{
	/* Whether the flag takes a value or not. */
	GETOPT2_FLAGOPT_MASK_VALUED = 0x1,
	/* Whether the value of a flag is mandatory. */
	GETOPT2_FLAGOPT_MASK_REQDVAL = 0x2,
	/* Mask of all reserved bits (must all be LOW). */
	GETOPT2_FLAGOPT_MASK_RESERVED = 0xFC
};

struct getopt2_flag
{
	/* short flag. */
	chr s;
	/* long flag. */
	chr * l;
};

struct getopt2_args
{
	/* number of arguments. this constitutes the size of the argv
	 * array, denominated in elements. */
	ptri argc;
	/* array of argument values, sized by ".argc". each element is a
	 * NUL-terminated string of ASCII characters with UTF-8 high-bit
	 * tolerance (no UTF-8 validation is performed). */
	chr ** argv;
};

struct getopt2_flagparam
{
	/* the value of the flag, as in presuming the flag is a key and it
	 * takes a value like "--flag value", "--flag=value" or "-f value".
	 * this will be NULL if no value was found. */
	chr * value;
	/* whether the requested flag was found in the args stream. */
	u32 found : 1;
	/* 0 = flag preceded verb, 1 = flag followed verb. */
	u32 afterverb : 1;
};

/**
 * TITLE: Normalise args from standard C main( ) subroutine
 * DESCRIPTION: Takes a standard ANSI C (int, char **) pair and returns
 *              a struct getopt2_args containing the same arguments
 *              reinterpreted as a ptri for argc and a chr ** for argv.
 * PARAMETER: argc from ANSI C main( ).
 * PARAMETER: argv from ANSI C main( ).
 * RETURNS: A struct getopt2_args containing the parameters given.
 * NOTES: This subroutine does not create a copy of the parameters. No
 *        memory allocation takes place, therefore no deallocation
 *        should be done either.
 */
struct getopt2_args getopt2_normalise( int, char ** );

/**
 * TITLE: Get flag and potentially parameter from args
 * DESCRIPTION: Scan the args struct for a given flag and return its
 *              presence and potentially value.
 * PARAMETER: Mode options. See enum getopt2_modeopts for details.
 * PARAMETER: Flag-specific options. See enum getopt2_flagopts for
 *            details.
 * PARAMETER: Flag companion to search for. The .s field may be omitted
 *            by specifying the '\0' character as the short flag.
 * PARAMETER: args struct to scan through.
 * RETURNS: A getopt2_flagparam struct containing a boolean telling if
 *          the flag was found, and if it has a value, a pointer to a
 *          character string that starts at the first character of that
 *          value in the original args stream.
 * NOTES: This subroutine does not create a copy of the parameters. No
 *        memory allocation takes place, therefore no deallocation
 *        should be done either.
 */
struct getopt2_flagparam getopt2_getflag(
	getopt2_modeopt_t,
	getopt2_flagopt_t,
	struct getopt2_flag,
	struct getopt2_args );

/**
 * TITLE: Get a verb from args
 * DESCRIPTION: Finds a verb from the args struct. This is the first
 *              non-flag found during command invocation.
 * PARAMETER: Mode options. See enum getopt2_modeopts for details.
 * PARAMETER: args struct to scan through.
 * RETURNS: The verb as stored in the original args struct.
 * NOTES: This subroutine does not create a copy of the parameters. No
 *        memory allocation takes place, therefore no deallocation
 *        should be done either.
 */
chr * getopt2_getverb(
	getopt2_modeopt_t,
	struct getopt2_args );

/**
 * TITLE: Count parameters in args stream
 * DESCRIPTION: Enumerates the total number of parameters, discounting
 *              all flags, flag values and verbs.
 * PARAMETER: Mode options. See enum getopt2_modeopts for details.
 * PARAMETER: Size of the flag list in the following parameter.
 * PARAMETER: List of flags that take values, to be ignored for the
 *            purposes of parsing parameters.
 * PARAMETER: args struct to scan through.
 * RETURNS: The total number of parameters in the args stream.
 */
ptri getopt2_getparamcount(
	getopt2_modeopt_t,
	ptri,
	struct getopt2_flag *,
	struct getopt2_args );

/**
 * TITLE: Get a parameter from the args stream
 * DESCRIPTION: Gets a parameter by index from the args stream. Use the
 *              getopt2_getparamcount( ) subroutine to deduce how many
 *              parameters are present.
 * PARAMETER: Mode options. See enum getopt2_modeopts for details.
 * PARAMATER: Index of the parameter to return, starting from zero (0).
 * PARAMETER: Size of the flag list in the following parameter.
 * PARAMETER: List of flags that take values, to be ignored for the
 *            purposes of parsing parameters.
 * PARAMETER: args struct to scan through.
 * RETURNS: The parameter stored in the original args struct.
 * NOTES: This subroutine does not create a copy of the parameters. No
 *        memory allocation takes place, therefore no deallocation
 *        should be done either.
 */
chr * getopt2_getparam(
	getopt2_modeopt_t,
	ptri,
	ptri,
	struct getopt2_flag *,
	struct getopt2_args );

#endif /* INC_API__GETOPT2_H */
