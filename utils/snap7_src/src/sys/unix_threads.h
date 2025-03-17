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
| Posix Threads support (Linux, FreeBSD)                                       |
|                                                                              |
|=============================================================================*/
#ifndef unix_threads_h
#define unix_threads_h
//---------------------------------------------------------------------------
#include "snap_platform.h"
#include "snap_sysutils.h"
#include <semaphore.h>
#include <pthread.h>
//---------------------------------------------------------------------------

class TSnapCriticalSection 
{
private:
    pthread_mutex_t mx;
//    int result;
public:

    TSnapCriticalSection() 
    {
        /*
		   
        This would be the best code, but very often it causes a segmentation fault in many
        unix systems (the problem seems to be pthread_mutexattr_destroy()).
        So, to avoid problems in future kernel/libc release, we use the "safe" default.
		   
        pthread_mutexattr_t mxAttr;
        pthread_mutexattr_settype(&mxAttr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&mx, &mxAttr);
        pthread_mutexattr_destroy(&mxAttr);

         */
        pthread_mutex_init(&mx, 0);
    };

    ~TSnapCriticalSection() 
    {
        pthread_mutex_destroy(&mx);
    };

    void Enter() 
    {
        pthread_mutex_lock(&mx);
    };

    void Leave() 
    {
        pthread_mutex_unlock(&mx);
    };

    bool TryEnter() 
    {
        return pthread_mutex_trylock(&mx) == 0;
    };
};
typedef TSnapCriticalSection *PSnapCriticalSection;

//---------------------------------------------------------------------------
const longword WAIT_OBJECT_0 = 0x00000000L;
const longword WAIT_ABANDONED = 0x00000080L;
const longword WAIT_TIMEOUT = 0x00000102L;
const longword WAIT_FAILED = 0xFFFFFFFFL;

class TSnapEvent 
{
private:
    pthread_cond_t CVariable;
    pthread_mutex_t Mutex;
    bool AutoReset;
    bool State;
public:

    TSnapEvent(bool ManualReset) 
    {
        AutoReset = !ManualReset;
        if (pthread_cond_init(&CVariable, 0) == 0)
        	pthread_mutex_init(&Mutex, 0);
        State = false;
    }

    ~TSnapEvent() 
    {
        pthread_cond_destroy(&CVariable);
        pthread_mutex_destroy(&Mutex);
    };

    void Set() 
    {
        pthread_mutex_lock(&Mutex);
        State = true;
        if (AutoReset)
            pthread_cond_signal(&CVariable);
        else
            pthread_cond_broadcast(&CVariable);
        pthread_mutex_unlock(&Mutex);
    };

    void Reset() 
    {
        pthread_mutex_lock(&Mutex);
        State = false;
        pthread_mutex_unlock(&Mutex);
    }

    longword WaitForever() 
    {
        pthread_mutex_lock(&Mutex);
        while (!State) // <-- to avoid spurious wakeups
            pthread_cond_wait(&CVariable, &Mutex);
        if (AutoReset)
            State = false;
        pthread_mutex_unlock(&Mutex);
        return WAIT_OBJECT_0;
    };

    longword WaitFor(int64_t Timeout) 
    {
        longword Result = WAIT_OBJECT_0;
        if (Timeout == 0)
            Timeout = 1; // 0 is not allowed

        if (Timeout > 0) 
        {
            pthread_mutex_lock(&Mutex);
            if (!State)
            {
                timespec ts;
                timeval tv;
                gettimeofday(&tv, NULL);
                uint64_t nsecs = ((uint64_t) tv.tv_sec) * 1000000000 +
                        Timeout * 1000000 +
                        ((uint64_t) tv.tv_usec) * 1000;
                ts.tv_sec = nsecs / 1000000000;
                ts.tv_nsec = (nsecs - ((uint64_t) ts.tv_sec) * 1000000000);
                do {
                    Result = pthread_cond_timedwait(&CVariable, &Mutex, &ts);
                    if (Result == ETIMEDOUT)
                        Result = WAIT_TIMEOUT;
                } while (Result == 0 && !State);
            } 
            else
                if (AutoReset) // take the ownership
                    State = false;
            pthread_mutex_unlock(&Mutex);
            return Result;
        }
        else // Timeout<0
            return WaitForever();
    };
};
typedef TSnapEvent *PSnapEvent;
//---------------------------------------------------------------------------
class TSnapThread 
{
private:
    pthread_t th;
    bool FCreateSuspended;
    void ThreadCreate();

    void ThreadJoin() 
    {
        pthread_join(th, 0);
    };

    void ThreadKill() 
    {
        pthread_cancel(th);
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
#endif // unix_threads_h
