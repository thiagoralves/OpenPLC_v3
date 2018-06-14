/*
 * copyright 2008 Edouard TISSERANT
 * copyright 2011 Mario de Sousa (msousa@fe.up.pt)
 *
 * Offered to the public under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.
 *
 * This code is made available on the understanding that it will not be
 * used in safety-critical situations without a full and competent review.
 */

/****
 * IEC 61131-3 standard function library
 */

/* NOTE: This file is full of (what may seem at first) very strange macros.
 *       If you want to know what all these strange macros are doing,
 *       just parse this file through a C preprocessor (e.g. cpp), 
 *       and analyse the output!
 *       $gcc -E iec_std_lib.h 
 */

#ifndef _IEC_STD_LIB_H
#define _IEC_STD_LIB_H


#include <limits.h>
#include <float.h>
#include <math.h>
#include <stdint.h>
#include <ctype.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef DEBUG_IEC
#define DBG(...) printf(__VA_ARGS__);
#define DBG_TYPE(TYPENAME, name) __print_##TYPENAME(name);
#else
#define DBG(...)
#define DBG_TYPE(TYPENAME, name)
#endif

/*
 * Include type defs.
 */
#include "iec_types_all.h"

extern TIME __CURRENT_TIME;
extern BOOL __DEBUG;

/* TODO
typedef struct {
    __strlen_t len;
    u_int16_t body[STR_MAX_LEN];
} WSTRING;
*/
/*
# if __WORDSIZE == 64
#define __32b_sufix
#define __64b_sufix L
#else
#define __32b_sufix L
#define __64b_sufix LL
#endif
*/

# if __WORDSIZE == 64
#define __32b_sufix
#define __64b_sufix L
#else
#define __32b_sufix L
/* changed this from LL to L temporarily. It was causing a bug when compiling resulting code with gcc.
 * I have other things to worry about at the moment.. 
 */
#define __64b_sufix L   
#endif


#define __lit(type,value,...) (type)value##__VA_ARGS__
// Keep this macro expention step to let sfx(__VA_ARGS__) change into L or LL
#define __literal(type,value,...) __lit(type,value,__VA_ARGS__)

#define __BOOL_LITERAL(value) __literal(BOOL,value)
#define __SINT_LITERAL(value) __literal(SINT,value)
#define __INT_LITERAL(value) __literal(INT,value)
#define __DINT_LITERAL(value) __literal(DINT,value,__32b_sufix)
#define __LINT_LITERAL(value) __literal(LINT,value,__64b_sufix)
#define __USINT_LITERAL(value) __literal(USINT,value)
#define __UINT_LITERAL(value) __literal(UINT,value)
#define __UDINT_LITERAL(value) __literal(UDINT,value,__32b_sufix)
#define __ULINT_LITERAL(value) __literal(ULINT,value,__64b_sufix)
#define __REAL_LITERAL(value) __literal(REAL,value,__32b_sufix)
#define __LREAL_LITERAL(value) __literal(LREAL,value,__64b_sufix)
#define __TIME_LITERAL(value) __literal(TIME,value)
#define __DATE_LITERAL(value) __literal(DATE,value)
#define __TOD_LITERAL(value) __literal(TOD,value)
#define __DT_LITERAL(value) __literal(DT,value)
#define __STRING_LITERAL(count,value) (STRING){count,value}
#define __BYTE_LITERAL(value) __literal(BYTE,value)
#define __WORD_LITERAL(value) __literal(WORD,value)
#define __DWORD_LITERAL(value) __literal(DWORD,value,__32b_sufix)
#define __LWORD_LITERAL(value) __literal(LWORD,value,__64b_sufix)


typedef union __IL_DEFVAR_T {
    BOOL    BOOLvar;

    SINT    SINTvar;
    INT     INTvar;
    DINT    DINTvar;
    LINT    LINTvar;

    USINT   USINTvar;
    UINT    UINTvar;
    UDINT   UDINTvar;
    ULINT   ULINTvar;

    BYTE    BYTEvar;
    WORD    WORDvar;
    DWORD   DWORDvar;
    LWORD   LWORDvar;

    REAL    REALvar;
    LREAL   LREALvar;

    TIME    TIMEvar;
    TOD TODvar;
    DT  DTvar;
    DATE    DATEvar;
} __IL_DEFVAR_T;


/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****      Some helper functions...                              *****/
/*****                     ...used later:                         *****/
/*****    - when declaring the IEC 61131-3 standard functions     *****/
/*****    - in the C source code itself in SFC and ST expressions *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/


/****************************/
/* Notify IEC runtime error */
/****************************/

/* function that generates an IEC runtime error */
static inline void __iec_error(void) {
  /* TODO... */
  fprintf(stderr, "IEC 61131-3 runtime error.\n");
  /*exit(1);*/
}

/*******************************/
/* Time normalization function */
/*******************************/

static inline void __normalize_timespec (IEC_TIMESPEC *ts) {
  if( ts->tv_nsec < -1000000000 || (( ts->tv_sec > 0 ) && ( ts->tv_nsec < 0 ))){
    ts->tv_sec--;
    ts->tv_nsec += 1000000000;
  }
  if( ts->tv_nsec > 1000000000 || (( ts->tv_sec < 0 ) && ( ts->tv_nsec > 0 ))){
    ts->tv_sec++;
    ts->tv_nsec -= 1000000000;
  }
}

/**********************************************/
/* Time conversion to/from timespec functions */
/**********************************************/
/* NOTE: The following function was turned into a macro, so it could be used to initialize the initial value of TIME variables.
 *       Since each macro parameter is evaluated several times, the macro may result in multiple function invocations if an expression
 *       containing a function invocation is passed as a parameter. However, currently matiec only uses this conversion macro with 
 *       constant literals, so it is safe to change it into a macro.
 */
/* NOTE: I (Mario - msousa@fe.up.pt) believe that the following function contains a bug when handling negative times.
 *       The equivalent macro has this bug fixed.
 *       e.g.;
 *          T#3.8s
 *       using the function, will result in a timespec of 3.8s !!!: 
 *          tv_sec  =  4               <-----  1 *  3.8           is rounded up when converting a double to an int!
 *          tv_nsec = -200 000 000     <-----  1 * (3.8 - 4)*1e9
 * 
 *         -T#3.8s
 *       using the function, will result in a timespec of -11.8s !!!: 
 *          tv_sec  = -4                 <-----  -1 *  3.8 is rounded down when converting a double to an int!
 *          tv_nsec = -7 800 000 000     <-----  -1 * (3.8 - -4)*1e9
 */
/* NOTE: Due to the fact that the C compiler may round a tv_sec number away from zero, 
 *       the following macro may result in a timespec that is not normalized, i.e. with a tv_sec > 0, and a tv_nsec < 0 !!!!
 *       This is due to the rounding that C compiler applies when converting a (long double) to a (long int).
 *       To produce normalized timespec's we need to use floor(), but we cannot call any library functions since we want this macro to be 
 *       useable as a variable initializer.
 *       VAR x : TIME = T#3.5h; END_VAR --->  IEC_TIME x = __time_to_timespec(1, 0, 0, 0, 3.5, 0);
 */
/*
static inline IEC_TIMESPEC __time_to_timespec(int sign, double mseconds, double seconds, double minutes, double hours, double days) {
  IEC_TIMESPEC ts;

  // sign is 1 for positive values, -1 for negative time...
  long double total_sec = ((days*24 + hours)*60 + minutes)*60 + seconds + mseconds/1e3;
  if (sign >= 0) sign = 1; else sign = -1;
  ts.tv_sec = sign * (long int)total_sec;
  ts.tv_nsec = sign * (long int)((total_sec - ts.tv_sec)*1e9);

  return ts;
}
*/
/* NOTE: Unfortunately older versions of ANSI C (e.g. C99) do not allow explicit identification of elements in initializers
 *         e.g.  {tv_sec = 1, tv_nsec = 300}
 *       They are therefore commented out. This however means that any change to the definition of IEC_TIMESPEC may require this
 *       macro to be updated too!
 */
#define __time_to_timespec(sign,mseconds,seconds,minutes,hours,days) \
          ((IEC_TIMESPEC){\
              /*tv_sec  =*/ ((long int)   (((sign>=0)?1:-1)*((((long double)days*24 + (long double)hours)*60 + (long double)minutes)*60 + (long double)seconds + (long double)mseconds/1e3))), \
              /*tv_nsec =*/ ((long int)(( \
                            ((long double)(((sign>=0)?1:-1)*((((long double)days*24 + (long double)hours)*60 + (long double)minutes)*60 + (long double)seconds + (long double)mseconds/1e3))) - \
                            ((long int)   (((sign>=0)?1:-1)*((((long double)days*24 + (long double)hours)*60 + (long double)minutes)*60 + (long double)seconds + (long double)mseconds/1e3)))   \
                            )*1e9))\
        })




/* NOTE: The following function was turned into a macro, so it could be used to initialize the initial value of TOD (TIME_OF_DAY) variables */
/* NOTE: many (but not all) of the same comments made regarding __time_to_timespec() are also valid here, so go and read those comments too!
/*
static inline IEC_TIMESPEC __tod_to_timespec(double seconds, double minutes, double hours) {
  IEC_TIMESPEC ts;

  long double total_sec = (hours*60 + minutes)*60 + seconds;
  ts.tv_sec = (long int)total_sec;
  ts.tv_nsec = (long int)((total_sec - ts.tv_sec)*1e9);

  return ts;
}
*/
#define __tod_to_timespec(seconds,minutes,hours) \
          ((IEC_TIMESPEC){\
              /*tv_sec  =*/ ((long int)   ((((long double)hours)*60 + (long double)minutes)*60 + (long double)seconds)), \
              /*tv_nsec =*/ ((long int)(( \
                            ((long double)((((long double)hours)*60 + (long double)minutes)*60 + (long double)seconds)) - \
                            ((long int)   ((((long double)hours)*60 + (long double)minutes)*60 + (long double)seconds))   \
                            )*1e9))\
        })


#define EPOCH_YEAR 1970
#define SECONDS_PER_MINUTE 60
#define SECONDS_PER_HOUR (60 * SECONDS_PER_MINUTE)
#define SECONDS_PER_DAY (24 * SECONDS_PER_HOUR)
#define __isleap(year) \
  ((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))
static const unsigned short int __mon_yday[2][13] =
{
  /* Normal years.  */
  { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365},
  /* Leap years.  */
  { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366}
};

typedef struct {
	int tm_sec;			/* Seconds.	[0-60] (1 leap second) */
	int tm_min;			/* Minutes.	[0-59] */
	int tm_hour;			/* Hours.	[0-23] */
	int tm_day;			/* Day.		[1-31] */
	int tm_mon;			/* Month.	[0-11] */
	int tm_year;			/* Year	*/
} tm;

static inline tm convert_seconds_to_date_and_time(long int seconds) {
  tm dt;
  long int days, rem;
  days = seconds / SECONDS_PER_DAY;
  rem = seconds % SECONDS_PER_DAY;
  if (rem < 0) {
	  rem += SECONDS_PER_DAY;
	  days--;
  }

  // time of day
  dt.tm_hour = rem / SECONDS_PER_HOUR;
  rem %= SECONDS_PER_HOUR;
  dt.tm_min = rem / 60;
  dt.tm_sec = rem % 60;

  // date
  dt.tm_year = EPOCH_YEAR;
  while (days >= (rem = __isleap(dt.tm_year) ? 366 : 365)) {
	  dt.tm_year++;
	  days -= rem;
  }
  while (days < 0) {
	  dt.tm_year--;
	  days += __isleap(dt.tm_year) ? 366 : 365;
  }
  dt.tm_mon = 1;
  while (days > __mon_yday[__isleap(dt.tm_year)][dt.tm_mon]) {
	  dt.tm_mon += 1;
  }
  dt.tm_day = days - __mon_yday[__isleap(dt.tm_year)][dt.tm_mon - 1] + 1;

  return dt;
}

static inline IEC_TIMESPEC __date_to_timespec(int day, int month, int year) {
  IEC_TIMESPEC ts;
  int a4, b4, a100, b100, a400, b400;
  int yday;
  int intervening_leap_days;

  if (month < 1 || month > 12)
	 __iec_error();

  yday = __mon_yday[__isleap(year)][month - 1] + day;

  if (yday > __mon_yday[__isleap(year)][month])
	  __iec_error();

  a4 = (year >> 2) - ! (year & 3);
  b4 = (EPOCH_YEAR >> 2) - ! (EPOCH_YEAR & 3);
  a100 = a4 / 25 - (a4 % 25 < 0);
  b100 = b4 / 25 - (b4 % 25 < 0);
  a400 = a100 >> 2;
  b400 = b100 >> 2;
  intervening_leap_days = (a4 - b4) - (a100 - b100) + (a400 - b400);
  
  ts.tv_sec = ((year - EPOCH_YEAR) * 365 + intervening_leap_days + yday - 1) * 24 * 60 * 60;
  ts.tv_nsec = 0;

  return ts;
}

static inline IEC_TIMESPEC __dt_to_timespec(double seconds, double minutes, double hours, int day, int month, int year) {
  IEC_TIMESPEC ts_date = __date_to_timespec(day, month, year);
  IEC_TIMESPEC ts = __tod_to_timespec(seconds, minutes, hours);

  ts.tv_sec += ts_date.tv_sec;

  return ts;
}

/*******************/
/* Time operations */
/*******************/

#define __time_cmp(t1, t2) (t2.tv_sec == t1.tv_sec ? t1.tv_nsec - t2.tv_nsec : t1.tv_sec - t2.tv_sec)

static inline TIME __time_add(TIME IN1, TIME IN2){
  TIME res ={IN1.tv_sec + IN2.tv_sec,
             IN1.tv_nsec + IN2.tv_nsec };
  __normalize_timespec(&res);
  return res;
}
static inline TIME __time_sub(TIME IN1, TIME IN2){
  TIME res ={IN1.tv_sec - IN2.tv_sec,
             IN1.tv_nsec - IN2.tv_nsec };
  __normalize_timespec(&res);
  return res;
}
static inline TIME __time_mul(TIME IN1, LREAL IN2){
  LREAL s_f = IN1.tv_sec * IN2;
  time_t s = (time_t)s_f;
  div_t ns = div((int)((LREAL)IN1.tv_nsec * IN2), 1000000000);
  TIME res = {(long)s + ns.quot,
		      (long)ns.rem + (s_f - s) * 1000000000 };
  __normalize_timespec(&res);
  return res;
}
static inline TIME __time_div(TIME IN1, LREAL IN2){
  LREAL s_f = IN1.tv_sec / IN2;
  time_t s = (time_t)s_f;
  TIME res = {(long)s,
              (long)(IN1.tv_nsec / IN2 + (s_f - s) * 1000000000) };
  __normalize_timespec(&res);
  return res;
}


/***************/
/* Convertions */
/***************/
    /*****************/
    /*  REAL_TO_INT  */
    /*****************/
static inline LINT __real_round(LREAL IN) {
	return fmod(IN, 1) == 0 ? ((LINT)IN / 2) * 2 : (LINT)IN;
}
static inline LINT __preal_to_sint(LREAL IN) {
   return IN >= 0 ? __real_round(IN + 0.5) : __real_round(IN - 0.5);
}
static inline LINT __preal_to_uint(LREAL IN) {
   return IN >= 0 ? __real_round(IN + 0.5) : 0;
}
static inline LINT __real_to_sint(LREAL IN)  {return (LINT)__preal_to_sint(IN);}
static inline LWORD __real_to_bit(LREAL IN)  {return (LWORD)__preal_to_uint(IN);}
static inline ULINT __real_to_uint(LREAL IN) {return (ULINT)__preal_to_uint(IN);}

    /***************/
    /*  TO_STRING  */
    /***************/
static inline STRING __bool_to_string(BOOL IN) {
    if(IN) return (STRING){4, "TRUE"};
    return (STRING){5,"FALSE"};
}
static inline STRING __bit_to_string(LWORD IN) {
    STRING res;
    res = __INIT_STRING;
    res.len = snprintf((char*)res.body, STR_MAX_LEN, "16#%llx",(long long unsigned int)IN);
    if(res.len > STR_MAX_LEN) res.len = STR_MAX_LEN;
    return res;
}
static inline STRING __real_to_string(LREAL IN) {
    STRING res;
    res = __INIT_STRING;
    res.len = snprintf((char*)res.body, STR_MAX_LEN, "%.10g", IN);
    if(res.len > STR_MAX_LEN) res.len = STR_MAX_LEN;
    return res;
}
static inline STRING __sint_to_string(LINT IN) {
    STRING res;
    res = __INIT_STRING;
    res.len = snprintf((char*)res.body, STR_MAX_LEN, "%lld", (long long int)IN);
    if(res.len > STR_MAX_LEN) res.len = STR_MAX_LEN;
    return res;
}
static inline STRING __uint_to_string(ULINT IN) {
    STRING res;
    res = __INIT_STRING;
    res.len = snprintf((char*)res.body, STR_MAX_LEN, "%llu", (long long unsigned int)IN);
    if(res.len > STR_MAX_LEN) res.len = STR_MAX_LEN;
    return res;
}
    /***************/
    /* FROM_STRING */
    /***************/
static inline BOOL __string_to_bool(STRING IN) {
    int i;
    if (IN.len == 1) return !memcmp(&IN.body,"1", IN.len);
    for (i = 0; i < IN.len; i++) IN.body[i] = toupper(IN.body[i]);
    return IN.len == 4 ? !memcmp(&IN.body,"TRUE", IN.len) : 0;
}

static inline LINT __pstring_to_sint(STRING* IN) {
    LINT res = 0;
    __strlen_t l;
    unsigned int shift = 0;

    if(IN->body[0]=='2' && IN->body[1]=='#'){
        /* 2#0101_1010_1011_1111 */
        for(l = IN->len - 1; l >= 2 && shift < 64; l--)
        {
            char c = IN->body[l];
            if( c >= '0' && c <= '1'){
                res |= ( c - '0') << shift;
                shift += 1;
            }
        }
    }else if(IN->body[0]=='8' && IN->body[1]=='#'){
        /* 8#1234_5665_4321 */
        for(l = IN->len - 1; l >= 2 && shift < 64; l--)
        {
            char c = IN->body[l];
            if( c >= '0' && c <= '7'){
                res |= ( c - '0') << shift;
                shift += 3;
            }
        }
    }else if(IN->body[0]=='1' && IN->body[1]=='6' && IN->body[2]=='#'){
        /* 16#1234_5678_9abc_DEFG */
        for(l = IN->len - 1; l >= 3 && shift < 64; l--)
        {
            char c = IN->body[l];
            if( c >= '0' && c <= '9'){
                res |= (LWORD)( c - '0') << shift;
                shift += 4;
            }else if( c >= 'a' && c <= 'f'){
                res |= (LWORD)( c - 'a' + 10 ) << shift;
                shift += 4;
            }else if( c >= 'A' && c <= 'F'){
                res |= (LWORD)( c - 'A' + 10 ) << shift;
                shift += 4;
            }
        }
    }else{
        /* -123456789 */
        LINT fac = IN->body[0] == '-' ? -1 : 1;
        for(l = IN->len - 1; l >= 0 && shift < 20; l--)
        {
            char c = IN->body[l];
            if( c >= '0' && c <= '9'){
                res += ( c - '0') * fac;
                fac *= 10;
                shift += 1;
            }else if( c >= '.' ){ /* reset value */
                res = 0;
                fac = IN->body[0] == '-' ? -1 : 1;
                shift = 0;
            }
        }
    }
    return res;
}

static inline LINT  __string_to_sint(STRING IN) {return (LINT)__pstring_to_sint(&IN);}
static inline LWORD __string_to_bit (STRING IN) {return (LWORD)__pstring_to_sint(&IN);}
static inline ULINT __string_to_uint(STRING IN) {return (ULINT)__pstring_to_sint(&IN);}
static inline LREAL __string_to_real(STRING IN) {
    __strlen_t l;
    l = IN.len;
    /* search the dot */
    while(--l > 0 && IN.body[l] != '.');
    if(l != 0){
        return atof((const char *)&IN.body);
    }else{
        return (LREAL)__pstring_to_sint(&IN);
    }
}

    /***************/
    /*   TO_TIME   */
    /***************/
static inline TIME    __int_to_time(LINT IN)  {return (TIME){IN, 0};}
static inline TIME   __real_to_time(LREAL IN) {return (TIME){IN, (IN - (LINT)IN) * 1000000000};}
static inline TIME __string_to_time(STRING IN){
    __strlen_t l;
    /* TODO :
     *
     *  Duration literals without underlines: T#14ms    T#-14ms   T#14.7s   T#14.7m
     *                short prefix            T#14.7h    t#14.7d   t#25h15m
     *                                        t#5d14h12m18s3.5ms
     *                long prefix             TIME#14ms    TIME#-14ms   time#14.7s
     *  Duration literals with underlines:
     *                short prefix            t#25h_15m t#5d_14h_12m_18s_3.5ms
     *                long prefix             TIME#25h_15m
     *                                        time#5d_14h_12m_18s_3.5ms
     *
     *  Long prefix notation                 Short prefix notation
     *  DATE#1984-06-25                      D#1984-06-25
     *  date#1984-06-25                      d#1984-06-25
     *  TIME_OF_DAY#15:36:55.36              TOD#15:36:55.36
     *  time_of_day#15:36:55.36              tod#15:36:55.36
     *  DATE_AND_TIME#1984-06-25-15:36:55.36 DT#1984-06-25-15:36:55.36
     *  date_and_time#1984-06-25-15:36:55.36 dt#1984-06-25-15:36:55.36
     *
     */
    /* Quick hack : only transform seconds */
    /* search the dot */
    l = IN.len;
    while(--l > 0 && IN.body[l] != '.');
    if(l != 0){
        LREAL IN_val = atof((const char *)&IN.body);
        return  (TIME){(long)IN_val, (long)(IN_val - (LINT)IN_val)*1000000000};
    }else{
        return  (TIME){(long)__pstring_to_sint(&IN), 0};
    }
}

    /***************/
    /*  FROM_TIME  */
    /***************/
static inline LREAL __time_to_real(TIME IN){
    return (LREAL)IN.tv_sec + ((LREAL)IN.tv_nsec/1000000000);
}
static inline LINT __time_to_int(TIME IN) {return IN.tv_sec;}
static inline STRING __time_to_string(TIME IN){
    STRING res;
    div_t days;
    /*t#5d14h12m18s3.5ms*/
    res = __INIT_STRING;
    days = div((int &)IN.tv_sec, SECONDS_PER_DAY);
    if(!days.rem && IN.tv_nsec == 0){
        res.len = snprintf((char*)&res.body, STR_MAX_LEN, "T#%dd", days.quot);
    }else{
        div_t hours = div(days.rem, SECONDS_PER_HOUR);
        if(!hours.rem && IN.tv_nsec == 0){
            res.len = snprintf((char*)&res.body, STR_MAX_LEN, "T#%dd%dh", days.quot, hours.quot);
        }else{
            div_t minuts = div(hours.rem, SECONDS_PER_MINUTE);
            if(!minuts.rem && IN.tv_nsec == 0){
                res.len = snprintf((char*)&res.body, STR_MAX_LEN, "T#%dd%dh%dm", days.quot, hours.quot, minuts.quot);
            }else{
                if(IN.tv_nsec == 0){
                    res.len = snprintf((char*)&res.body, STR_MAX_LEN, "T#%dd%dh%dm%ds", days.quot, hours.quot, minuts.quot, minuts.rem);
                }else{
                    res.len = snprintf((char*)&res.body, STR_MAX_LEN, "T#%dd%dh%dm%ds%gms", days.quot, hours.quot, minuts.quot, minuts.rem, (LREAL)IN.tv_nsec / 1000000);
                }
            }
        }
    }
    if(res.len > STR_MAX_LEN) res.len = STR_MAX_LEN;
    return res;
}
static inline STRING __date_to_string(DATE IN){
    STRING res;
    tm broken_down_time;
    /* D#1984-06-25 */
    broken_down_time = convert_seconds_to_date_and_time(IN.tv_sec);
    res = __INIT_STRING;
    res.len = snprintf((char*)&res.body, STR_MAX_LEN, "D#%d-%2.2d-%2.2d",
             broken_down_time.tm_year,
             broken_down_time.tm_mon,
             broken_down_time.tm_day);
    if(res.len > STR_MAX_LEN) res.len = STR_MAX_LEN;
    return res;
}
static inline STRING __tod_to_string(TOD IN){
    STRING res;
    tm broken_down_time;
    time_t seconds;
    /* TOD#15:36:55.36 */
    seconds = IN.tv_sec;
    if (seconds >= SECONDS_PER_DAY){
		__iec_error();
		return (STRING){9,"TOD#ERROR"};
	}
    broken_down_time = convert_seconds_to_date_and_time(seconds);
    res = __INIT_STRING;
    if(IN.tv_nsec == 0){
        res.len = snprintf((char*)&res.body, STR_MAX_LEN, "TOD#%2.2d:%2.2d:%2.2d",
                 broken_down_time.tm_hour,
                 broken_down_time.tm_min,
                 broken_down_time.tm_sec);
    }else{
        res.len = snprintf((char*)&res.body, STR_MAX_LEN, "TOD#%2.2d:%2.2d:%09.6f",
                 broken_down_time.tm_hour,
                 broken_down_time.tm_min,
                 (LREAL)broken_down_time.tm_sec + (LREAL)IN.tv_nsec / 1e9);
    }
    if(res.len > STR_MAX_LEN) res.len = STR_MAX_LEN;
    return res;
}
static inline STRING __dt_to_string(DT IN){
    STRING res;
    tm broken_down_time;
    /* DT#1984-06-25-15:36:55.36 */
    broken_down_time = convert_seconds_to_date_and_time(IN.tv_sec);
    if(IN.tv_nsec == 0){
        res.len = snprintf((char*)&res.body, STR_MAX_LEN, "DT#%d-%2.2d-%2.2d-%2.2d:%2.2d:%2.2d",
                 broken_down_time.tm_year,
                 broken_down_time.tm_mon,
                 broken_down_time.tm_day,
                 broken_down_time.tm_hour,
                 broken_down_time.tm_min,
                 broken_down_time.tm_sec);
    }else{
        res.len = snprintf((char*)&res.body, STR_MAX_LEN, "DT#%d-%2.2d-%2.2d-%2.2d:%2.2d:%09.6f",
                 broken_down_time.tm_year,
                 broken_down_time.tm_mon,
                 broken_down_time.tm_day,
                 broken_down_time.tm_hour,
                 broken_down_time.tm_min,
                 (LREAL)broken_down_time.tm_sec + ((LREAL)IN.tv_nsec / 1e9));
    }
    if(res.len > STR_MAX_LEN) res.len = STR_MAX_LEN;
    return res;
}

    /**********************************************/
    /*  [ANY_DATE | TIME] _TO_ [ANY_DATE | TIME]  */
    /**********************************************/

static inline TOD __date_and_time_to_time_of_day(DT IN) {
	return (TOD){
		IN.tv_sec % SECONDS_PER_DAY + (IN.tv_sec < 0 ? SECONDS_PER_DAY : 0),
		IN.tv_nsec};
}
static inline DATE __date_and_time_to_date(DT IN){
	return (DATE){
		IN.tv_sec - IN.tv_sec % SECONDS_PER_DAY - (IN.tv_sec < 0 ? SECONDS_PER_DAY : 0),
		0};
}

    /*****************/
    /*  FROM/TO BCD  */
    /*****************/

static inline BOOL __test_bcd(LWORD IN) {
	while (IN) {
		if ((IN & 0xf) > 9) return 1;
		IN >>= 4;
	}
	return 0;
}

static inline ULINT __bcd_to_uint(LWORD IN){
    ULINT res = IN & 0xf;
    ULINT factor = 10ULL;

    while (IN >>= 4) {
        res += (IN & 0xf) * factor;
        factor *= 10;
    }
    return res;
}

static inline LWORD __uint_to_bcd(ULINT IN){
    LWORD res = IN % 10;
    USINT shift = 4;

    while (IN /= 10) {
        res |= (IN % 10) << shift;
        shift += 4;
    }
    return res;
}


    /************/
    /*  MOVE_*  */
    /************/

/* some helpful __move_[ANY] functions, used in the *_TO_** and MOVE  standard functions */
/* e.g. __move_BOOL, __move_BYTE, __move_REAL, __move_TIME, ... */
#define __move_(TYPENAME)\
static inline TYPENAME __move_##TYPENAME(TYPENAME op1) {return op1;}
__ANY(__move_)








#include "iec_std_functions.h"
#include "iec_std_FB.h"

#endif /* _IEC_STD_LIB_H */
