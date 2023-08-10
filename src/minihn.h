/*********************************************************************\
 *                  Mini-Hinterlib header provision                  *
 *               Copyright (C) 2023  Alexander Nicholi               *
 *            Released under Artisan Software Licence 1.1            *
\*********************************************************************/

#ifndef INC__MINIHN_H
#define INC__MINIHN_H

/* basic CPP defines */

#if defined(FALSE)
#undef FALSE
#endif /* defined(FALSE) */

#if defined(TRUE)
#undef TRUE
#endif /* defined(TRUE) */

#if defined(NULL)
#undef NULL
#endif /* defined(NULL) */

/* falsey */
#define FALSE 0
/* truthy */
#define TRUE (!(FALSE))
/* "null" pointer constant */
#define NULL 0

/* basic typedefs */

/* 8-bit signed integer */
typedef __INT8_TYPE__ s8;
/* 16-bit signed integer */
typedef __INT16_TYPE__ s16;
/* 32-bit signed integer */
typedef __INT32_TYPE__ s32;
/* 64-bit signed integer */
typedef __INT64_TYPE__ s64;

/* 8-bit unsigned integer */
typedef __UINT8_TYPE__ u8;
/* 16-bit unsigned integer */
typedef __UINT16_TYPE__ u16;
/* 32-bit unsigned integer */
typedef __UINT32_TYPE__ u32;
/* 64-bit unsigned integer */
typedef __UINT64_TYPE__ u64;

/* Address/pointer, absolute */
typedef __UINTPTR_TYPE__ ptri;
/* Address/pointer, relative */
typedef __INTPTR_TYPE__ offs;

/* Boolean */
typedef int bl;
/* ASCII character */
typedef __UINT8_TYPE__ chr;
/* Unicode code point */
typedef __UINT32_TYPE__ uchr;

/* Helper function to check if a character is valid ASCII. */
#define HN_CHR_ISVALID( _chr ) (((_chr) >= 0) && ((_chr) <= 127))

/* Helper function to check if a code point is valid Unicode. */
#define HN_UCHR_ISVALID( _uchr) \
	(((hn_uchr)(_uchr) >= 0) && ((hn_uchr)(_uchr) <= 0x1FFFFF))

/* Double-precision IEEE 754 floating-point number. */
typedef double f64;
/* Single-precision IEEE 754 floating-point number. */
typedef float f32;

#endif /* INC__MINIHN_H */
