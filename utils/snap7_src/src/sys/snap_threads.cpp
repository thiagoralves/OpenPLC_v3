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

#include "snap_threads.h"
//---------------------------------------------------------------------------
#ifdef OS_WINDOWS
DWORD WINAPI ThreadProc(LPVOID param)
#else

void* ThreadProc(void* param)
#endif
{
    PSnapThread Thread;
    // Unix but not Solaris
#if (defined(POSIX) || defined(OS_OSX)) && (!defined(OS_SOLARIS_NATIVE_THREADS))
    int last_type, last_state;
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &last_type);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &last_state);
#endif
    Thread = PSnapThread(param);

    if (!Thread->Terminated)
        try
        {
            Thread->Execute();
        } catch (...)
        {
        };
    Thread->Closed = true;
    if (Thread->FreeOnTerminate)
    {
        delete Thread;
    };
#ifdef OS_WINDOWS
    ExitThread(0);
#endif
#if defined(POSIX) && (!defined(OS_SOLARIS_NATIVE_THREADS))
    pthread_exit((void*)0);
#endif
#if defined(OS_OSX)
    pthread_exit((void*)0);
#endif
#ifdef OS_SOLARIS_NATIVE_THREADS
    thr_exit((void*)0);
#endif
    return 0; // never reach, only to avoid compiler warning
}
//---------------------------------------------------------------------------
TSnapThread::TSnapThread() 
{
    Started = false;
	Closed=false;
    Terminated = false;
    FreeOnTerminate = false;
}
//---------------------------------------------------------------------------
TSnapThread::~TSnapThread() 
{
    if (Started && !Closed)
    {
        Terminate();
        Join();
    };
#ifdef OS_WINDOWS
	if (Started)
		CloseHandle(th);
#endif
}
//---------------------------------------------------------------------------
void TSnapThread::ThreadCreate() 
{
#ifdef OS_WINDOWS
    th = CreateThread(0, 0, ThreadProc, this, 0, 0);
#endif
#if defined(POSIX) && (!defined(OS_SOLARIS_NATIVE_THREADS))
    pthread_attr_t a;
    pthread_attr_init(&a);
    pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED);
    pthread_create(&th, &a, &ThreadProc, this);
#endif  
#if defined(OS_OSX)
    pthread_create(&th, 0, &ThreadProc, this);
#endif
#ifdef OS_SOLARIS_NATIVE_THREADS
    thr_create(0, // default stack base
               0, // default stack size
               &ThreadProc, // Thread routine
               this, //  argument
               0,
               &th);
#endif
}
//---------------------------------------------------------------------------
void TSnapThread::Start() 
{
    if (!Started)
    {
        ThreadCreate();
        Started = true;
    }
}
//---------------------------------------------------------------------------
void TSnapThread::Terminate() 
{
    Terminated = true;
}
//---------------------------------------------------------------------------
void TSnapThread::Kill() 
{
    if (Started && !Closed)
    {
        ThreadKill();
        Closed = true;
    }
}
//---------------------------------------------------------------------------
void TSnapThread::Join() 
{
    if (Started && !Closed)
    {
        ThreadJoin();
        Closed = true;
    }
}
//---------------------------------------------------------------------------
longword TSnapThread::WaitFor(uint64_t Timeout) 
{
    if (Started)
    {
        if (!Closed)
            return ThreadWait(Timeout);
        else
            return WAIT_OBJECT_0;
    }
    else
        return WAIT_OBJECT_0;
}
//---------------------------------------------------------------------------

