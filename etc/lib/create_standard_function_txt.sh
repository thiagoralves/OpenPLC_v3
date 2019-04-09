#!/bin/sh 
#
# copyright 2011 Mario de Sousa (msousa@fe.up.pt)
#
# Offered to the public under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
# General Public License for more details.
#
# This code is made available on the understanding that it will not be
# used in safety-critical situations without a full and competent review.
#

echo "(************************************************)"
echo "(************************************************)"
echo "(***                                          ***)"
echo "(***    The IEC 61131-3 standard functions    ***)"
echo "(***                                          ***)"
echo "(***    Auto-generated file. Do not edit!     ***)"
echo "(***                                          ***)"
echo "(************************************************)"
echo "(************************************************)"


echo "{disable code generation}"

# Lets parse this file with the C pre-processor!
#cpp <<"END" | sed "s/ FUNCTION/\nFUNCTION/g" | grep -v '#'
# We usee gcc -E instead, as it seems cpp on OSX works differently! However, it needs an input file parameter...
#gcc -E <<"END" | sed "s/ FUNCTION/\nFUNCTION/g" | grep -v '#'
cpp <<"END" | sed "s/ FUNCTION/\nFUNCTION/g" | grep -v '#'

#define __DEFINE_CLASHING_FUNCTIONS 

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



/*****************************************************************/
/*****************************************************************/
/*****                                                       *****/
/*****                 IEC 61131-3                           *****/
/*****      S T A N D A R D     F U N C T I O N S            *****/
/*****                                                       *****/
/*****************************************************************/
/*****************************************************************/

/* NOTE: We only define and declare the explicitly typed standard functions
 *       (e.g., SIN_REAL, SIN_LREAL, ..., ADD_SINT, ADD_INT, ADD_LINT, ...)
 *       We do not declare/define the overloaded functions
 *       (SIN, ADD, ...). 
 *       When handling a call to an overloaded function, the iec2c compiler 
 *       will determine in stage3 the data type of the parameter being passed, 
 *       and in stage4 generate the C code to call the correct
 *       typed standard function.
 */

/* NOTE on explicit typing of:
 *           - Table 25 - Standard bit shift functions
 *           - Table 29 - Character string Functions
 *           - Table 27 - Standard selection functions (actually, just the MUX function)
 *
 *  In section 2.5.1.4 (Typing, overloading, and type conversion) of the IEC 61131-3 (version 2)
 *  of the standard, it is stated:
 * "A standard function, [...] is said to be overloaded when it
 * can operate on input data elements of various types within a generic type designator as defined in
 * 2.3.2. For instance, an overloaded addition function on generic type ANY_NUM can operate on data
 * of types LREAL, REAL, DINT, INT, and SINT."
 * [...]
 * "When a function which normally represents an overloaded operator is to be typed, i.e., the types
 * of its inputs and outputs restricted to a particular elementary or derived data type as defined in
 * 2.3, this shall be done by appending an "underline" character followed by the required type, as
 * shown in table 21."
 *
 * However, this explanation (as well as the example in table 21) only refers to functions where the same
 * generic data type is used for the single input and the output parameter.
 * How can we create explicitly types functions when this is not the case?
 * It does not seem to be covered by the standard.
 *
 * For this reason, we do not define the LEN_SINT, LEN_INT, LEN_STRING, LEN_[ANY_INT], LEN_[ANY_STRING] functions...
 */
 

/*****************************************/  
/*****************************************/  
/*  2.5.1.5.1 Type Conversion Functions  */
/*****************************************/  
/*****************************************/  

/* macros for non-extensible functions */
#define __function_1p(fname,to_TYPENAME,p1_NAME,p1_TYPENAME) \
FUNCTION fname : to_TYPENAME\
  VAR_INPUT p1_NAME : p1_TYPENAME; END_VAR\
  RETURN; \
END_FUNCTION\

#define __function_2p(fname,to_TYPENAME,p1_NAME,p1_TYPENAME,p2_NAME,p2_TYPENAME) \
FUNCTION fname : to_TYPENAME\
  VAR_INPUT p1_NAME : p1_TYPENAME; p2_NAME : p2_TYPENAME; END_VAR\
  RETURN; \
END_FUNCTION\

#define __function_3p(fname,to_TYPENAME,p1_NAME,p1_TYPENAME,p2_NAME,p2_TYPENAME,p3_NAME,p3_TYPENAME) \
FUNCTION fname : to_TYPENAME\
  VAR_INPUT p1_NAME : p1_TYPENAME; p2_NAME : p2_TYPENAME; p3_NAME : p3_TYPENAME; END_VAR\
  RETURN; \
END_FUNCTION\

#define __function_4p(fname,to_TYPENAME,p1_NAME,p1_TYPENAME,p2_NAME,p2_TYPENAME,p3_NAME,p3_TYPENAME,p4_NAME,p4_TYPENAME) \
FUNCTION fname : to_TYPENAME\
  VAR_INPUT p1_NAME : p1_TYPENAME; p2_NAME : p2_TYPENAME; p3_NAME : p3_TYPENAME; p4_NAME : p4_TYPENAME; END_VAR\
  RETURN; \
END_FUNCTION\


/* macro for extensible functions */
#define __function_1e(fname,to_TYPENAME,p1_NAME,p1_TYPENAME, FIRST_INDEX) \
FUNCTION fname : to_TYPENAME\
  VAR_INPUT p1_NAME FIRST_INDEX .. : p1_TYPENAME; END_VAR\
  RETURN; \
END_FUNCTION\

#define __function_2e(fname,to_TYPENAME,p1_NAME,p1_TYPENAME,p2_NAME,p2_TYPENAME, FIRST_INDEX) \
FUNCTION fname : to_TYPENAME\
  VAR_INPUT p1_NAME : p1_TYPENAME; p2_NAME FIRST_INDEX .. : p2_TYPENAME; END_VAR\
  RETURN; \
END_FUNCTION\






#define __to_anynum_(from_TYPENAME)   __ANY_NUM_1(__iec_,from_TYPENAME)
#define __to_anyint_(from_TYPENAME)   __ANY_INT_1(__iec_,from_TYPENAME)
#define __to_anybit_(from_TYPENAME)   __ANY_BIT_1(__iec_,from_TYPENAME)
#define __to_anynbit_(from_TYPENAME) __ANY_NBIT_1(__iec_,from_TYPENAME)
#define __to_anysint_(from_TYPENAME) __ANY_SINT_1(__iec_,from_TYPENAME)
#define __to_anyuint_(from_TYPENAME) __ANY_UINT_1(__iec_,from_TYPENAME)
#define __to_anyreal_(from_TYPENAME) __ANY_REAL_1(__iec_,from_TYPENAME)
#define __to_anydate_(from_TYPENAME) __ANY_DATE_1(__iec_,from_TYPENAME)
#define __to_time_(from_TYPENAME)    __iec_(TIME,from_TYPENAME)
#define __to_string_(from_TYPENAME)  __iec_(STRING,from_TYPENAME)


/*****************/
/*   *_TO_**     */
/*****************/
#define __iec_(to_TYPENAME,from_TYPENAME) __function_1p(from_TYPENAME##_TO_##to_TYPENAME, to_TYPENAME, IN, from_TYPENAME)

/******** [ANY_BIT]_TO_[ANY_NUM | ANY_BIT]   ************/ 
__ANY_BIT(__to_anynum_)
__ANY_BIT(__to_anybit_)

/******** [ANY_NUM]_TO_[ANY_NUM | ANY_BIT]   ************/ 
__ANY_NUM(__to_anynum_)
__ANY_NUM(__to_anybit_)


/******** [ANY_BIT | ANY_NUM]_TO_[TIME | ANY_DATE]   ************/ 
__ANY_BIT(__to_time_)
__ANY_NUM(__to_time_)
__ANY_BIT(__to_anydate_)
__ANY_NUM(__to_anydate_)


/******** [TIME | ANY_DATE]_TO_[ANY_BIT | ANY_NUM]   ************/ 
__to_anynum_(TIME)
__to_anybit_(TIME)
__ANY_DATE(__to_anynum_)
__ANY_DATE(__to_anybit_)


/******** [ANY_DATE]_TO_[ANY_DATE | TIME]   ************/ 
/* Not supported: DT_TO_TIME */
/*__iec_(to_TYPENAME,from_TYPENAME)*/
__iec_(DATE,DT)
__function_1p(DATE_AND_TIME_TO_DATE, DATE, IN, DT)
__iec_(DT,DT)
__iec_(TOD,DT)
__function_1p(DATE_AND_TIME_TO_TIME_OF_DAY, TOD, IN, DT)
/* Not supported: DATE_TO_TIME */
__iec_(DATE,DATE)
/* Not supported: DATE_TO_DT */
/* Not supported: DATE_TO_TOD */
/* Not supported: TOD_TO_TIME */
/* Not supported: TOD_TO_DATE */
/* Not supported: TOD_TO_DT */
__iec_(TOD,TOD)

/******** TIME_TO_[ANY_DATE]   ************/ 
/* Not supported: TIME_TO_DATE */
/* Not supported: TIME_TO_DT */
/* Not supported: TIME_TO_TOD */

/******** TIME_TO_TIME   ************/ 
__iec_(TIME,TIME)


/******** [ANY_BIT]_TO_STRING   ************/ 
__ANY_BIT(__to_string_)


/******** [ANY_NUM]_TO_STRING   ************/ 
__ANY_NUM(__to_string_)

/******** [ANY_DATE]_TO_STRING   ************/ 
__ANY_DATE(__to_string_)


/******** TIME_TO_STRING   ************/ 
__iec_(STRING,TIME)


/******** STRING_TO_[ANY_BIT]   ************/ 
__to_anybit_(STRING)


/******** STRING_TO_[ANY_NUM]   ************/ 
__to_anynum_(STRING)


/******** STRING_TO_[ANY_DATE]   ************/ 
__to_anydate_(STRING)

                                   
/******** STRING_TO_TIME   ************/ 
__iec_(TIME,STRING)

#undef __iec_



/**************/
/*   TRUNC    */
/**************/
/* overloaded function! */
#define __iec_(to_TYPENAME,from_TYPENAME) __function_1p(TRUNC, to_TYPENAME, IN, from_TYPENAME)
__ANY_REAL(__to_anyint_)
#undef __iec_


/*******************/
/*   *_TO_BCD_*    */
/*******************/
#define __iec_(to_TYPENAME,from_TYPENAME)\
__function_1p(from_TYPENAME##_TO_BCD_##to_TYPENAME, to_TYPENAME, IN, from_TYPENAME)  /* explicitly typed function */\
__function_1p(from_TYPENAME##_TO_BCD, to_TYPENAME, IN, from_TYPENAME)                /* overloaded function */ 
__ANY_UINT(__to_anynbit_)
#undef __iec_


/*******************/
/*   *_BCD_TO_*    */
/*******************/
#define __iec_(to_TYPENAME,from_TYPENAME)\
__function_1p(from_TYPENAME##_BCD_TO_##to_TYPENAME, to_TYPENAME, IN, from_TYPENAME)  /* explicitly typed function */\
__function_1p(BCD_TO_##to_TYPENAME, to_TYPENAME, IN, from_TYPENAME)                  /* overloaded function */ 

__ANY_NBIT(__to_anyuint_)
#undef __iec_

/***********************************/  
/***********************************/  
/*  2.5.1.5.2 Numerical Functions  */
/***********************************/  
/***********************************/  

/******************************************************************/
/***   Table 23 - Standard functions of one numeric variable    ***/
/******************************************************************/

  /**************/
  /*    ABS     */
  /**************/
#define __iec_(TYPENAME) \
__function_1p(ABS_##TYPENAME, TYPENAME, IN, TYPENAME)  /* explicitly typed function */\
__function_1p(ABS, TYPENAME, IN, TYPENAME)             /* overloaded function */ 
__ANY_REAL(__iec_)
__ANY_INT(__iec_)
#undef __iec_

  /**************/
  /*    SQRT    */
  /**************/
#define __iec_(TYPENAME) \
__function_1p(SQRT_##TYPENAME, TYPENAME, IN, TYPENAME)  /* explicitly typed function */\
__function_1p(SQRT, TYPENAME, IN, TYPENAME)             /* overloaded function */ 
__ANY_REAL(__iec_)
#undef __iec_

  /**************/
  /*     LN     */
  /**************/
#define __iec_(TYPENAME) \
__function_1p(LN_##TYPENAME, TYPENAME, IN, TYPENAME)  /* explicitly typed function */\
__function_1p(LN, TYPENAME, IN, TYPENAME)             /* overloaded function */ 
__ANY_REAL(__iec_)
#undef __iec_

  /**************/
  /*     LOG    */
  /**************/
#define __iec_(TYPENAME) \
__function_1p(LOG_##TYPENAME, TYPENAME, IN, TYPENAME)  /* explicitly typed function */\
__function_1p(LOG, TYPENAME, IN, TYPENAME)             /* overloaded function */ 
__ANY_REAL(__iec_)
#undef __iec_

  /**************/
  /*     EXP    */
  /**************/
#define __iec_(TYPENAME) \
__function_1p(EXP_##TYPENAME, TYPENAME, IN, TYPENAME)  /* explicitly typed function */\
__function_1p(EXP, TYPENAME, IN, TYPENAME)             /* overloaded function */ 
__ANY_REAL(__iec_)
#undef __iec_

  /**************/
  /*     SIN    */
  /**************/
#define __iec_(TYPENAME) \
__function_1p(SIN_##TYPENAME, TYPENAME, IN, TYPENAME)  /* explicitly typed function */\
__function_1p(SIN, TYPENAME, IN, TYPENAME)             /* overloaded function */ 
__ANY_REAL(__iec_)
#undef __iec_

  /**************/
  /*     COS    */
  /**************/
#define __iec_(TYPENAME) \
__function_1p(COS_##TYPENAME, TYPENAME, IN, TYPENAME)  /* explicitly typed function */\
__function_1p(COS, TYPENAME, IN, TYPENAME)             /* overloaded function */ 
__ANY_REAL(__iec_)
#undef __iec_

  /**************/
  /*     TAN    */
  /**************/
#define __iec_(TYPENAME) \
__function_1p(TAN_##TYPENAME, TYPENAME, IN, TYPENAME)  /* explicitly typed function */\
__function_1p(TAN, TYPENAME, IN, TYPENAME)             /* overloaded function */ 
__ANY_REAL(__iec_)
#undef __iec_

  /**************/
  /*    ASIN    */
  /**************/
#define __iec_(TYPENAME) \
__function_1p(ASIN_##TYPENAME, TYPENAME, IN, TYPENAME)  /* explicitly typed function */\
__function_1p(ASIN, TYPENAME, IN, TYPENAME)             /* overloaded function */ 
__ANY_REAL(__iec_)
#undef __iec_

  /**************/
  /*    ACOS    */
  /**************/
#define __iec_(TYPENAME) \
__function_1p(ACOS_##TYPENAME, TYPENAME, IN, TYPENAME)  /* explicitly typed function */\
__function_1p(ACOS, TYPENAME, IN, TYPENAME)             /* overloaded function */ 
__ANY_REAL(__iec_)
#undef __iec_

  /**************/
  /*    ATAN    */
  /**************/
#define __iec_(TYPENAME) \
__function_1p(ATAN_##TYPENAME, TYPENAME, IN, TYPENAME)  /* explicitly typed function */\
__function_1p(ATAN, TYPENAME, IN, TYPENAME)             /* overloaded function */ 
__ANY_REAL(__iec_)
#undef __iec_



/*****************************************************/
/***   Table 24 - Standard arithmetic functions    ***/
/*****************************************************/

#define __arith_expand(fname,TYPENAME) \
__function_1e(fname##_##TYPENAME, TYPENAME, IN, TYPENAME, 1)     /* explicitly typed function */\
__function_1e(fname, TYPENAME, IN, TYPENAME, 1)                  /* overloaded function */ 


#define __arith_static(fname,TYPENAME) \
__function_1p(fname##_##TYPENAME, TYPENAME, IN, TYPENAME)  /* explicitly typed function */\
__function_1p(fname, TYPENAME, IN, TYPENAME)               /* overloaded function */ 

  /**************/
  /*     ADD    */
  /**************/
#define __iec_(TYPENAME) __arith_expand(ADD, TYPENAME)
__ANY_NUM(__iec_)
#undef __iec_

  /**************/
  /*     MUL    */
  /**************/
#define __iec_(TYPENAME) __arith_expand(MUL, TYPENAME)
__ANY_NUM(__iec_)
#undef __iec_

  /**************/
  /*     SUB    */
  /**************/
#define __iec_(TYPENAME)\
__function_2p(SUB_##TYPENAME, TYPENAME, IN1, TYPENAME, IN2, TYPENAME)  /* explicitly typed function */\
__function_2p(SUB, TYPENAME, IN1, TYPENAME, IN2, TYPENAME)             /* overloaded function */ 
__ANY_NUM(__iec_)
#undef __iec_

  /**************/
  /*     DIV    */
  /**************/
#define __iec_(TYPENAME)\
__function_2p(DIV_##TYPENAME, TYPENAME, IN1, TYPENAME, IN2, TYPENAME)  /* explicitly typed function */\
__function_2p(DIV, TYPENAME, IN1, TYPENAME, IN2, TYPENAME)             /* overloaded function */ 
__ANY_NUM(__iec_)
#undef __iec_

  /**************/
  /*     MOD    */
  /**************/
#ifdef __DEFINE_CLASHING_FUNCTIONS  
#define __iec_(TYPENAME)\
__function_2p(MOD_##TYPENAME, TYPENAME, IN1, TYPENAME, IN2, TYPENAME)  /* explicitly typed function */\
__function_2p(MOD, TYPENAME, IN1, TYPENAME, IN2, TYPENAME)             /* overloaded function */ 
__ANY_INT(__iec_)
#undef __iec_
#endif

  /**************/
  /*    EXPT    */
  /**************/
#define __in1_anyreal_(in2_TYPENAME)   __ANY_REAL_1(__iec_,in2_TYPENAME)
#define __iec_(in1_TYPENAME,in2_TYPENAME) \
__function_2p(EXPT, in1_TYPENAME, IN1, in1_TYPENAME, IN2, in2_TYPENAME)             /* overloaded function */ 
__ANY_NUM(__in1_anyreal_)
#undef __iec_

  /***************/
  /*     MOVE    */
  /***************/
#define __iec_(TYPENAME) __arith_static(MOVE, TYPENAME)
__ANY(__iec_)
#undef __iec_




/***********************************/  
/***********************************/  
/*  2.5.1.5.3 Bit String Functions */
/***********************************/  
/***********************************/  

/****************************************************/
/***   Table 25 - Standard bit shift functions    ***/
/****************************************************/
/* We do not delcare explcitly typed versions of the functions in table 29.
 * See note at top of this file regarding explicitly typed functions for more details.
 */
#define __in1_anybit_(in2_TYPENAME)   __ANY_BIT_1(__iec_,in2_TYPENAME)


  /**************/
  /*     SHL    */
  /**************/
#define __iec_(in1_TYPENAME,in2_TYPENAME) __function_2p(SHL, in1_TYPENAME, IN, in1_TYPENAME, N, in2_TYPENAME)             /* overloaded function */ 
__ANY_INT(__in1_anybit_)
#undef __iec_

  /**************/
  /*     SHR    */
  /**************/
#define __iec_(in1_TYPENAME,in2_TYPENAME) __function_2p(SHR, in1_TYPENAME, IN, in1_TYPENAME, N, in2_TYPENAME)             /* overloaded function */ 
__ANY_INT(__in1_anybit_)
#undef __iec_

  /**************/
  /*     ROR    */
  /**************/
#define __iec_(in1_TYPENAME,in2_TYPENAME) __function_2p(ROR, in1_TYPENAME, IN, in1_TYPENAME, N, in2_TYPENAME)             /* overloaded function */ 
__ANY_INT(__in1_anybit_)
#undef __iec_

  /**************/
  /*     ROL    */
  /**************/
#define __iec_(in1_TYPENAME,in2_TYPENAME) __function_2p(ROL, in1_TYPENAME, IN, in1_TYPENAME, N, in2_TYPENAME)             /* overloaded function */ 
__ANY_INT(__in1_anybit_)
#undef __iec_



/*********************/
/***   Table 26    ***/
/*********************/

  /**************/
  /*     AND    */
  /**************/
#ifdef __DEFINE_CLASHING_FUNCTIONS  
#define __iec_(TYPENAME) \
__function_1e(AND, TYPENAME, IN, TYPENAME, 1)                /* overloaded function */ \
__function_1e(AND_##TYPENAME, TYPENAME, IN, TYPENAME, 1)     /* explicitly typed function */ 
__ANY_BIT(__iec_)
#undef __iec_
#endif

  /*************/
  /*     OR    */
  /*************/
#ifdef __DEFINE_CLASHING_FUNCTIONS  
#define __iec_(TYPENAME) \
__function_1e(OR, TYPENAME, IN, TYPENAME, 1)                /* overloaded function */ \
__function_1e(OR_##TYPENAME, TYPENAME, IN, TYPENAME, 1)     /* explicitly typed function */ 
__ANY_BIT(__iec_)
#undef __iec_
#endif

  /**************/
  /*     XOR    */
  /**************/
#ifdef __DEFINE_CLASHING_FUNCTIONS  
#define __iec_(TYPENAME) \
__function_1e(XOR, TYPENAME, IN, TYPENAME, 1)                /* overloaded function */ \
__function_1e(XOR_##TYPENAME, TYPENAME, IN, TYPENAME, 1)     /* explicitly typed function */ 
__ANY_BIT(__iec_)
#undef __iec_
#endif

  /**************/
  /*     NOT    */
  /**************/
#ifdef __DEFINE_CLASHING_FUNCTIONS  
#define __iec_(TYPENAME) \
__function_1p(NOT, TYPENAME, IN, TYPENAME)             /* overloaded function */ \
__function_1p(NOT_##TYPENAME, TYPENAME, IN, TYPENAME)  /* explicitly typed function */ 
__ANY_BIT(__iec_)
#undef __iec_
#endif






/***************************************************/  
/***************************************************/  
/*  2.5.1.5.4  Selection and comparison Functions  */
/***************************************************/  
/***************************************************/  

/*********************/
/***   Table 27    ***/
/*********************/

    /**************/
    /*    SEL     */
    /**************/
/* The standard states that the inputs for SEL and MUX must be named starting off from 0,
 * unlike remaining functions, that start off at 1.
 */
#define __iec_(TYPENAME) \
__function_3p(SEL, TYPENAME, G, BOOL, IN0, TYPENAME, IN1, TYPENAME)             /* overloaded function */ \
__function_3p(SEL_##TYPENAME, TYPENAME, G, BOOL, IN0, TYPENAME, IN1, TYPENAME)  /* explicitly typed function */ 
__ANY(__iec_)
#undef __iec_

    /**************/
    /*     MAX    */
    /**************/
/* Should be for: ANY_ELEMENTARY, but we currently do not support WSTRING yet... */
/* However, we can call __ANY_ELEMENTARY since the __ANY_STRING macro does not call DO(WSTRING) */
#define __iec_(TYPENAME) \
__function_1e(MAX, TYPENAME, IN, TYPENAME, 1)             /* overloaded function */ \
__function_1e(MAX_##TYPENAME, TYPENAME, IN, TYPENAME, 1)  /* explicitly typed function */ 
__ANY_ELEMENTARY(__iec_)
#undef __iec_

    /**************/
    /*     MIN    */
    /**************/
/* Should be for: ANY_ELEMENTARY, but we currently do not support WSTRING yet... */
/* However, we can call __ANY_ELEMENTARY since the __ANY_STRING macro does not call DO(WSTRING) */
#define __iec_(TYPENAME) \
__function_1e(MIN, TYPENAME, IN, TYPENAME, 1)             /* overloaded function */ \
__function_1e(MIN_##TYPENAME, TYPENAME, IN, TYPENAME, 1)  /* explicitly typed function */ 
__ANY_ELEMENTARY(__iec_)
#undef __iec_

    /**************/
    /*   LIMIT    */
    /**************/
/* Should be for: ANY_ELEMENTARY, but we currently do not support WSTRING yet... */
/* However, we can call __ANY_ELEMENTARY since the __ANY_STRING macro does not call DO(WSTRING) */
#define __iec_(TYPENAME) \
__function_3p(LIMIT, TYPENAME, MN, TYPENAME, IN, TYPENAME, MX, TYPENAME)             /* overloaded function */ \
__function_3p(LIMIT_##TYPENAME, TYPENAME, MN, TYPENAME, IN, TYPENAME, MX, TYPENAME)  /* explicitly typed function */ 
__ANY_ELEMENTARY(__iec_)
#undef __iec_

    /**************/
    /*     MUX    */
    /**************/
/* The standard states that the inputs for SEL and MUX must be named starting off from 0,
 * unlike remaining functions, that start off at 1.
 */
/* The standard considers the explicit typing of MUX function as a special case... 
 * It should look like: 
 *         MUX_SINT_REAL, MUX_SINT_STRING, MUX_SINT_[ANY]
 *         MUX_INT_REAL,  MUX_INT_STRING,  MUX_INT_[ANY]
 *         MUX_[ANY_INT]_[ANY]
 */
#define __in1_anyint_(in2_TYPENAME)   __ANY_INT_1(__iec_,in2_TYPENAME)
#define __iec_(in1_TYPENAME,in2_TYPENAME) \
__function_2e(MUX, in2_TYPENAME, K, in1_TYPENAME, IN, in2_TYPENAME, 0)                                  /* overloaded function */ \
__function_2e(MUX_##in1_TYPENAME##_##in2_TYPENAME, in2_TYPENAME, K, in1_TYPENAME, IN, in2_TYPENAME, 0)  /* explicitly typed function */ 
__ANY(__in1_anyint_)
#undef __iec_



/******************************************/
/***             Table 28               ***/
/***   Standard comparison functions    ***/
/******************************************/

    /**************/
    /*     GT     */
    /**************/
/* Should be for: ANY_ELEMENTARY, but we currently do not support WSTRING yet... */
/* However, we can call __ANY_ELEMENTARY since the __ANY_STRING macro does not call DO(WSTRING) */
#define __iec_(TYPENAME) \
__function_1e(GT, BOOL, IN, TYPENAME, 1)             /* overloaded function */ \
__function_1e(GT_##TYPENAME, BOOL, IN, TYPENAME, 1)  /* explicitly typed function */ 
__ANY_ELEMENTARY(__iec_)
#undef __iec_

    /**************/
    /*     GE     */
    /**************/
/* Should be for: ANY_ELEMENTARY, but we currently do not support WSTRING yet... */
/* However, we can call __ANY_ELEMENTARY since the __ANY_STRING macro does not call DO(WSTRING) */
#define __iec_(TYPENAME) \
__function_1e(GE, BOOL, IN, TYPENAME, 1)             /* overloaded function */ \
__function_1e(GE_##TYPENAME, BOOL, IN, TYPENAME, 1)  /* explicitly typed function */ 
__ANY_ELEMENTARY(__iec_)
#undef __iec_

    /**************/
    /*     EQ     */
    /**************/
/* Should be for: ANY_ELEMENTARY, but we currently do not support WSTRING yet... */
/* However, we can call __ANY_ELEMENTARY since the __ANY_STRING macro does not call DO(WSTRING) */
#define __iec_(TYPENAME) \
__function_1e(EQ, BOOL, IN, TYPENAME, 1)             /* overloaded function */ \
__function_1e(EQ_##TYPENAME, BOOL, IN, TYPENAME, 1)  /* explicitly typed function */ 
__ANY_ELEMENTARY(__iec_)
#undef __iec_

    /**************/
    /*     LT     */
    /**************/
/* Should be for: ANY_ELEMENTARY, but we currently do not support WSTRING yet... */
/* However, we can call __ANY_ELEMENTARY since the __ANY_STRING macro does not call DO(WSTRING) */
#define __iec_(TYPENAME) \
__function_1e(LT, BOOL, IN, TYPENAME, 1)             /* overloaded function */ \
__function_1e(LT_##TYPENAME, BOOL, IN, TYPENAME, 1)  /* explicitly typed function */ 
__ANY_ELEMENTARY(__iec_)
#undef __iec_

    /**************/
    /*     LE     */
    /**************/
/* Should be for: ANY_ELEMENTARY, but we currently do not support WSTRING yet... */
/* However, we can call __ANY_ELEMENTARY since the __ANY_STRING macro does not call DO(WSTRING) */
#define __iec_(TYPENAME) \
__function_1e(LE, BOOL, IN, TYPENAME, 1)             /* overloaded function */ \
__function_1e(LE_##TYPENAME, BOOL, IN, TYPENAME, 1)  /* explicitly typed function */ 
__ANY_ELEMENTARY(__iec_)
#undef __iec_

    /**************/
    /*     NE     */
    /**************/
/* Should be for: ANY_ELEMENTARY, but we currently do not support WSTRING yet... */
/* However, we can call __ANY_ELEMENTARY since the __ANY_STRING macro does not call DO(WSTRING) */
#define __iec_(TYPENAME) \
__function_2p(NE, BOOL, IN1, TYPENAME, IN2, TYPENAME)            /* overloaded function */ \
__function_2p(NE_##TYPENAME, BOOL, IN1, TYPENAME, IN2, TYPENAME) /* explicitly typed function */
__ANY_ELEMENTARY(__iec_)
#undef __iec_



/*********************************************/  
/*********************************************/  
/*  2.5.1.5.5   Character string  Functions  */
/*********************************************/  
/*********************************************/  

/*************************************/
/***           Table 29            ***/
/***  Character string  Functions  ***/
/*************************************/

/* We do not delcare explcitly typed versions of the functions in table 29.
 * See note at top of this file regarding explicitly typed functions for more details.
 */
 
#define __A_anyint_(B_TYPENAME)   __ANY_INT_1(__iec_,B_TYPENAME)



    /***************/
    /*     LEN     */
    /***************/
#define __iec_(A_TYPENAME,B_TYPENAME) \
__function_1p(LEN, A_TYPENAME, IN, B_TYPENAME)                                  /* overloaded function */ 
__ANY_STRING(__A_anyint_)
#undef __iec_


    /****************/
    /*     LEFT     */
    /****************/
#define __iec_(A_TYPENAME,B_TYPENAME) \
__function_2p(LEFT, B_TYPENAME, IN, B_TYPENAME, L, A_TYPENAME)                  /* overloaded function */ 
__ANY_STRING(__A_anyint_)
#undef __iec_


    /*****************/
    /*     RIGHT     */
    /*****************/
#define __iec_(A_TYPENAME,B_TYPENAME) \
__function_2p(RIGHT, B_TYPENAME, IN, B_TYPENAME, L, A_TYPENAME)                 /* overloaded function */ 
__ANY_STRING(__A_anyint_)
#undef __iec_


    /***************/
    /*     MID     */
    /***************/
#define __iec_(A_TYPENAME,B_TYPENAME) \
__function_3p(MID, B_TYPENAME, IN, B_TYPENAME, L, A_TYPENAME, P, A_TYPENAME)    /* overloaded function */ 
__ANY_STRING(__A_anyint_)
#undef __iec_


    /******************/
    /*     CONCAT     */
    /******************/
#define __iec_(TYPENAME) \
__function_1e(CONCAT, TYPENAME, IN, TYPENAME, 1)    /* overloaded function */ 
__ANY_STRING(__iec_)
#undef __iec_


    /******************/
    /*     INSERT     */
    /******************/
#define __iec_(A_TYPENAME,B_TYPENAME) \
__function_3p(INSERT, B_TYPENAME, IN1, B_TYPENAME, IN2, B_TYPENAME, P, A_TYPENAME)    /* overloaded function */ 
__ANY_STRING(__A_anyint_)
#undef __iec_


    /******************/
    /*     DELETE     */
    /******************/
#define __iec_(A_TYPENAME,B_TYPENAME) \
__function_3p(DELETE, B_TYPENAME, IN, B_TYPENAME, L, A_TYPENAME, P, A_TYPENAME)    /* overloaded function */ 
__ANY_STRING(__A_anyint_)
#undef __iec_


    /*******************/
    /*     REPLACE     */
    /*******************/
#define __iec_(A_TYPENAME,B_TYPENAME) \
__function_4p(REPLACE, B_TYPENAME, IN1, B_TYPENAME, IN2, B_TYPENAME, L, A_TYPENAME, P, A_TYPENAME)    /* overloaded function */ 
__ANY_STRING(__A_anyint_)
#undef __iec_


    /****************/
    /*     FIND     */
    /****************/
#define __iec_(A_TYPENAME,B_TYPENAME) \
__function_2p(FIND, A_TYPENAME, IN1, B_TYPENAME, IN2, B_TYPENAME)                 /* overloaded function */ 
__ANY_STRING(__A_anyint_)
#undef __iec_


/*********************************************/  
/*********************************************/  
/*  2.5.1.5.6  Functions of time data types  */
/*********************************************/  
/*********************************************/  

/**************************************/
/***           Table 30             ***/
/***  Functions of time data types  ***/
/**************************************/

__function_2p(ADD_TIME, TIME, IN1, TIME, IN2, TIME)
__function_2p(ADD_TOD_TIME, TOD, IN1, TOD, IN2, TIME)
__function_2p(ADD_DT_TIME, DT, IN1, DT, IN2, TIME)

__function_2p(SUB_TIME, TIME, IN1, TIME, IN2, TIME)
__function_2p(SUB_DATE_DATE, TIME, IN1, DATE, IN2, DATE)
__function_2p(SUB_TOD_TIME, TOD, IN1, TOD, IN2, TIME)
__function_2p(SUB_TOD_TOD, TIME, IN1, TOD, IN2, TOD)
__function_2p(SUB_DT_TIME, DT, IN1, DT, IN2, TIME)
__function_2p(SUB_DT_DT, TIME, IN1, DT, IN2, DT)


/***  MULTIME  ***/
#define __iec_(TYPENAME) \
__function_2p(MULTIME_##TYPENAME, TIME, IN1, TIME, IN2, TYPENAME)    /* explicitly typed function */\
__function_2p(MULTIME, TIME, IN1, TIME, IN2, TYPENAME)               /* overloaded function */ 
__ANY_NUM(__iec_)
#undef __iec_

/***  MUL  ***/
/* NOTE: We can not include the explicitly typed function
 * __function_2p(DIV_##TYPENAME, TIME, IN1, TIME, IN2, TYPENAME)    
 * as it would clash with another function with the exact same name declared above
 * __function_2p(DIV_##TYPENAME, TYPENAME, IN1, TYPENAME, IN2, TYPENAME)
 * and the standard does not define this explicitly defined function as being overloaded 
 * (i.e. having an ANY_***) input or output parameter
 * Note that our compiler would nevertheless work just fine with this overloaded declaration.
 */
#define __iec_(TYPENAME) \
__function_2p(MUL, TIME, IN1, TIME, IN2, TYPENAME)               /* overloaded function */ 
__ANY_NUM(__iec_)
#undef __iec_

/***  DIVTIME  ***/
#define __iec_(TYPENAME) \
__function_2p(DIVTIME_##TYPENAME, TIME, IN1, TIME, IN2, TYPENAME)    /* explicitly typed function */\
__function_2p(DIVTIME, TIME, IN1, TIME, IN2, TYPENAME)               /* overloaded function */ 
__ANY_NUM(__iec_)
#undef __iec_

/***  DIV  ***/
/* NOTE: We can not include the explicitly typed function
 * __function_2p(DIV_##TYPENAME, TIME, IN1, TIME, IN2, TYPENAME)    
 * as it would clash with another function with the exact same name declared above
 * __function_2p(DIV_##TYPENAME, TYPENAME, IN1, TYPENAME, IN2, TYPENAME)
 * and the standard does not define this explicitly defined function as being overloaded 
 * (i.e. having an ANY_***) input or output parameter
 * Note that our compiler would nevertheless work just fine with this overloaded declaration.
 */
#define __iec_(TYPENAME) \
__function_2p(DIV, TIME, IN1, TIME, IN2, TYPENAME)               /* overloaded function */ 
__ANY_NUM(__iec_)
#undef __iec_


/*** CONCAT_DATE_TOD ***/
__function_2p(CONCAT_DATE_TOD, DT, IN1, DATE, IN2, TOD)




/****************************************************/  
/****************************************************/  
/*  2.5.1.5.6   Functions of enumerated data types  */
/****************************************************/  
/****************************************************/  

/********************************************/
/***              Table 31                ***/
/***  Functions of enumerated data types  ***/
/********************************************/

/* Do we support this? */

END

echo "{enable code generation}"
echo
