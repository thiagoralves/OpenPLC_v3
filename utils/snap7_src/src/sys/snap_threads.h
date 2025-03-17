/*=============================================================================|
|  PROJECT SNAP7                                                         1.3.0 |
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
#ifndef snap_threads_h
#define snap_threads_h
//---------------------------------------------------------------------------
#include "snap_platform.h"

#ifdef OS_WINDOWS
# include "win_threads.h"
#endif
#if defined(POSIX) && (!defined(OS_SOLARIS_NATIVE_THREADS))
# include "unix_threads.h"
#endif
#ifdef OS_SOLARIS_NATIVE_THREADS
# include "sol_threads.h"
#endif
#if defined(OS_OSX)
# include "unix_threads.h"
#endif

//---------------------------------------------------------------------------
#endif // snap_threads_h
