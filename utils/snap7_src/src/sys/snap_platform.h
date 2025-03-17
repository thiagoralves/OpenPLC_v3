/*=============================================================================|
|  PROJECT SNAP7                                                         1.4.1 |
|==============================================================================|
|  Copyright (C) 2013, 2015 Davide Nardella                                    |
|  All rights reserved.                                                        |
|==============================================================================|
|  SNAP7 is free software: you can redistribute it and/or modify               |
|  it under the terms of the Lesser GNU General Public License as published by |
|  the Free Software Foundation, either version 3 of the License, or           |
|  (at your option) any later version.                                         |
|                                                                              |
|  It means that you can distribute your commercial software linked with       |
|  SNAP7 without the requirement to distribute the source code of your         |
|  application and without the requirement that your application be itself     |
|  distributed under LGPL.                                                     |
|                                                                              |
|  SNAP7 is distributed in the hope that it will be useful,                    |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of              |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               |
|  Lesser GNU General Public License for more details.                         |
|                                                                              |
|  You should have received a copy of the GNU General Public License and a     |
|  copy of Lesser GNU General Public License along with Snap7.                 |
|  If not, see  http://www.gnu.org/licenses/                                   |
|=============================================================================*/
#ifndef snap_platform_h
#define snap_platform_h
//---------------------------------------------------------------------------
#if defined (_WIN32)|| defined(_WIN64)|| defined(__WIN32__) || defined(__WINDOWS__)
# define OS_WINDOWS
#endif

// Visual Studio needs this to use the correct time_t size
#if defined (_WIN32) && !defined(_WIN64) && !defined(_EMBEDDING_VS2013UP)
  # define _USE_32BIT_TIME_T 
#endif

// Linux, BSD and Solaris define "unix", OSX doesn't, even though it derives from BSD
#if defined(unix) || defined(__unix__) || defined(__unix)
# define PLATFORM_UNIX
#endif

#if BSD>=0
# define OS_BSD
#endif

#if __APPLE__
# define OS_OSX
#endif

#if defined(__SVR4) || defined(__svr4__)
# define OS_SOLARIS
// Thanks to Rolf Stalder now it's possible to use pthreads also for Solaris
// In any case the Solaris native threads model is still present and can be
// used uncommenting the #define line below.
# undef OS_SOLARIS_NATIVE_THREADS
// # define OS_SOLARIS_NATIVE_THREADS
#endif

#if defined(PLATFORM_UNIX)
# include <unistd.h>
# include <sys/param.h>
# if defined(_POSIX_VERSION)
#   define POSIX
# endif
#endif

#ifdef OS_OSX
# include <unistd.h>
#endif

#if (!defined (OS_WINDOWS)) && (!defined(PLATFORM_UNIX)) && (!defined(OS_BSD)) && (!defined(OS_OSX))
# error platform still unsupported (please add it yourself and report ;-)
#endif

// Visual C++ not C99 compliant (VS2008--)
#ifdef _MSC_VER
# if _MSC_VER >= 1600
#  include <stdint.h>  // VS2010++ have it 
# else
   typedef signed __int8     int8_t;
   typedef signed __int16    int16_t;
   typedef signed __int32    int32_t;
   typedef signed __int64    int64_t;
   typedef unsigned __int8   uint8_t;
   typedef unsigned __int16  uint16_t;
   typedef unsigned __int32  uint32_t;
   typedef unsigned __int64  uint64_t;
   #ifdef _WIN64
     typedef unsigned __int64  uintptr_t;
   #else
     typedef unsigned __int32  uintptr_t;
   #endif
# endif
#else
# include <stdint.h>
#endif

#include <time.h>
#include <cstring>
#include <stdlib.h>

#ifdef OS_WINDOWS
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include <winsock2.h>
# include <mmsystem.h>
#endif

#ifdef OS_SOLARIS
# include <sys/filio.h>
# include <cstdlib>
# include <string.h>
#endif

#if defined(PLATFORM_UNIX) || defined(OS_OSX)
# include <errno.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netinet/tcp.h>
# include <netinet/in.h>
# include <sys/ioctl.h>
#endif

#ifdef OS_WINDOWS
# define EXPORTSPEC extern "C" __declspec ( dllexport )
# define S7API __stdcall
#else
# define EXPORTSPEC extern "C"
# define S7API
#endif

// Exact length types regardless of platform/processor
// We absolute need of them, all structs have an exact size that
// must be the same across the processor used 32/64 bit

// *Use them* if you change/expand the code and avoid long, u_long and so on...

typedef uint8_t    byte;
typedef uint16_t   word;
typedef uint32_t   longword;
typedef byte       *pbyte;
typedef word       *pword;
typedef uintptr_t  snap_obj; // multi platform/processor object reference

#ifndef OS_WINDOWS
# define INFINITE  0XFFFFFFFF
#endif


#endif // snap_platform_h
