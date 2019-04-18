#ifndef IEC_TYPES_H
#define IEC_TYPES_H

#include <limits.h>
#include <float.h>
#include <stdint.h>

/*********************/
/*  IEC Types defs   */
/*********************/

typedef uint8_t  IEC_BOOL;

typedef int8_t    IEC_SINT;
typedef int16_t   IEC_INT;
typedef int32_t   IEC_DINT;
typedef int64_t   IEC_LINT;

typedef uint8_t    IEC_USINT;
typedef uint16_t   IEC_UINT;
typedef uint32_t   IEC_UDINT;
typedef uint64_t   IEC_ULINT;

typedef uint8_t    IEC_BYTE;
typedef uint16_t   IEC_WORD;
typedef uint32_t   IEC_DWORD;
typedef uint64_t   IEC_LWORD;

typedef float    IEC_REAL;
typedef double   IEC_LREAL;

/* WARNING: When editing the definition of IEC_TIMESPEC, take note that 
 *          if the order of the two elements 'tv_sec' and 'tv_nsec' is changed, then the macros 
 *          __time_to_timespec() and __tod_to_timespec() will need to be changed accordingly.
 *          (these macros may be found in iec_std_lib.h)
 */
typedef struct {
    long int tv_sec;            /* Seconds.  */
    long int tv_nsec;           /* Nanoseconds.  */
} /* __attribute__((packed)) */ IEC_TIMESPEC;  /* packed is gcc specific! */

typedef IEC_TIMESPEC IEC_TIME;
typedef IEC_TIMESPEC IEC_DATE;
typedef IEC_TIMESPEC IEC_DT;
typedef IEC_TIMESPEC IEC_TOD;

#ifndef STR_MAX_LEN
#define STR_MAX_LEN 126
#endif

#ifndef STR_LEN_TYPE
#define STR_LEN_TYPE int8_t
#endif

#define __INIT_REAL 0
#define __INIT_LREAL 0
#define __INIT_SINT 0
#define __INIT_INT 0
#define __INIT_DINT 0
#define __INIT_LINT 0
#define __INIT_USINT 0
#define __INIT_UINT 0
#define __INIT_UDINT 0
#define __INIT_ULINT 0
#define __INIT_TIME (TIME){0,0}
#define __INIT_BOOL 0
#define __INIT_BYTE 0
#define __INIT_WORD 0
#define __INIT_DWORD 0
#define __INIT_LWORD 0
#define __INIT_STRING (STRING){0,""}
//#define __INIT_WSTRING
#define __INIT_DATE (DATE){0,0}
#define __INIT_TOD (TOD){0,0}
#define __INIT_DT (DT){0,0}

typedef STR_LEN_TYPE __strlen_t;
typedef struct {
    __strlen_t len;
    uint8_t body[STR_MAX_LEN];
} /* __attribute__((packed)) */ IEC_STRING;  /* packed is gcc specific! */

#endif /*IEC_TYPES_H*/
