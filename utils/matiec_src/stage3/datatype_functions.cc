/*
 *  matiec - a compiler for the programming languages defined in IEC 61131-3
 *
 *  Copyright (C) 2009-2012  Mario de Sousa (msousa@fe.up.pt)
 *  Copyright (C) 2012       Manuele Conti  (conti.ma@alice.it)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * This code is made available on the understanding that it will not be
 * used in safety-critical situations without a full and competent review.
 */

#include "datatype_functions.hh"
#include "../absyntax_utils/absyntax_utils.hh"
#include <vector>







/* Macro that expand to subtypes */
/* copied from matiec/lib/create_standard_functions_txt.sh */
#define __ANY(DO)                 __ANY_DERIVED(DO) __ANY_ELEMENTARY(DO)
#define __ANY_DERIVED(DO)
#define __ANY_ELEMENTARY(DO)      __ANY_MAGNITUDE(DO) __ANY_BIT(DO) __ANY_STRING(DO) __ANY_DATE(DO)
#define __ANY_MAGNITUDE(DO)       __ANY_NUM(DO) DO(time)
#define __ANY_BIT(DO)             __ANY_NBIT(DO) DO(bool)
#define __ANY_NBIT(DO)            DO(byte) DO(word) DO(dword) DO(lword)
//#define __ANY_STRING(DO)          DO(string) DO(wstring)
#define __ANY_STRING(DO)          DO(string)
#define __ANY_DATE(DO)            DO(date) DO(tod) DO(dt)
#define __ANY_NUM(DO)             __ANY_REAL(DO) __ANY_INT(DO)
#define __ANY_REAL(DO)            DO(real) DO(lreal)
#define __ANY_INT(DO)             __ANY_SINT(DO) __ANY_UINT(DO)
#define __ANY_SINT(DO)            DO(sint) DO(int) DO(dint) DO(lint)
#define __ANY_UINT(DO)            DO(usint) DO(uint) DO(udint) DO(ulint)

#define __ANY_1(DO,P1)            __ANY_DERIVED_1(DO,P1) __ANY_ELEMENTARY_1(DO,P1)
#define __ANY_DERIVED_1(DO,P1)
#define __ANY_ELEMENTARY_1(DO,P1) __ANY_MAGNITUDE_1(DO,P1) __ANY_BIT_1(DO,P1) __ANY_STRING_1(DO,P1) __ANY_DATE_1(DO,P1)
#define __ANY_MAGNITUDE_1(DO,P1)  __ANY_NUM_1(DO,P1) DO(time,P1)
#define __ANY_BIT_1(DO,P1)        __ANY_NBIT_1(DO,P1) DO(bool,P1)
#define __ANY_NBIT_1(DO,P1)       DO(byte,P1) DO(word,P1) DO(dword,P1) DO(lword,P1)
// #define __ANY_STRING_1(DO,P1)     DO(string,P1) DO(wstring,P1)
#define __ANY_STRING_1(DO,P1)     DO(string,P1)
#define __ANY_DATE_1(DO,P1)       DO(date,P1) DO(tod,P1) DO(dt,P1)
#define __ANY_NUM_1(DO,P1)        __ANY_REAL_1(DO,P1) __ANY_INT_1(DO,P1)
#define __ANY_REAL_1(DO,P1)       DO(real,P1) DO(lreal,P1)
#define __ANY_INT_1(DO,P1)        __ANY_SINT_1(DO,P1) __ANY_UINT_1(DO,P1)
#define __ANY_SINT_1(DO,P1)       DO(sint,P1) DO(int,P1) DO(dint,P1) DO(lint,P1)
#define __ANY_UINT_1(DO,P1)       DO(usint,P1) DO(uint,P1) DO(udint,P1) DO(ulint,P1)


/**************************************************************/
/**************************************************************/
/**************************************************************/
/*******  TABLE 24: Standard arithmetic functions       *******/
/*******    merged with                                 *******/
/*******  TABLE 30: Functions of time data types        *******/
/**************************************************************/
/**************************************************************/
/**************************************************************/

/* NOTE: IEC 61131-3 v2 declares that using implicit operations ('+', '-', '*', '/') on ANYTIME data types is
 *       valid, but deprecated, suposedly meaning that they will be removed in the following version of the standard.
 *       However, the current draft version of IEC 61131-3 v3 still allows this use, and no longer declares these
 *       implicit operations as deprecated.
 *       Because of this, and although we are implementing v2 of the standard, we will no longer mark these 
 *       operations as deprecated.
 */
  #define ANYTIME_OPER_DEPRECATION_STATUS widen_entry::ok
//#define ANYTIME_OPER_DEPRECATION_STATUS widen_entry::deprecated


const struct widen_entry widen_ADD_table[] = {
#define __add(TYPE)       \
    { &get_datatype_info_c::TYPE##_type_name,        &get_datatype_info_c::TYPE##_type_name,          &get_datatype_info_c::TYPE##_type_name,       widen_entry::ok                 }, \
    { &get_datatype_info_c::safe##TYPE##_type_name,  &get_datatype_info_c::TYPE##_type_name,          &get_datatype_info_c::TYPE##_type_name,       widen_entry::ok                 }, \
    { &get_datatype_info_c::TYPE##_type_name,        &get_datatype_info_c::safe##TYPE##_type_name,    &get_datatype_info_c::TYPE##_type_name,       widen_entry::ok                 }, \
    { &get_datatype_info_c::safe##TYPE##_type_name,  &get_datatype_info_c::safe##TYPE##_type_name,    &get_datatype_info_c::safe##TYPE##_type_name, widen_entry::ok                 },
    __ANY_NUM(__add)
#undef __add

    /*******************************************/
    /*******************************************/
    /*** Operations with TIME, DT and TOD... ***/
    /*******************************************/
    /*******************************************/ 
    { &get_datatype_info_c::time_type_name,          &get_datatype_info_c::time_type_name,            &get_datatype_info_c::time_type_name,         widen_entry::ok                 },
    { &get_datatype_info_c::tod_type_name,           &get_datatype_info_c::time_type_name,            &get_datatype_info_c::tod_type_name,          ANYTIME_OPER_DEPRECATION_STATUS },
    /* NOTE: the standard des not explicitly support the following semantics. However, since 'addition' is supposed to be commutative, we add it anyway... */
    /* not currently supported by stage4, so it is best no tto add it for now... */
//  { &get_datatype_info_c::time_type_name,          &get_datatype_info_c::tod_type_name,             &get_datatype_info_c::tod_type_name,          ANYTIME_OPER_DEPRECATION_STATUS },
    { &get_datatype_info_c::dt_type_name,            &get_datatype_info_c::time_type_name,            &get_datatype_info_c::dt_type_name,           ANYTIME_OPER_DEPRECATION_STATUS },         
    /* NOTE: the standard des not explicitly support the following semantics. However, since 'addition' is supposed to be commutative, we add it anyway... */
    /* not currently supported by stage4, so it is best no tto add it for now... */
//  { &get_datatype_info_c::time_type_name,          &get_datatype_info_c::dt_type_name,              &get_datatype_info_c::dt_type_name,           ANYTIME_OPER_DEPRECATION_STATUS },         

    /*******************************/
    /* SAFE version on the left... */
    /*******************************/
    { &get_datatype_info_c::safetime_type_name,      &get_datatype_info_c::time_type_name,            &get_datatype_info_c::time_type_name,         widen_entry::ok                 },
    { &get_datatype_info_c::safetod_type_name,       &get_datatype_info_c::time_type_name,            &get_datatype_info_c::tod_type_name,          ANYTIME_OPER_DEPRECATION_STATUS },
    /* NOTE: the standard des not explicitly support the following semantics. However, since 'addition' is supposed to be commutative, we add it anyway... */
    /* not currently supported by stage4, so it is best no tto add it for now... */
//  { &get_datatype_info_c::safetime_type_name,      &get_datatype_info_c::tod_type_name,             &get_datatype_info_c::tod_type_name,          ANYTIME_OPER_DEPRECATION_STATUS },
    { &get_datatype_info_c::safedt_type_name,        &get_datatype_info_c::time_type_name,            &get_datatype_info_c::dt_type_name,           ANYTIME_OPER_DEPRECATION_STATUS },         
    /* NOTE: the standard des not explicitly support the following semantics. However, since 'addition' is supposed to be commutative, we add it anyway... */
    /* not currently supported by stage4, so it is best no tto add it for now... */
//  { &get_datatype_info_c::safetime_type_name,      &get_datatype_info_c::dt_type_name,              &get_datatype_info_c::dt_type_name,           ANYTIME_OPER_DEPRECATION_STATUS },         

    /********************************/
    /* SAFE version on the right... */
    /********************************/
    { &get_datatype_info_c::time_type_name,          &get_datatype_info_c::safetime_type_name,        &get_datatype_info_c::time_type_name,         widen_entry::ok         },
    { &get_datatype_info_c::tod_type_name,           &get_datatype_info_c::safetime_type_name,        &get_datatype_info_c::tod_type_name,          ANYTIME_OPER_DEPRECATION_STATUS },
    /* NOTE: the standard des not explicitly support the following semantics. However, since 'addition' is supposed to be commutative, we add it anyway... */
    /* not currently supported by stage4, so it is best no tto add it for now... */
//  { &get_datatype_info_c::time_type_name,          &get_datatype_info_c::safetod_type_name,         &get_datatype_info_c::tod_type_name,          ANYTIME_OPER_DEPRECATION_STATUS },
    { &get_datatype_info_c::dt_type_name,            &get_datatype_info_c::safetime_type_name,        &get_datatype_info_c::dt_type_name,           ANYTIME_OPER_DEPRECATION_STATUS },         
    /* NOTE: the standard des not explicitly support the following semantics. However, since 'addition' is supposed to be commutative, we add it anyway... */
    /* not currently supported by stage4, so it is best no tto add it for now... */
//  { &get_datatype_info_c::time_type_name,          &get_datatype_info_c::safedt_type_name,          &get_datatype_info_c::dt_type_name,           ANYTIME_OPER_DEPRECATION_STATUS },         

    /*************************************/
    /* SAFE version on left and right... */
    /*************************************/
    { &get_datatype_info_c::safetime_type_name,      &get_datatype_info_c::safetime_type_name,        &get_datatype_info_c::safetime_type_name,     widen_entry::ok                 },
    { &get_datatype_info_c::safetod_type_name,       &get_datatype_info_c::safetime_type_name,        &get_datatype_info_c::safetod_type_name,      ANYTIME_OPER_DEPRECATION_STATUS },
    /* NOTE: the standard des not explicitly support the following semantics. However, since 'addition' is supposed to be commutative, we add it anyway... */
    /* not currently supported by stage4, so it is best no tto add it for now... */
//  { &get_datatype_info_c::safetime_type_name,      &get_datatype_info_c::safetod_type_name,         &get_datatype_info_c::safetod_type_name,      ANYTIME_OPER_DEPRECATION_STATUS },
    { &get_datatype_info_c::safedt_type_name,        &get_datatype_info_c::safetime_type_name,        &get_datatype_info_c::safedt_type_name,       ANYTIME_OPER_DEPRECATION_STATUS },         
    /* NOTE: the standard des not explicitly support the following semantics. However, since 'addition' is supposed to be commutative, we add it anyway... */
    /* not currently supported by stage4, so it is best no tto add it for now... */
//  { &get_datatype_info_c::safetime_type_name,      &get_datatype_info_c::safedt_type_name,          &get_datatype_info_c::safedt_type_name,       ANYTIME_OPER_DEPRECATION_STATUS },
   
    { NULL, NULL, NULL, widen_entry::ok },
};







const struct widen_entry widen_SUB_table[] = {
#define __sub(TYPE)       \
    { &get_datatype_info_c::TYPE##_type_name,        &get_datatype_info_c::TYPE##_type_name,          &get_datatype_info_c::TYPE##_type_name,       widen_entry::ok                 }, \
    { &get_datatype_info_c::safe##TYPE##_type_name,  &get_datatype_info_c::TYPE##_type_name,          &get_datatype_info_c::TYPE##_type_name,       widen_entry::ok                 }, \
    { &get_datatype_info_c::TYPE##_type_name,        &get_datatype_info_c::safe##TYPE##_type_name,    &get_datatype_info_c::TYPE##_type_name,       widen_entry::ok                 }, \
    { &get_datatype_info_c::safe##TYPE##_type_name,  &get_datatype_info_c::safe##TYPE##_type_name,    &get_datatype_info_c::safe##TYPE##_type_name, widen_entry::ok                 },
    __ANY_NUM(__sub)
#undef __sub

    /*******************************************/
    /*******************************************/
    /*** Operations with TIME, DT and TOD... ***/
    /*******************************************/
    /*******************************************/ 
    { &get_datatype_info_c::time_type_name,          &get_datatype_info_c::time_type_name,            &get_datatype_info_c::time_type_name,         widen_entry::ok                 },
    { &get_datatype_info_c::date_type_name,          &get_datatype_info_c::date_type_name,            &get_datatype_info_c::time_type_name,         ANYTIME_OPER_DEPRECATION_STATUS },
    { &get_datatype_info_c::tod_type_name,           &get_datatype_info_c::time_type_name,            &get_datatype_info_c::tod_type_name,          ANYTIME_OPER_DEPRECATION_STATUS },
    { &get_datatype_info_c::tod_type_name,           &get_datatype_info_c::tod_type_name,             &get_datatype_info_c::time_type_name,         ANYTIME_OPER_DEPRECATION_STATUS },
    { &get_datatype_info_c::dt_type_name,            &get_datatype_info_c::time_type_name,            &get_datatype_info_c::dt_type_name,           ANYTIME_OPER_DEPRECATION_STATUS },
    { &get_datatype_info_c::dt_type_name,            &get_datatype_info_c::dt_type_name,              &get_datatype_info_c::time_type_name,         ANYTIME_OPER_DEPRECATION_STATUS },        

    /*******************************/
    /* SAFE version on the left... */
    /*******************************/
    { &get_datatype_info_c::safetime_type_name,      &get_datatype_info_c::time_type_name,            &get_datatype_info_c::time_type_name,         widen_entry::ok                 },
    { &get_datatype_info_c::safedate_type_name,      &get_datatype_info_c::date_type_name,            &get_datatype_info_c::time_type_name,         ANYTIME_OPER_DEPRECATION_STATUS },
    { &get_datatype_info_c::safetod_type_name,       &get_datatype_info_c::time_type_name,            &get_datatype_info_c::tod_type_name,          ANYTIME_OPER_DEPRECATION_STATUS },
    { &get_datatype_info_c::safetod_type_name,       &get_datatype_info_c::tod_type_name,             &get_datatype_info_c::time_type_name,         ANYTIME_OPER_DEPRECATION_STATUS },
    { &get_datatype_info_c::safedt_type_name,        &get_datatype_info_c::time_type_name,            &get_datatype_info_c::dt_type_name,           ANYTIME_OPER_DEPRECATION_STATUS },
    { &get_datatype_info_c::safedt_type_name,        &get_datatype_info_c::dt_type_name,              &get_datatype_info_c::time_type_name,         ANYTIME_OPER_DEPRECATION_STATUS },        

    /********************************/
    /* SAFE version on the right... */
    /********************************/
    { &get_datatype_info_c::time_type_name,          &get_datatype_info_c::safetime_type_name,        &get_datatype_info_c::time_type_name,         widen_entry::ok                 },
    { &get_datatype_info_c::date_type_name,          &get_datatype_info_c::safedate_type_name,        &get_datatype_info_c::time_type_name,         ANYTIME_OPER_DEPRECATION_STATUS },
    { &get_datatype_info_c::tod_type_name,           &get_datatype_info_c::safetime_type_name,        &get_datatype_info_c::tod_type_name,          ANYTIME_OPER_DEPRECATION_STATUS },
    { &get_datatype_info_c::tod_type_name,           &get_datatype_info_c::safetod_type_name,         &get_datatype_info_c::time_type_name,         ANYTIME_OPER_DEPRECATION_STATUS },
    { &get_datatype_info_c::dt_type_name,            &get_datatype_info_c::safetime_type_name,        &get_datatype_info_c::dt_type_name,           ANYTIME_OPER_DEPRECATION_STATUS },
    { &get_datatype_info_c::dt_type_name,            &get_datatype_info_c::safedt_type_name,          &get_datatype_info_c::time_type_name,         ANYTIME_OPER_DEPRECATION_STATUS },        

    /*************************************/
    /* SAFE version on left and right... */
    /*************************************/
    { &get_datatype_info_c::safetime_type_name,      &get_datatype_info_c::safetime_type_name,        &get_datatype_info_c::safetime_type_name,     widen_entry::ok                 },
    { &get_datatype_info_c::safedate_type_name,      &get_datatype_info_c::safedate_type_name,        &get_datatype_info_c::safetime_type_name,     ANYTIME_OPER_DEPRECATION_STATUS },
    { &get_datatype_info_c::safetod_type_name,       &get_datatype_info_c::safetime_type_name,        &get_datatype_info_c::safetod_type_name,      ANYTIME_OPER_DEPRECATION_STATUS },
    { &get_datatype_info_c::safetod_type_name,       &get_datatype_info_c::safetod_type_name,         &get_datatype_info_c::safetime_type_name,     ANYTIME_OPER_DEPRECATION_STATUS },
    { &get_datatype_info_c::safedt_type_name,        &get_datatype_info_c::safetime_type_name,        &get_datatype_info_c::safedt_type_name,       ANYTIME_OPER_DEPRECATION_STATUS },
    { &get_datatype_info_c::safedt_type_name,        &get_datatype_info_c::safedt_type_name,          &get_datatype_info_c::safetime_type_name,     ANYTIME_OPER_DEPRECATION_STATUS },        

    { NULL, NULL, NULL, widen_entry::ok },
};







const struct widen_entry widen_MUL_table[] = {
#define __mul(TYPE)       \
    { &get_datatype_info_c::TYPE##_type_name,        &get_datatype_info_c::TYPE##_type_name,          &get_datatype_info_c::TYPE##_type_name,       widen_entry::ok                 }, \
    { &get_datatype_info_c::safe##TYPE##_type_name,  &get_datatype_info_c::TYPE##_type_name,          &get_datatype_info_c::TYPE##_type_name,       widen_entry::ok                 }, \
    { &get_datatype_info_c::TYPE##_type_name,        &get_datatype_info_c::safe##TYPE##_type_name,    &get_datatype_info_c::TYPE##_type_name,       widen_entry::ok                 }, \
    { &get_datatype_info_c::safe##TYPE##_type_name,  &get_datatype_info_c::safe##TYPE##_type_name,    &get_datatype_info_c::safe##TYPE##_type_name, widen_entry::ok                 },
    __ANY_NUM(__mul)
#undef __mul

    /*******************************************/
    /*******************************************/
    /*** Operations with TIME, DT and TOD... ***/
    /*******************************************/
    /*******************************************/ 
#define __multime(TYPE)       \
    { &get_datatype_info_c::time_type_name,          &get_datatype_info_c::TYPE##_type_name,          &get_datatype_info_c::time_type_name,         ANYTIME_OPER_DEPRECATION_STATUS }, \
    { &get_datatype_info_c::safetime_type_name,      &get_datatype_info_c::TYPE##_type_name,          &get_datatype_info_c::time_type_name,         ANYTIME_OPER_DEPRECATION_STATUS }, \
    { &get_datatype_info_c::time_type_name,          &get_datatype_info_c::safe##TYPE##_type_name,    &get_datatype_info_c::time_type_name,         ANYTIME_OPER_DEPRECATION_STATUS }, \
    { &get_datatype_info_c::safetime_type_name,      &get_datatype_info_c::safe##TYPE##_type_name,    &get_datatype_info_c::safetime_type_name,     ANYTIME_OPER_DEPRECATION_STATUS }, \
    /* NOTE: the standard des not explicitly support the following semantics. However, since 'multiplication' is supposed to be commutative, we add it anyway... */                 \
    { &get_datatype_info_c::TYPE##_type_name,        &get_datatype_info_c::time_type_name,            &get_datatype_info_c::time_type_name,         ANYTIME_OPER_DEPRECATION_STATUS }, \
    { &get_datatype_info_c::safe##TYPE##_type_name,  &get_datatype_info_c::time_type_name,            &get_datatype_info_c::time_type_name,         ANYTIME_OPER_DEPRECATION_STATUS }, \
    { &get_datatype_info_c::TYPE##_type_name,        &get_datatype_info_c::safetime_type_name,        &get_datatype_info_c::time_type_name,         ANYTIME_OPER_DEPRECATION_STATUS }, \
    { &get_datatype_info_c::safe##TYPE##_type_name,  &get_datatype_info_c::safetime_type_name,        &get_datatype_info_c::safetime_type_name,     ANYTIME_OPER_DEPRECATION_STATUS },
    __ANY_NUM(__multime)
#undef __multime

    { NULL, NULL, NULL, widen_entry::ok },
};





const struct widen_entry widen_DIV_table[] = {
#define __div(TYPE)       \
    { &get_datatype_info_c::TYPE##_type_name,        &get_datatype_info_c::TYPE##_type_name,          &get_datatype_info_c::TYPE##_type_name,       widen_entry::ok                 }, \
    { &get_datatype_info_c::safe##TYPE##_type_name,  &get_datatype_info_c::TYPE##_type_name,          &get_datatype_info_c::TYPE##_type_name,       widen_entry::ok                 }, \
    { &get_datatype_info_c::TYPE##_type_name,        &get_datatype_info_c::safe##TYPE##_type_name,    &get_datatype_info_c::TYPE##_type_name,       widen_entry::ok                 }, \
    { &get_datatype_info_c::safe##TYPE##_type_name,  &get_datatype_info_c::safe##TYPE##_type_name,    &get_datatype_info_c::safe##TYPE##_type_name, widen_entry::ok                 },
    __ANY_NUM(__div)
#undef __div

    /*******************************************/
    /*******************************************/
    /*** Operations with TIME, DT and TOD... ***/
    /*******************************************/
    /*******************************************/ 
#define __divtime(TYPE)       \
    { &get_datatype_info_c::time_type_name,          &get_datatype_info_c::TYPE##_type_name,          &get_datatype_info_c::time_type_name,         ANYTIME_OPER_DEPRECATION_STATUS }, \
    { &get_datatype_info_c::safetime_type_name,      &get_datatype_info_c::TYPE##_type_name,          &get_datatype_info_c::time_type_name,         ANYTIME_OPER_DEPRECATION_STATUS }, \
    { &get_datatype_info_c::time_type_name,          &get_datatype_info_c::safe##TYPE##_type_name,    &get_datatype_info_c::time_type_name,         ANYTIME_OPER_DEPRECATION_STATUS }, \
    { &get_datatype_info_c::safetime_type_name,      &get_datatype_info_c::safe##TYPE##_type_name,    &get_datatype_info_c::safetime_type_name,     ANYTIME_OPER_DEPRECATION_STATUS },
    __ANY_NUM(__divtime)
#undef __divtime

    { NULL, NULL, NULL, widen_entry::ok },
 };

 


const struct widen_entry widen_MOD_table[] = {
#define __mod(TYPE)       \
    { &get_datatype_info_c::TYPE##_type_name,        &get_datatype_info_c::TYPE##_type_name,          &get_datatype_info_c::TYPE##_type_name,       widen_entry::ok                 }, \
    { &get_datatype_info_c::safe##TYPE##_type_name,  &get_datatype_info_c::TYPE##_type_name,          &get_datatype_info_c::TYPE##_type_name,       widen_entry::ok                 }, \
    { &get_datatype_info_c::TYPE##_type_name,        &get_datatype_info_c::safe##TYPE##_type_name,    &get_datatype_info_c::TYPE##_type_name,       widen_entry::ok                 }, \
    { &get_datatype_info_c::safe##TYPE##_type_name,  &get_datatype_info_c::safe##TYPE##_type_name,    &get_datatype_info_c::safe##TYPE##_type_name, widen_entry::ok                 },
    __ANY_NUM(__mod)
#undef __mod

    { NULL, NULL, NULL, widen_entry::ok },
};
 
 


const struct widen_entry widen_EXPT_table[] = {
#define __expt(IN2TYPE, IN1TYPE)       \
    { &get_datatype_info_c::IN1TYPE##_type_name,        &get_datatype_info_c::IN2TYPE##_type_name,          &get_datatype_info_c::IN1TYPE##_type_name,       widen_entry::ok        }, \
    { &get_datatype_info_c::safe##IN1TYPE##_type_name,  &get_datatype_info_c::IN2TYPE##_type_name,          &get_datatype_info_c::IN1TYPE##_type_name,       widen_entry::ok        }, \
    { &get_datatype_info_c::IN1TYPE##_type_name,        &get_datatype_info_c::safe##IN2TYPE##_type_name,    &get_datatype_info_c::IN1TYPE##_type_name,       widen_entry::ok        }, \
    { &get_datatype_info_c::safe##IN1TYPE##_type_name,  &get_datatype_info_c::safe##IN2TYPE##_type_name,    &get_datatype_info_c::safe##IN1TYPE##_type_name, widen_entry::ok        },
#define __IN2_anynum_(IN1_TYPENAME)   __ANY_NUM_1(__expt,IN1_TYPENAME)
    __ANY_REAL(__IN2_anynum_)
#undef __expt
#undef __IN2_anynum_
    { NULL, NULL, NULL, widen_entry::ok },
};



/**************************************************************/
/**************************************************************/
/**************************************************************/
/*******                                                *******/
/*******  TABLE 26: Standard bitwise Boolean functions  *******/
/*******                                                *******/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/* table used by AND and ANDN operators, and and_expression */
const struct widen_entry widen_AND_table[] = {
#define __and(TYPE)       \
    { &get_datatype_info_c::TYPE##_type_name,        &get_datatype_info_c::TYPE##_type_name,          &get_datatype_info_c::TYPE##_type_name,       widen_entry::ok                 }, \
    { &get_datatype_info_c::safe##TYPE##_type_name,  &get_datatype_info_c::TYPE##_type_name,          &get_datatype_info_c::TYPE##_type_name,       widen_entry::ok                 }, \
    { &get_datatype_info_c::TYPE##_type_name,        &get_datatype_info_c::safe##TYPE##_type_name,    &get_datatype_info_c::TYPE##_type_name,       widen_entry::ok                 }, \
    { &get_datatype_info_c::safe##TYPE##_type_name,  &get_datatype_info_c::safe##TYPE##_type_name,    &get_datatype_info_c::safe##TYPE##_type_name, widen_entry::ok                 },
    __ANY_BIT(__and)
#undef __and

    { NULL, NULL, NULL, widen_entry::ok },
};

/* table used by OR and ORN operators, and or_expression */
const struct widen_entry widen_OR_table[] = {
#define __or(TYPE)       \
    { &get_datatype_info_c::TYPE##_type_name,        &get_datatype_info_c::TYPE##_type_name,          &get_datatype_info_c::TYPE##_type_name,       widen_entry::ok                 }, \
    { &get_datatype_info_c::safe##TYPE##_type_name,  &get_datatype_info_c::TYPE##_type_name,          &get_datatype_info_c::TYPE##_type_name,       widen_entry::ok                 }, \
    { &get_datatype_info_c::TYPE##_type_name,        &get_datatype_info_c::safe##TYPE##_type_name,    &get_datatype_info_c::TYPE##_type_name,       widen_entry::ok                 }, \
    { &get_datatype_info_c::safe##TYPE##_type_name,  &get_datatype_info_c::safe##TYPE##_type_name,    &get_datatype_info_c::safe##TYPE##_type_name, widen_entry::ok                 },
    __ANY_BIT(__or)
#undef __or

    { NULL, NULL, NULL, widen_entry::ok },
};


/* table used by XOR and XORN operators, and xor_expression */
const struct widen_entry widen_XOR_table[] = {
#define __xor(TYPE)       \
    { &get_datatype_info_c::TYPE##_type_name,        &get_datatype_info_c::TYPE##_type_name,          &get_datatype_info_c::TYPE##_type_name,       widen_entry::ok                 }, \
    { &get_datatype_info_c::safe##TYPE##_type_name,  &get_datatype_info_c::TYPE##_type_name,          &get_datatype_info_c::TYPE##_type_name,       widen_entry::ok                 }, \
    { &get_datatype_info_c::TYPE##_type_name,        &get_datatype_info_c::safe##TYPE##_type_name,    &get_datatype_info_c::TYPE##_type_name,       widen_entry::ok                 }, \
    { &get_datatype_info_c::safe##TYPE##_type_name,  &get_datatype_info_c::safe##TYPE##_type_name,    &get_datatype_info_c::safe##TYPE##_type_name, widen_entry::ok                 },
    __ANY_BIT(__xor)
#undef __xor

    { NULL, NULL, NULL, widen_entry::ok },
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
/*******                                                *******/
/*******  TABLE 28: Standard comparison functions       *******/
/*******                                                *******/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/* table used by GT, GE, EQ, LE, LT, and NE  operators, and equivalent ST expressions. */
const struct widen_entry widen_CMP_table[] = {
#define __cmp(TYPE)       \
    { &get_datatype_info_c::TYPE##_type_name,        &get_datatype_info_c::TYPE##_type_name,          &get_datatype_info_c::bool_type_name,         widen_entry::ok                 }, \
    { &get_datatype_info_c::safe##TYPE##_type_name,  &get_datatype_info_c::TYPE##_type_name,          &get_datatype_info_c::bool_type_name,         widen_entry::ok                 }, \
    { &get_datatype_info_c::TYPE##_type_name,        &get_datatype_info_c::safe##TYPE##_type_name,    &get_datatype_info_c::bool_type_name,         widen_entry::ok                 }, \
    { &get_datatype_info_c::safe##TYPE##_type_name,  &get_datatype_info_c::safe##TYPE##_type_name,    &get_datatype_info_c::safebool_type_name,     widen_entry::ok                 },
    __ANY_ELEMENTARY(__cmp)
#undef __cmp

    { NULL, NULL, NULL, widen_entry::ok },
};


/* Search for a datatype inside a candidate_datatypes list.
 * Returns: position of datatype in the list, or -1 if not found.
 */
int search_in_candidate_datatype_list(symbol_c *datatype, const std::vector <symbol_c *> &candidate_datatypes) {
	if (NULL == datatype) 
		return -1;

	for(unsigned int i = 0; i < candidate_datatypes.size(); i++)
		if (get_datatype_info_c::is_type_equal(datatype, candidate_datatypes[i]))
			return i;
	/* Not found ! */
	return -1;
}

/* Remove a datatype inside a candidate_datatypes list.
 * Returns: If successful it returns true, false otherwise.
 */
bool remove_from_candidate_datatype_list(symbol_c *datatype, std::vector <symbol_c *> &candidate_datatypes) {
	int pos = search_in_candidate_datatype_list(datatype, candidate_datatypes);
	if (pos < 0)
		return false;
	
	candidate_datatypes.erase(candidate_datatypes.begin() + pos);
	return true;
}



/* Intersect two candidate_datatype_lists.
 * Remove from list1 (origin, dest.) all elements that are not found in list2 (with).
 * In essence, list1 will contain the result of the intersection of list1 with list2.
 * In other words, modify list1 so it only contains the elelements that are simultaneously in list1 and list2!
 */
void intersect_candidate_datatype_list(symbol_c *list1 /*origin, dest.*/, symbol_c *list2 /*with*/) {
	if ((NULL == list1) || (NULL == list2))
		/* In principle, we should never call it with NULL values. Best to abort the compiler just in case! */
		return;

	for(std::vector<symbol_c *>::iterator i = list1->candidate_datatypes.begin(); i < list1->candidate_datatypes.end(); ) {
		/* Note that we do _not_ increment i in the for() loop!
		 * When we erase an element from position i, a new element will take it's place, that must also be tested! 
		 */
		if (search_in_candidate_datatype_list(*i, list2->candidate_datatypes) < 0)
			/* remove this element! This will change the value of candidate_datatypes.size() */
			list1->candidate_datatypes.erase(i);
		else i++;
	}
}




/* intersect the candidate_datatype lists of all prev_il_intructions, and set the local candidate_datatype list to the result! */
void intersect_prev_candidate_datatype_lists(il_instruction_c *symbol) {
	if (symbol->prev_il_instruction.empty())
		return;
	
	symbol->candidate_datatypes = symbol->prev_il_instruction[0]->candidate_datatypes;
	for (unsigned int i = 1; i < symbol->prev_il_instruction.size(); i++) {
		intersect_candidate_datatype_list(symbol /*origin, dest.*/, symbol->prev_il_instruction[i] /*with*/);
	}  
}







