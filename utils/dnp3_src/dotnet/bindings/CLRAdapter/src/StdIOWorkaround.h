

#if (_MSC_VER >= 1900)

/*
http://stackoverflow.com/questions/30412951/unresolved-external-symbol-imp-fprintf-and-imp-iob-func-sdl2
*/

#include <cstdio>

extern "C" FILE * __cdecl __iob_func(void);

#endif
