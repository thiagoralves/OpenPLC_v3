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
| Solaris 11 Threads support                                                   |
|                                                                              |
|=============================================================================*/
#ifndef sol_threads_h
#define sol_threads_h
//---------------------------------------------------------------------------
#include "snap_platform.h"
#include "snap_sysutils.h"
#include <thread.h>
#include <synch.h>
//---------------------------------------------------------------------------

class TSnapCriticalSection {
private:
    mutex_t mx;
    int result;
public:

    TSnapCriticalSection() {
        mutex_init(&mx, USYNC_THREAD, 0);
    };

    ~TSnapCriticalSection() {
        mutex_destroy(&mx);
    };

    void Enter() {
        mutex_lock(&mx);
    };

    void Leave() {
        mutex_unlock(&mx);
    };

    bool TryEnter() {
        return mutex_trylock(&mx) == 0;
    };
};
typedef TSnapCriticalSection *PSnapCriticalSection;

//---------------------------------------------------------------------------
const longword WAIT_OBJECT_0 = 0x00000000L;
const longword WAIT_ABANDONED = 0x00000080L;
const longword WAIT_TIMEOUT = 0x00000102L;
const longword WAIT_FAILED = 0xFFFFFFFFL;

class TSnapEvent {
private:
    cond_t CVariable;
    mutex_t Mutex;
    bool AutoReset;
    bool State;
public:

    TSnapEvent(bool ManualReset) 
    {
        AutoReset = !ManualReset;
        cond_init(&CVariable, USYNC_THREAD, 0) == 0;
        mutex_init(&Mutex, USYNC_THREAD, 0);
        State = false;
    }

    ~TSnapEvent() 
    {
        cond_destroy(&CVariable);
        mutex_destroy(&Mutex);
    };

    void Set() 
    {
        mutex_lock(&Mutex);
        State = true;
        if (AutoReset)
            cond_signal(&CVariable);
        else
            cond_broadcast(&CVariable);
        mutex_unlock(&Mutex);
    };

    void Reset() 
    {
        mutex_lock(&Mutex);
        State = false;
        mutex_unlock(&Mutex);
    }

    longword WaitForever() 
    {
        mutex_lock(&Mutex);
        while (!State) // <-- to avoid spurious wakeups
            cond_wait(&CVariable, &Mutex);
        if (AutoReset)
            State = false;
        mutex_unlock(&Mutex);
        return WAIT_OBJECT_0;
    };

    longword WaitFor(int64_t Timeout) 
    {
        longword Result = WAIT_OBJECT_0;
        if (Timeout == 0)
            Timeout = 1; // 0 is not allowed 

        if (Timeout > 0) {
            mutex_lock(&Mutex);
            if (!State) {
                timespec ts;
                timeval tv;
                gettimeofday(&tv, NULL);
                uint64_t nsecs = ((uint64_t) tv.tv_sec) * 1000000000 +
                        Timeout * 1000000 +
                        ((uint64_t) tv.tv_usec) * 1000;
                ts.tv_sec = nsecs / 1000000000;
                ts.tv_nsec = (nsecs - ((uint64_t) ts.tv_sec) * 1000000000);
                do 
                {
                    Result = cond_timedwait(&CVariable, &Mutex, &ts);
                    if (Result == ETIMEDOUT)
                        Result = WAIT_TIMEOUT;
                } 
                while (Result == 0 && !State);
            } 
            else
                if (AutoReset) // take the ownership
                    State = false;
            mutex_unlock(&Mutex);
            return Result;
        } 
        else // Timeout<0
            return WaitForever();
    };
};
typedef TSnapEvent *PSnapEvent;
//---------------------------------------------------------------------------

class TSnapThread {
private:
    thread_t th;
    bool FCreateSuspended;
    void ThreadCreate();

    void ThreadJoin() 
    {
        thr_join(th, 0, 0);
    };

    void ThreadKill() 
    {
        thr_kill(th, 0);
    };

    longword ThreadWait(uint64_t Timeout) 
    {
        longword Elapsed = SysGetTick();
        while (!Closed && !(DeltaTime(Elapsed) > Timeout))
            SysSleep(100);
        if (Closed)
            return WAIT_OBJECT_0;
        else
            return WAIT_TIMEOUT;
    };
protected:
    bool Started;
public:
    bool Terminated;
    bool Closed;
    bool FreeOnTerminate;
    TSnapThread();
    virtual ~TSnapThread();

    virtual void Execute() {
    };
    void Start();
    void Terminate();
    void Kill();
    void Join();
    longword WaitFor(uint64_t Timeout);
};
typedef TSnapThread *PSnapThread;

//---------------------------------------------------------------------------
#endif // sol_threads_h
