/*
 * Copyright (C) 2007-2011: Edouard TISSERANT and Laurent BESSARD
 *
 * See COPYING and COPYING.LESSER files for copyright details.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef IEC_TYPES_ALL_H
#define IEC_TYPES_ALL_H


/* Macro that expand to subtypes */
#define __ANY(DO)                 __ANY_DERIVED(DO) __ANY_ELEMENTARY(DO)
#define __ANY_DERIVED(DO)
#define __ANY_ELEMENTARY(DO)      __ANY_MAGNITUDE(DO) __ANY_BIT(DO) __ANY_STRING(DO) __ANY_DATE(DO)
#define __ANY_MAGNITUDE(DO)       __ANY_NUM(DO) DO(TIME)
#define __ANY_BIT(DO)             __ANY_NBIT(DO) DO(BOOL)
#define __ANY_NBIT(DO)            DO(BYTE) DO(WORD) DO(DWORD) DO(LWORD)
#define __ANY_STRING(DO)          DO(STRING)
#define __ANY_DATE(DO)            DO(DATE) DO(TOD) DO(DT)
#define __ANY_NUM(DO)             __ANY_REAL(DO) __ANY_INT(DO)
#define __ANY_REAL(DO)            DO(REAL) DO(LREAL)
#define __ANY_INT(DO)             __ANY_SINT(DO) __ANY_UINT(DO)
#define __ANY_SINT(DO)            DO(SINT) DO(INT) DO(DINT) DO(LINT)
#define __ANY_UINT(DO)            DO(USINT) DO(UINT) DO(UDINT) DO(ULINT)


/* Macro that expand to subtypes */
#define __ANY_1(DO,P1)            __ANY_DERIVED_1(DO,P1) __ANY_ELEMENTARY_1(DO,P1)
#define __ANY_DERIVED_1(DO,P1)
#define __ANY_ELEMENTARY_1(DO,P1) __ANY_MAGNITUDE_1(DO,P1) __ANY_BIT_1(DO,P1) __ANY_STRING_1(DO,P1) __ANY_DATE_1(DO,P1)
#define __ANY_MAGNITUDE_1(DO,P1)  __ANY_NUM_1(DO,P1) DO(TIME,P1)
#define __ANY_BIT_1(DO,P1)        __ANY_NBIT_1(DO,P1) DO(BOOL,P1)
#define __ANY_NBIT_1(DO,P1)       DO(BYTE,P1) DO(WORD,P1) DO(DWORD,P1) DO(LWORD,P1)
#define __ANY_STRING_1(DO,P1)     DO(STRING,P1)
#define __ANY_DATE_1(DO,P1)       DO(DATE,P1) DO(TOD,P1) DO(DT,P1)
#define __ANY_NUM_1(DO,P1)        __ANY_REAL_1(DO,P1) __ANY_INT_1(DO,P1)
#define __ANY_REAL_1(DO,P1)       DO(REAL,P1) DO(LREAL,P1)
#define __ANY_INT_1(DO,P1)        __ANY_SINT_1(DO,P1) __ANY_UINT_1(DO,P1)
#define __ANY_SINT_1(DO,P1)       DO(SINT,P1) DO(INT,P1) DO(DINT,P1) DO(LINT,P1)
#define __ANY_UINT_1(DO,P1)       DO(USINT,P1) DO(UINT,P1) DO(UDINT,P1) DO(ULINT,P1)



/*********************/
/*  IEC Types defs   */
/*********************/

/* Include non windows.h clashing typedefs */
#include "iec_types.h"

#ifndef TRUE
  #define TRUE 1
  #define FALSE 0
#endif

#define __IEC_DEBUG_FLAG 0x01
#define __IEC_FORCE_FLAG 0x02
#define __IEC_RETAIN_FLAG 0x04
#define __IEC_OUTPUT_FLAG 0x08

#define __DECLARE_IEC_TYPE(type)\
typedef IEC_##type type;\
\
typedef struct {\
  IEC_##type value;\
  IEC_BYTE flags;\
} __IEC_##type##_t;\
\
typedef struct {\
  IEC_##type *value;\
  IEC_BYTE flags;\
  IEC_##type fvalue;\
} __IEC_##type##_p;



#define __DECLARE_DERIVED_TYPE(type, base)\
typedef base type;\
typedef __IEC_##base##_t __IEC_##type##_t;\
typedef __IEC_##base##_p __IEC_##type##_p;

#define __DECLARE_COMPLEX_STRUCT(type)\
typedef struct {\
  type value;\
  IEC_BYTE flags;\
} __IEC_##type##_t;\
\
typedef struct {\
  type *value;\
  IEC_BYTE flags;\
  type fvalue;\
} __IEC_##type##_p;

#define __DECLARE_ENUMERATED_TYPE(type, ...)\
typedef enum {\
  __VA_ARGS__\
} type;\
__DECLARE_COMPLEX_STRUCT(type)

#define __DECLARE_ARRAY_TYPE(type, base, size)\
typedef struct {\
  base table size;\
} type;\
__DECLARE_COMPLEX_STRUCT(type)

#define __DECLARE_STRUCT_TYPE(type, elements)\
typedef struct {\
  elements\
} type;\
__DECLARE_COMPLEX_STRUCT(type)

#define __DECLARE_REFTO_TYPE(type, name)\
typedef name type;\
__DECLARE_COMPLEX_STRUCT(type)


/* Those typdefs clash with windows.h */
/* i.e. this file cannot be included aside windows.h */
__ANY(__DECLARE_IEC_TYPE)

typedef struct {
  __IEC_BOOL_t X;  // state;  --> current step state. 0 : inative, 1: active.   We name it 'X' as it may be accessed from IEC 61131.3 code using stepname.X syntax!!
  BOOL prev_state; // previous step state. 0 : inative, 1: active
  __IEC_TIME_t T;  // elapsed_time;  --> time since step is active.   We name it 'T' as it may be accessed from IEC 61131.3 code using stepname.T syntax!!
} STEP;


typedef struct {
  BOOL stored;  // action storing state. 0 : not stored, 1: stored
  __IEC_BOOL_t state; // current action state. 0 : inative, 1: active
  BOOL set;   // set have been requested (reset each time the body is evaluated)
  BOOL reset; // reset have been requested (reset each time the body is evaluated)
  TIME set_remaining_time;    // time before set will be requested
  TIME reset_remaining_time;  // time before reset will be requested
} ACTION;

/* Extra debug types for SFC */
#define __ANY_SFC(DO) DO(STEP) DO(TRANSITION) DO(ACTION)

/* Enumerate native types */
#define __decl_enum_type(TYPENAME) TYPENAME##_ENUM,
#define __decl_enum_pointer(TYPENAME) TYPENAME##_P_ENUM,
#define __decl_enum_output(TYPENAME) TYPENAME##_O_ENUM,
typedef enum{
  __ANY(__decl_enum_type)
  __ANY(__decl_enum_pointer)
  __ANY(__decl_enum_output)
  /* SFC specific types are never external or global */
  UNKNOWN_ENUM
} __IEC_types_enum;

/* Get size of type from its number */
#define __decl_size_case(TYPENAME) \
	case TYPENAME##_ENUM:\
	case TYPENAME##_O_ENUM:\
	case TYPENAME##_P_ENUM:\
		return sizeof(TYPENAME);
static inline USINT __get_type_enum_size(__IEC_types_enum t){
 switch(t){
  __ANY(__decl_size_case)
  /* size do not correspond to real struct.
   * only a bool is used to represent state*/
  default:
	  return 0;
 }
 return 0;
}

#endif /*IEC_TYPES_ALL_H*/
