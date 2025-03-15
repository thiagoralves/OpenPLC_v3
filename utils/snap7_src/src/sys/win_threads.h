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
|==============================================================================|
|                                                                              |
| Windows Threads support (Windows, ReactOS)                                   |
|                                                                              |
|=============================================================================*/
#ifndef win_threads_h
#define win_threads_h
//---------------------------------------------------------------------------
#include "snap_platform.h"
#include "snap_sysutils.h"
//---------------------------------------------------------------------------

class TSnapCriticalSection 
{
private:
    CRITICAL_SECTION cs;
public:

    TSnapCriticalSection() 
    {
        InitializeCriticalSection(&cs);
    };

    ~TSnapCriticalSection() 
    {
        DeleteCriticalSection(&cs);
    };

    void Enter() 
    {
        EnterCriticalSection(&cs);
    };

    void Leave() 
    {
        LeaveCriticalSection(&cs);
    };

    bool TryEnter() 
    {
        return (TryEnterCriticalSection(&cs) != 0);
    };
};
typedef TSnapCriticalSection *PSnapCriticalSection;
//---------------------------------------------------------------------------

class TSnapEvent 
{
private:
    HANDLE Event;
public:

    TSnapEvent(bool ManualReset) 
    {
        Event = CreateEvent(0, ManualReset, false, 0);
    };

    ~TSnapEvent() 
    {
        if (Event != 0)
            CloseHandle(Event);
    };

    void Set() 
    {
        if (Event != 0)
            SetEvent(Event);
    };

    void Reset() 
    {
        if (Event != 0)
            ResetEvent(Event);
    };

    longword WaitForever() 
    {
        if (Event != 0)
            return WaitForSingleObject(Event, INFINITE);
        else
            return WAIT_FAILED;
    };

    longword WaitFor(int64_t Timeout) {
        if (Event != 0)
            return WaitForSingleObject(Event, DWORD(Timeout));
        else
            return WAIT_FAILED;
    };
};
typedef TSnapEvent *PSnapEvent;
//---------------------------------------------------------------------------

class TSnapThread {
private:
    HANDLE th;
    bool FCreateSuspended;
    void ThreadCreate();

    void ThreadJoin() 
    {
        WaitForSingleObject(th, INFINITE);
    };

    void ThreadKill() 
    {
        TerminateThread(th, 0);
    };

    longword ThreadWait(uint64_t Timeout) 
    {
        return WaitForSingleObject(th, DWORD(Timeout));
    };
protected:
    bool Started;
public:
    bool Terminated;
    bool Closed;
    bool FreeOnTerminate;
    TSnapThread();
    virtual ~TSnapThread();

    virtual void Execute() 
    {
    };
    void Start();
    void Terminate();
    void Kill();
    void Join();
    longword WaitFor(uint64_t Timeout);
};
typedef TSnapThread *PSnapThread;

//---------------------------------------------------------------------------
#endif // win_threads_h
