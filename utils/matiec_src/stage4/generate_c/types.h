/*
 * (c) 2000 Jiri Baum
 *          Mario de Sousa
 *
 * Offered to the public under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 *
 * This code is made available on the understanding that it will not be
 * used in safety-critical situations without a full and competent review.
 */


#ifndef __PLC_TYPES_H
#define __PLC_TYPES_H

#include <limits.h>
#include <float.h>   /* some limits related to floats are located here (?) */


/*
 * This will need to be conditional on the arquitecture or something.
 * Anyone know where we can snarf the relevant magic easily?
 */
typedef double                 f64; /* 64-bit floating point   */
typedef float                  f32; /* 32-bit floating point   */

typedef unsigned long long int u64; /* 64-bit unsigned integer */
typedef long long int          i64; /* 64-bit signed integer   */
typedef unsigned int           u32; /* 32-bit unsigned integer */
typedef int                    i32; /* 32-bit signed integer   */
typedef unsigned short int     u16; /* 16-bit unsigned integer */
typedef short int              i16; /* 16-bit signed integer   */
typedef unsigned char          u8;  /*  8-bit unsigned integer */
typedef signed char            i8;  /*  8-bit signed integer   */

/* some platforms seem to be missing <float.h> with the definition of FLT_MAX */
#ifndef FLT_MAX
  /* this is the minimum value guaranteed by ANSI C++           */
  /* does anybody know the minimum value guaranteed for ANSI C ? */
#define FLT_MAX 1E+37
#endif
#ifndef FLT_MIN
  /* this is the minimum value guaranteed by ANSI C++           */
  /* does anybody know the minimum value guaranteed for ANSI C ? */
#define FLT_MIN 1E-37
#endif

#define f32_MAX FLT_MAX
#define f32_MIN FLT_MIN

#define u32_MAX UINT_MAX
#define u32_MIN 0
#define i32_MAX INT_MAX
#define i32_MIN INT_MIN

#define u16_MAX USHRT_MAX
#define u16_MIN 0
#define i16_MAX SHRT_MAX
#define i16_MIN SHRT_MIN

#define u8_MAX UCHAR_MAX
#define u8_MIN 0
#define i8_MAX SCHAR_MAX
#define i8_MIN SCHAR_MIN

#endif /* __PLC_TYPES_H */

