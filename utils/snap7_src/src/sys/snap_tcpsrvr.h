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
#ifndef snap_tcpsrvr_h
#define snap_tcpsrvr_h
//---------------------------------------------------------------------------
#include "snap_msgsock.h"
#include "snap_threads.h"
//---------------------------------------------------------------------------

#define MaxWorkers 1024
#define MaxEvents  1500

const int SrvStopped = 0;
const int SrvRunning = 1;
const int SrvError   = 2;

const longword evcServerStarted       = 0x00000001;
const longword evcServerStopped       = 0x00000002;
const longword evcListenerCannotStart = 0x00000004;
const longword evcClientAdded         = 0x00000008;
const longword evcClientRejected      = 0x00000010;
const longword evcClientNoRoom        = 0x00000020;
const longword evcClientException     = 0x00000040;
const longword evcClientDisconnected  = 0x00000080;
const longword evcClientTerminated    = 0x00000100;
const longword evcClientsDropped      = 0x00000200;
const longword evcReserved_00000400   = 0x00000400;
const longword evcReserved_00000800   = 0x00000800;
const longword evcReserved_00001000   = 0x00001000;
const longword evcReserved_00002000   = 0x00002000;
const longword evcReserved_00004000   = 0x00004000;
const longword evcReserved_00008000   = 0x00008000;

// Server Interface errors
const longword errSrvBase             = 0x0000FFFF;
const longword errSrvMask             = 0xFFFF0000;
const longword errSrvCannotStart      = 0x00100000;

const longword ThTimeout   = 2000; // Thread timeout
const longword WkTimeout   = 3000; // Workers termination timeout

#pragma pack(1)

typedef struct{
    time_t EvtTime;    // Timestamp
    int EvtSender;     // Sender
    longword EvtCode;  // Event code
    word EvtRetCode;   // Event result
    word EvtParam1;    // Param 1 (if available)
    word EvtParam2;    // Param 2 (if available)
    word EvtParam3;    // Param 3 (if available)
    word EvtParam4;    // Param 4 (if available)
}TSrvEvent, *PSrvEvent;

extern "C" 
{
typedef void (S7API *pfn_SrvCallBack)(void * usrPtr, PSrvEvent PEvent, int Size);
}
#pragma pack()

//---------------------------------------------------------------------------
// EVENTS QUEUE
//---------------------------------------------------------------------------
class TMsgEventQueue
{
private:
        int   IndexIn;   // <-- insert index
        int   IndexOut;  // --> extract index
        int   Max;       // Buffer upper bound [0..Max]
        int   FCapacity; // Queue capacity
        pbyte Buffer;
        int   FBlockSize;
public:
        TMsgEventQueue(const int Capacity, const int BlockSize);
        ~TMsgEventQueue();
        void Flush();
        void Insert(void *lpdata);
        bool Extract(void *lpdata);
        bool Empty();
        bool Full();
};
typedef TMsgEventQueue *PMsgEventQueue;

//---------------------------------------------------------------------------
// WORKER THREAD
//---------------------------------------------------------------------------
class TCustomMsgServer; // forward declaration

// It's created when connection is accepted, it will interface with the client.
class TMsgWorkerThread : public TSnapThread
{
private:
        TCustomMsgServer *FServer;
protected:
        TMsgSocket *WorkerSocket;
public:
        int Index;
        friend class TCustomMsgServer;
        TMsgWorkerThread(TMsgSocket *Socket, TCustomMsgServer *Server);
        void Execute();
};
typedef TMsgWorkerThread *PMsgWorkerThread;

//---------------------------------------------------------------------------
// LISTENER THREAD
//---------------------------------------------------------------------------
// It listens for incoming connection.
class TMsgListenerThread : public TSnapThread
{
private:
        TMsgSocket *FListener;
        TCustomMsgServer *FServer;
public:
        TMsgListenerThread(TMsgSocket *Listener, TCustomMsgServer *Server);
        void Execute();
};
typedef TMsgListenerThread *PMsgListenerThread;

//---------------------------------------------------------------------------
// TCP SERVER
//---------------------------------------------------------------------------
typedef TMsgSocket *PWorkerSocket;

class TCustomMsgServer
{
private:
        int FLastError;
        char FLocalAddress[16];
        // Socket listener
        PMsgSocket SockListener;
        // Server listener
        PMsgListenerThread ServerThread;
        // Critical section to lock Workers list activities
        PSnapCriticalSection CSList;
        // Event queue
        PMsgEventQueue FEventQueue;
        // Callback related
        pfn_SrvCallBack OnEvent;
        void *FUsrPtr;
        // private methods
        int StartListener();
        void LockList();
        void UnlockList();
        int FirstFree();
protected:
        bool Destroying;
        // Critical section to lock Event activities
        PSnapCriticalSection CSEvent;
	    // Workers list
        void *Workers[MaxWorkers];
        // Terminates all worker threads
        virtual void TerminateAll();
        // Kills all worker threads that are unresponsive
        void KillAll();
        // if (true the connection is accepted, otherwise the connection
        // is closed gracefully
        virtual bool CanAccept(socket_t Socket);
        // Returns the class of the worker socket, override it for real servers
        virtual PWorkerSocket CreateWorkerSocket(socket_t Sock);
        // Handles the event
        virtual void DoEvent(int Sender, longword Code, word RetCode, word Param1,
          word Param2, word Param3, word Param4);
        // Delete the worker from the list (It's invoked by Worker Thread)
        void Delete(int Index);
        // Incoming connection (It's invoked by ServerThread, the listener)
        virtual void Incoming(socket_t Sock);
public:
        friend class TMsgWorkerThread;
        friend class TMsgListenerThread;
        word LocalPort;
        longword LocalBind;
        longword LogMask;
        longword EventMask;
        int Status;
        int ClientsCount;
        int MaxClients;
        TCustomMsgServer();
        virtual ~TCustomMsgServer();
        // Starts the server
        int Start();
        int StartTo(const char *Address, word Port);
        // Stops the server
        void Stop();
        // Sets Event callback
        int SetEventsCallBack(pfn_SrvCallBack PCallBack, void *UsrPtr);
        // Pick an event from the circular queue
        bool PickEvent(void *pEvent);
        // Returns true if (the Event queue is empty
        bool EventEmpty();
        // Flushes Event queue
        void EventsFlush();
};

//---------------------------------------------------------------------------
// TCP WORKER
//---------------------------------------------------------------------------
// Default worker class, a simply tcp echo to test the connection and
// data I/O to use the server outside the project
class TEcoTcpWorker : public TMsgSocket
{
public:
	bool Execute()
	{
            byte Buffer[4096];
            int Size;

            if (CanRead(WorkInterval)) // Small time to avoid time wait during the close
            {
                Receive(&Buffer,sizeof(Buffer),Size);
                if ((LastTcpError==0) && (Size>0))
                {
                    SendPacket(&Buffer,Size);
                    return LastTcpError==0;
                }
                else
                    return false;
            }
        else
            return true;
	};
};

//---------------------------------------------------------------------------
#endif // snap_tcpsrvr_h
