
#include "StdIOWorkaround.h"

#if (_MSC_VER >= 1900)

FILE _iob[] = { *stdin, *stdout, *stderr };

extern "C" FILE * __cdecl __iob_func(void)
{
	return _iob;
}

#endif
