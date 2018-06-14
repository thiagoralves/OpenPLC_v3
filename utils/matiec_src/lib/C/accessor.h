#ifndef __ACCESSOR_H
#define __ACCESSOR_H

#define __INITIAL_VALUE(...) __VA_ARGS__

// variable declaration macros
#define __DECLARE_VAR(type, name)\
	__IEC_##type##_t name;
#define __DECLARE_GLOBAL(type, domain, name)\
	__IEC_##type##_t domain##__##name;\
	static __IEC_##type##_t *GLOBAL__##name = &(domain##__##name);\
	void __INIT_GLOBAL_##name(type value) {\
		(*GLOBAL__##name).value = value;\
	}\
	IEC_BYTE __IS_GLOBAL_##name##_FORCED(void) {\
		return (*GLOBAL__##name).flags & __IEC_FORCE_FLAG;\
	}\
	type* __GET_GLOBAL_##name(void) {\
		return &((*GLOBAL__##name).value);\
	}
#define __DECLARE_GLOBAL_FB(type, domain, name)\
	type domain##__##name;\
	static type *GLOBAL__##name = &(domain##__##name);\
	type* __GET_GLOBAL_##name(void) {\
		return &(*GLOBAL__##name);\
	}\
	extern void type##_init__(type* data__, BOOL retain);
#define __DECLARE_GLOBAL_LOCATION(type, location)\
	extern type *location;
#define __DECLARE_GLOBAL_LOCATED(type, resource, name)\
	__IEC_##type##_p resource##__##name;\
	static __IEC_##type##_p *GLOBAL__##name = &(resource##__##name);\
	void __INIT_GLOBAL_##name(type value) {\
		*((*GLOBAL__##name).value) = value;\
	}\
	IEC_BYTE __IS_GLOBAL_##name##_FORCED(void) {\
		return (*GLOBAL__##name).flags & __IEC_FORCE_FLAG;\
	}\
	type* __GET_GLOBAL_##name(void) {\
		return (*GLOBAL__##name).value;\
	}
#define __DECLARE_GLOBAL_PROTOTYPE(type, name)\
    extern type* __GET_GLOBAL_##name(void);
#define __DECLARE_EXTERNAL(type, name)\
	__IEC_##type##_p name;
#define __DECLARE_EXTERNAL_FB(type, name)\
	type* name;
#define __DECLARE_LOCATED(type, name)\
	__IEC_##type##_p name;


// variable initialization macros
#define __INIT_RETAIN(name, retained)\
    name.flags |= retained?__IEC_RETAIN_FLAG:0;
#define __INIT_VAR(name, initial, retained)\
	name.value = initial;\
	__INIT_RETAIN(name, retained)
#define __INIT_GLOBAL(type, name, initial, retained)\
    {\
	    static const type temp = initial;\
	    __INIT_GLOBAL_##name(temp);\
	    __INIT_RETAIN((*GLOBAL__##name), retained)\
    }
#define __INIT_GLOBAL_FB(type, name, retained)\
	type##_init__(&(*GLOBAL__##name), retained);
#define __INIT_GLOBAL_LOCATED(domain, name, location, retained)\
	domain##__##name.value = location;\
	__INIT_RETAIN(domain##__##name, retained)
#define __INIT_EXTERNAL(type, global, name, retained)\
    {\
		name.value = __GET_GLOBAL_##global();\
		__INIT_RETAIN(name, retained)\
    }
#define __INIT_EXTERNAL_FB(type, global, name, retained)\
	name = __GET_GLOBAL_##global();
#define __INIT_LOCATED(type, location, name, retained)\
	{\
		extern type *location;\
		name.value = location;\
		__INIT_RETAIN(name, retained)\
    }
#define __INIT_LOCATED_VALUE(name, initial)\
	*(name.value) = initial;


// variable getting macros
#define __GET_VAR(name, ...)\
	name.value __VA_ARGS__
#define __GET_EXTERNAL(name, ...)\
	((name.flags & __IEC_FORCE_FLAG) ? name.fvalue __VA_ARGS__ : (*(name.value)) __VA_ARGS__)
#define __GET_EXTERNAL_FB(name, ...)\
	__GET_VAR(((*name) __VA_ARGS__))
#define __GET_LOCATED(name, ...)\
	((name.flags & __IEC_FORCE_FLAG) ? name.fvalue __VA_ARGS__ : (*(name.value)) __VA_ARGS__)

#define __GET_VAR_BY_REF(name, ...)\
	((name.flags & __IEC_FORCE_FLAG) ? &(name.fvalue __VA_ARGS__) : &(name.value __VA_ARGS__))
#define __GET_EXTERNAL_BY_REF(name, ...)\
	((name.flags & __IEC_FORCE_FLAG) ? &(name.fvalue __VA_ARGS__) : &((*(name.value)) __VA_ARGS__))
#define __GET_EXTERNAL_FB_BY_REF(name, ...)\
	__GET_EXTERNAL_BY_REF(((*name) __VA_ARGS__))
#define __GET_LOCATED_BY_REF(name, ...)\
	((name.flags & __IEC_FORCE_FLAG) ? &(name.fvalue __VA_ARGS__) : &((*(name.value)) __VA_ARGS__))

#define __GET_VAR_REF(name, ...)\
	(&(name.value __VA_ARGS__))
#define __GET_EXTERNAL_REF(name, ...)\
	(&((*(name.value)) __VA_ARGS__))
#define __GET_EXTERNAL_FB_REF(name, ...)\
	(&(__GET_VAR(((*name) __VA_ARGS__))))
#define __GET_LOCATED_REF(name, ...)\
	(&((*(name.value)) __VA_ARGS__))

#define __GET_VAR_DREF(name, ...)\
	(*(name.value __VA_ARGS__))
#define __GET_EXTERNAL_DREF(name, ...)\
	(*((*(name.value)) __VA_ARGS__))
#define __GET_EXTERNAL_FB_DREF(name, ...)\
	(*(__GET_VAR(((*name) __VA_ARGS__))))
#define __GET_LOCATED_DREF(name, ...)\
	(*((*(name.value)) __VA_ARGS__))


// variable setting macros
#define __SET_VAR(prefix, name, suffix, new_value)\
	if (!(prefix name.flags & __IEC_FORCE_FLAG)) prefix name.value suffix = new_value
#define __SET_EXTERNAL(prefix, name, suffix, new_value)\
	{extern IEC_BYTE __IS_GLOBAL_##name##_FORCED();\
    if (!(prefix name.flags & __IEC_FORCE_FLAG || __IS_GLOBAL_##name##_FORCED()))\
		(*(prefix name.value)) suffix = new_value;}
#define __SET_EXTERNAL_FB(prefix, name, suffix, new_value)\
	__SET_VAR((*(prefix name)), suffix, new_value)
#define __SET_LOCATED(prefix, name, suffix, new_value)\
	if (!(prefix name.flags & __IEC_FORCE_FLAG)) *(prefix name.value) suffix = new_value

#endif //__ACCESSOR_H
