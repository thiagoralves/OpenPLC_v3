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

#include "snap_msgsock.h"

//---------------------------------------------------------------------------

static SocketsLayer SocketsLayerInitializer;

//---------------------------------------------------------------------------
//  Base class endian aware
//---------------------------------------------------------------------------
TSnapBase::TSnapBase()
{
	int x = 1;
	LittleEndian=*(char *)&x == 1;
}
//---------------------------------------------------------------------------
word TSnapBase::SwapWord(word Value)
{
	if (LittleEndian)
		return  ((Value >> 8) & 0xFF) | ((Value << 8) & 0xFF00);
	else
	    return Value;
}
//---------------------------------------------------------------------------
longword TSnapBase::SwapDWord(longword Value)
{
	if (LittleEndian)
		return (Value >> 24) | ((Value << 8) & 0x00FF0000) | ((Value >> 8) & 0x0000FF00) | (Value << 24);
	else
		return Value;
}
//---------------------------------------------------------------------------
void Msg_CloseSocket(socket_t FSocket)
{
    #ifdef OS_WINDOWS
    closesocket(FSocket);
    #else
    close(FSocket);
    #endif
}
//---------------------------------------------------------------------------
longword Msg_GetSockAddr(socket_t FSocket)
{
    sockaddr_in RemoteSin;
    #ifdef OS_WINDOWS
    int namelen = sizeof(RemoteSin);
    #else
    uint32_t namelen = sizeof(RemoteSin);
    #endif
    namelen=sizeof(sockaddr_in);
    if (getpeername(FSocket,(struct sockaddr*)&RemoteSin, &namelen)==0)
        return RemoteSin.sin_addr.s_addr;
    else
        return 0;
}
//---------------------------------------------------------------------------
TMsgSocket::TMsgSocket()
{
    Pinger = new TPinger();
    // Set Defaults
    strcpy(LocalAddress,"0.0.0.0");
    LocalPort=0;
    strcpy(RemoteAddress,"127.0.0.1");
    RemotePort=0;
    WorkInterval=100;
    RecvTimeout=500;
    SendTimeout=10;
    PingTimeout=750;
    Connected=false;
    FSocket=INVALID_SOCKET;
    LastTcpError=0;
    LocalBind=0;
}
//---------------------------------------------------------------------------
TMsgSocket::~TMsgSocket()
{
    DestroySocket();
    delete Pinger;
}
//---------------------------------------------------------------------------
void TMsgSocket::SetSin(sockaddr_in &sin, char *Address, u_short Port)
{	
    uint32_t in_addr;
    in_addr=inet_addr(Address);
    memset(&sin, 0, sizeof(sin));
    LastTcpError=0;

    if (in_addr!=INADDR_NONE)
    {
		sin.sin_addr.s_addr = in_addr; //INADDR_ANY; 
		sin.sin_family = AF_INET;
		sin.sin_port = htons(Port);
    }
    else
        LastTcpError=WSAEINVALIDADDRESS;
}
//---------------------------------------------------------------------------
void TMsgSocket::GetSin(sockaddr_in sin, char *Address, u_short &Port)
{
  strcpy(Address,inet_ntoa(sin.sin_addr));
  Port=htons(sin.sin_port);
}
//---------------------------------------------------------------------------
void TMsgSocket::GetLocal()
{
    #ifdef OS_WINDOWS
		int namelen = sizeof(LocalSin);
    #else
        uint32_t namelen = sizeof(LocalSin);
    #endif
    if (getsockname(FSocket, (struct sockaddr*)&LocalSin, &namelen)==0)
        GetSin(LocalSin, LocalAddress, LocalPort);
}
//---------------------------------------------------------------------------
void TMsgSocket::GetRemote()
{
    #ifdef OS_WINDOWS
        int namelen = sizeof(RemoteSin);
    #else
        uint32_t namelen = sizeof(RemoteSin);
    #endif
	if (getpeername(FSocket,(struct sockaddr*)&RemoteSin, &namelen)==0)
		GetSin(RemoteSin, RemoteAddress, RemotePort);
}
//---------------------------------------------------------------------------
int TMsgSocket::GetLastSocketError()
{
#ifdef OS_WINDOWS
    return WSAGetLastError();
#else    
    return errno;
#endif
}
//---------------------------------------------------------------------------
void TMsgSocket::Purge()
{
    // small buffer to empty the socket
    char Trash[512];
    int Read;
    if (LastTcpError!=WSAECONNRESET)
    {
        if (CanRead(0)) {
           do
           {
               Read=recv(FSocket, Trash, 512, MSG_NOSIGNAL );
           } while(Read==512);
        }
    }
}
//---------------------------------------------------------------------------
void TMsgSocket::CreateSocket()
{
	DestroySocket();
	LastTcpError=0;
	FSocket =socket(AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if (FSocket!=INVALID_SOCKET)
		SetSocketOptions();
	else
		LastTcpError =GetLastSocketError();
}
//---------------------------------------------------------------------------
void TMsgSocket::GotSocket()
{
    ClientHandle=RemoteSin.sin_addr.s_addr;
    // could be inherited it if wee need further actions on the socket
}
//---------------------------------------------------------------------------
void TMsgSocket::SetSocket(socket_t s)
{
    FSocket=s;
    if (FSocket!=INVALID_SOCKET)
    {
		SetSocketOptions();
		GetLocal();
		GetRemote();
		GotSocket();
    }
    Connected=FSocket!=INVALID_SOCKET;
}
//---------------------------------------------------------------------------
void TMsgSocket::DestroySocket()
{
    if(FSocket != INVALID_SOCKET)
    {        
        if (shutdown(FSocket, SD_SEND)==0)
			Purge();
	#ifdef OS_WINDOWS
		closesocket(FSocket);
	#else
		close(FSocket);
	#endif
		FSocket=INVALID_SOCKET;
    }
    LastTcpError=0;
}
//---------------------------------------------------------------------------
int TMsgSocket::WaitingData()
{
    int result = 0;
    u_long x = 0;
#ifdef OS_WINDOWS
    if (ioctlsocket(FSocket, FIONREAD, &x) == 0)
        result = x;
#else
    if (ioctl(FSocket, FIONREAD, &x) == 0)
        result = x;
#endif
    if (result>MaxPacketSize)
        result = MaxPacketSize;
    return result;
}
//---------------------------------------------------------------------------
int TMsgSocket::WaitForData(int Size, int Timeout)
{
    longword Elapsed;

    // Check for connection active
    if (CanRead(0) && (WaitingData()==0))
        LastTcpError=WSAECONNRESET;
    else
        LastTcpError=0;

    // Enter main loop
    if (LastTcpError==0)
    {
        Elapsed =SysGetTick();
        while((WaitingData()<Size) && (LastTcpError==0))
        {
            // Checks timeout
            if (DeltaTime(Elapsed)>=(longword)(Timeout))
                LastTcpError =WSAETIMEDOUT;
            else
                SysSleep(1);
        }
    }
    if(LastTcpError==WSAECONNRESET)
            Connected =false;

    return LastTcpError;
}
//---------------------------------------------------------------------------
void TMsgSocket::SetSocketOptions()
{
    int NoDelay = 1;
	int KeepAlive = 1;
    LastTcpError=0;
    SockCheck(setsockopt(FSocket, IPPROTO_TCP, TCP_NODELAY,(char*)&NoDelay, sizeof(NoDelay)));

	if (LastTcpError==0)
        SockCheck(setsockopt(FSocket, SOL_SOCKET, SO_KEEPALIVE,(char*)&KeepAlive, sizeof(KeepAlive)));
}
//---------------------------------------------------------------------------
int TMsgSocket::SockCheck(int SockResult)
{
  if (SockResult == (int)(SOCKET_ERROR))
     LastTcpError = GetLastSocketError();

  return LastTcpError;
}
//---------------------------------------------------------------------------
bool TMsgSocket::CanWrite(int Timeout)
{
    timeval TimeV;
    int64_t x;
    fd_set FDset;

	if(FSocket == INVALID_SOCKET)
		return false;

    TimeV.tv_usec = (Timeout % 1000) * 1000;
    TimeV.tv_sec = Timeout / 1000;

    FD_ZERO(&FDset);
    FD_SET(FSocket, &FDset);

    x = select(FSocket + 1, NULL, &FDset, NULL, &TimeV); //<-Ignore this warning in 64bit Visual Studio
    if (x==(int)SOCKET_ERROR) 
    {
        LastTcpError = GetLastSocketError();
        x=0;
    }
    return (x > 0);
}
//---------------------------------------------------------------------------
bool TMsgSocket::CanRead(int Timeout)
{
    timeval TimeV;
    int64_t x;
    fd_set FDset;

	if(FSocket == INVALID_SOCKET)
		return false;

	TimeV.tv_usec = (Timeout % 1000) * 1000;
    TimeV.tv_sec = Timeout / 1000;

    FD_ZERO(&FDset);
    FD_SET(FSocket, &FDset);

    x = select(FSocket + 1, &FDset, NULL, NULL, &TimeV); //<-Ignore this warning in 64bit Visual Studio
    if (x==(int)SOCKET_ERROR)
    {
       LastTcpError = GetLastSocketError();
       x=0;
    }
    return (x > 0);
}
//---------------------------------------------------------------------------
#ifdef NON_BLOCKING_CONNECT
//
// Non blocking connection (UNIX) Thanks to Rolf Stalder
//
int TMsgSocket::SckConnect()
{
	int n, flags, err;
	socklen_t len;
	fd_set rset, wset;
	struct timeval tval;

 	SetSin(RemoteSin, RemoteAddress, RemotePort);

	if (LastTcpError == 0) {
		CreateSocket();
		if (LastTcpError == 0) {
			flags = fcntl(FSocket, F_GETFL, 0);
			if (flags >= 0) {
				if (fcntl(FSocket, F_SETFL, flags | O_NONBLOCK)  != -1) {
					n = connect(FSocket, (struct sockaddr*)&RemoteSin, sizeof(RemoteSin));
					if (n < 0) {
						if (errno != EINPROGRESS) {
							LastTcpError = GetLastSocketError();
						}
						else {
							// still connecting ... 
							FD_ZERO(&rset);
							FD_SET(FSocket, &rset);
							wset = rset;
							tval.tv_sec = PingTimeout / 1000;
							tval.tv_usec = (PingTimeout % 1000) * 1000;

							n = select(FSocket+1, &rset, &wset, NULL, 
							           (PingTimeout ? &tval : NULL));
							if (n == 0) {
								// timeout
								LastTcpError = WSAEHOSTUNREACH;
							}
							else {
								if (FD_ISSET(FSocket, &rset) || FD_ISSET(FSocket, &wset)) {
									err = 0;
									len = sizeof(err);						
									if (getsockopt(
									      FSocket, SOL_SOCKET, SO_ERROR, &err, &len) == 0) {
										if (err) {
											 LastTcpError = err;
										}
										else {
											if (fcntl(FSocket, F_SETFL, flags) != -1) {
												GetLocal();
												ClientHandle = LocalSin.sin_addr.s_addr;
											}
											else {
												LastTcpError = GetLastSocketError();
											}
										}
									}
									else {
										LastTcpError = GetLastSocketError();
									}
								}
								else {
									LastTcpError = -1;
								}
							}
						} // still connecting
					} 
					else if (n == 0) { 
						// connected immediatly
						GetLocal();
						ClientHandle = LocalSin.sin_addr.s_addr;
					}
				}
				else {
					LastTcpError = GetLastSocketError();
				} // fcntl(F_SETFL)
			}
			else {
				LastTcpError = GetLastSocketError();
			} // fcntl(F_GETFL)
		} //valid socket 
	} // LastTcpError==0
	Connected=LastTcpError==0;
 	return LastTcpError;
}
#else
//
// Regular connection (Windows)
//
int TMsgSocket::SckConnect()
{
    int Result;
    SetSin(RemoteSin, RemoteAddress, RemotePort);
    if (LastTcpError==0)
    {
        if (Ping(RemoteSin))
        {
            CreateSocket();
            if (LastTcpError==0)
            {
                Result=connect(FSocket, (struct sockaddr*)&RemoteSin, sizeof(RemoteSin));
                if (SockCheck(Result)==0)
                {
                    GetLocal();
                    // Client handle is self_address (here the connection is ACTIVE)
                    ClientHandle=LocalSin.sin_addr.s_addr;
                }
            }
        }
        else
            LastTcpError=WSAEHOSTUNREACH;
    }
    Connected=LastTcpError==0;
    return LastTcpError;
}
#endif
//---------------------------------------------------------------------------
void TMsgSocket::SckDisconnect()
{
    DestroySocket();
    Connected=false;
}
//---------------------------------------------------------------------------
void TMsgSocket::ForceClose()
{
    if(FSocket != INVALID_SOCKET)
    {
        try {
            #ifdef OS_WINDOWS
            closesocket(FSocket);
            #else
            close(FSocket);
            #endif
        } catch (...) {
        }
        FSocket=INVALID_SOCKET;
    }
    LastTcpError=0;
}
//---------------------------------------------------------------------------
int TMsgSocket::SckBind()
{
    int Res;
    int Opt=1;
    SetSin(LocalSin, LocalAddress, LocalPort);
    if (LastTcpError==0)
    {
        CreateSocket();
        if (LastTcpError==0)
        {		
            setsockopt(FSocket ,SOL_SOCKET, SO_REUSEADDR, (const char *)&Opt, sizeof(int));
            Res =bind(FSocket, (struct sockaddr*)&LocalSin, sizeof(sockaddr_in));
            SockCheck(Res);
            if (Res==0) 
            {
                LocalBind=LocalSin.sin_addr.s_addr;
            }
        }
    }
    else
        LastTcpError=WSAEINVALIDADDRESS;

    return LastTcpError;
}
//---------------------------------------------------------------------------
int TMsgSocket::SckListen()
{
    LastTcpError=0;
    SockCheck(listen(FSocket ,SOMAXCONN));
    return LastTcpError;
}
//---------------------------------------------------------------------------
bool TMsgSocket::Ping(char *Host)
{
    return Pinger->Ping(Host, PingTimeout);
}
//---------------------------------------------------------------------------
bool TMsgSocket::Ping(sockaddr_in Addr)
{
	if (PingTimeout == 0)
		return true;
	else
        return Pinger->Ping(Addr.sin_addr.s_addr, PingTimeout);
}
//---------------------------------------------------------------------------
socket_t TMsgSocket::SckAccept()
{
    socket_t result;
    LastTcpError=0;
    result = accept(FSocket, NULL, NULL);
    if(result==INVALID_SOCKET)
        LastTcpError =GetLastSocketError();
    return result;
}
//---------------------------------------------------------------------------
int TMsgSocket::SendPacket(void *Data, int Size)
{
    int Result;
    
    LastTcpError=0;
    if (SendTimeout>0)
    {
        if (!CanWrite(SendTimeout))
        {
            LastTcpError = WSAETIMEDOUT;
            return LastTcpError;
        }
    }
    if (send(FSocket, (char*)Data, Size, MSG_NOSIGNAL)==Size)
        return 0;
    else
        Result =SOCKET_ERROR;

    SockCheck(Result);
    return Result;
}
//---------------------------------------------------------------------------
bool TMsgSocket::PacketReady(int Size)
{
	return (WaitingData()>=Size);
}
//---------------------------------------------------------------------------
int TMsgSocket::Receive(void *Data, int BufSize, int &SizeRecvd)
{
    LastTcpError=0;
    if (CanRead(RecvTimeout))
    {
        SizeRecvd=recv(FSocket ,(char*)Data ,BufSize ,MSG_NOSIGNAL );

        if (SizeRecvd>0) // something read (default case)
           LastTcpError=0;
        else
            if (SizeRecvd==0)
                LastTcpError = WSAECONNRESET;  // Connection reset by Peer
            else
                LastTcpError=GetLastSocketError(); // we need to know what happened
    }
    else
        LastTcpError = WSAETIMEDOUT;

    if (LastTcpError==WSAECONNRESET)
        Connected = false;

    return LastTcpError;
}
//---------------------------------------------------------------------------
int TMsgSocket::RecvPacket(void *Data, int Size)
{
    int BytesRead;
    WaitForData(Size, RecvTimeout);
    if (LastTcpError==0)
    {
        BytesRead=recv(FSocket, (char*)Data, Size, MSG_NOSIGNAL);
        if (BytesRead==0)
            LastTcpError = WSAECONNRESET;  // Connection reset by Peer
        else
            if (BytesRead<0)
                LastTcpError = GetLastSocketError();
    }
    else // After the timeout the bytes waiting were less then we expected
        if (LastTcpError==WSAETIMEDOUT)
            Purge();

    if (LastTcpError==WSAECONNRESET)
        Connected =false;

    return LastTcpError;
}
//---------------------------------------------------------------------------
int TMsgSocket::PeekPacket(void *Data, int Size)
{
    int BytesRead;
    WaitForData(Size, RecvTimeout);
    if (LastTcpError==0)
    {
        BytesRead=recv(FSocket, (char*)Data, Size, MSG_PEEK | MSG_NOSIGNAL );
        if (BytesRead==0)
            LastTcpError = WSAECONNRESET;  // Connection reset by Peer
        else
            if (BytesRead<0)
                LastTcpError = GetLastSocketError();
    }
    else // After the timeout the bytes waiting were less then we expected
        if (LastTcpError==WSAETIMEDOUT)
            Purge();

    if (LastTcpError==WSAECONNRESET)
        Connected =false;

    return LastTcpError;
}
//---------------------------------------------------------------------------
bool TMsgSocket::Execute()
{
    return true;
}
//==============================================================================
// PING
//==============================================================================

static int PingKind;

#ifdef OS_WINDOWS
// iphlpapi, is loaded dinamically because if this fails we can still try
// to use raw sockets

static char const *iphlpapi = "\\iphlpapi.dll";
#pragma pack(1)

//typedef byte TTxBuffer[40];
typedef byte TTxBuffer[32];
#pragma pack()

typedef HANDLE (__stdcall *pfn_IcmpCreateFile)();
typedef bool (__stdcall *pfn_IcmpCloseHandle)(HANDLE PingHandle);

typedef int (__stdcall *pfn_IcmpSendEcho2)(
    HANDLE PingHandle,
    void *Event,
    void *AcpRoutine,
    void *AcpContext,
    unsigned long DestinationAddress,
    void *RequestData,
    int RequestSize,
    void *not_used,  //should be *IP_OPTION_INFORMATION but we don't use it
    void *ReplyBuffer,
    int ReplySize,
    int Timeout
);

static pfn_IcmpCreateFile IcmpCreateFile;
static pfn_IcmpCloseHandle IcmpCloseHandle;
static pfn_IcmpSendEcho2 IcmpSendEcho2;
static HINSTANCE IcmpDllHandle = 0;
static bool IcmpAvail = false;

bool IcmpInit()
{
	char iphlppath[MAX_PATH+12];

	int PathLen = GetSystemDirectoryA(iphlppath, MAX_PATH);
	if (PathLen != 0)
	{
		strcat(iphlppath, iphlpapi);
		IcmpDllHandle = LoadLibraryA(iphlppath);
	}
	else
		IcmpDllHandle = 0;

    if (IcmpDllHandle != 0)
    {
        IcmpCreateFile=(pfn_IcmpCreateFile)GetProcAddress(IcmpDllHandle,"IcmpCreateFile");
        IcmpCloseHandle=(pfn_IcmpCloseHandle)GetProcAddress(IcmpDllHandle,"IcmpCloseHandle");
        IcmpSendEcho2=(pfn_IcmpSendEcho2)GetProcAddress(IcmpDllHandle,"IcmpSendEcho2");
        return (IcmpCreateFile!=NULL) && (IcmpCloseHandle!=NULL) && (IcmpSendEcho2!=NULL);
    }
    else
        return false;
}

void IcmpDone()
{
    if (IcmpDllHandle!=0)
       FreeLibrary(IcmpDllHandle);
    IcmpAvail=false;
}
#endif

//---------------------------------------------------------------------------
//  RAW Socket Pinger
//---------------------------------------------------------------------------
TRawSocketPinger::TRawSocketPinger()
{
    FSocket =socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    FId  =word(size_t(this));
    FSeq =0;
}
//---------------------------------------------------------------------------
TRawSocketPinger::~TRawSocketPinger()
{
    if (FSocket!=INVALID_SOCKET)
    {
      #ifdef OS_WINDOWS
          closesocket(FSocket);
      #else
          close(FSocket);
      #endif
      FSocket=INVALID_SOCKET;
    };
}
//---------------------------------------------------------------------------
void TRawSocketPinger::InitPacket()
{
    memset(&IcmpBuffer,0,ICmpBufferSize);
    FSeq++;
    
	SendPacket=PIcmpPacket(pbyte(&IcmpBuffer)+sizeof(TIPHeader));
    SendPacket->Header.ic_type=ICMP_ECHORQ;
    SendPacket->Header.ic_code=0;
    SendPacket->Header.ic_cksum=0;
    SendPacket->Header.ic_id=FId;
    SendPacket->Header.ic_seq=FSeq;
 
    memset(&SendPacket->Data,0,sizeof(SendPacket->Data));
    SendPacket->Header.ic_cksum=PacketChecksum();
}
//---------------------------------------------------------------------------
word TRawSocketPinger::PacketChecksum()
{
    word *P = (word*)(SendPacket);
    longword Sum = 0;
    int c;
    for (c = 0; c < int(sizeof(TIcmpPacket) / 2); c++) {
        Sum+=*P;
        P++;
    }
    Sum=(Sum >> 16) + (Sum & 0xFFFF);
    Sum=Sum+(Sum >> 16);
    return word(~Sum);
}
//---------------------------------------------------------------------------
bool TRawSocketPinger::CanRead(int Timeout)
{
    timeval TimeV;
    int64_t x;
    fd_set FDset;

    TimeV.tv_usec = (Timeout % 1000) * 1000;
    TimeV.tv_sec = Timeout / 1000;

    FD_ZERO(&FDset);
    FD_SET(FSocket, &FDset);

    x = select(FSocket + 1, &FDset, NULL, NULL, &TimeV); //<-Ignore this warning in 64bit Visual Studio
    if (x==(int)(SOCKET_ERROR))
       x=0;
    return (x > 0);
}
//---------------------------------------------------------------------------
bool TRawSocketPinger::Ping(longword ip_addr, int Timeout)
{
    sockaddr_in LSockAddr;
    sockaddr_in RSockAddr;
    PIcmpReply Reply;

    if (FSocket==INVALID_SOCKET)
      return true;

    // Init packet
    InitPacket();
    Reply=PIcmpReply(&IcmpBuffer);
    // Init Remote and Local Addresses struct
    RSockAddr.sin_family=AF_INET;
    RSockAddr.sin_port=0;
    RSockAddr.sin_addr.s_addr=ip_addr;

    LSockAddr.sin_family=AF_INET;
    LSockAddr.sin_port=0;
    LSockAddr.sin_addr.s_addr=inet_addr("0.0.0.0");

    // Bind to local
    if (bind(FSocket, (struct sockaddr*)&LSockAddr, sizeof(sockaddr_in))!=0)
        return false;
    // Connect to remote (not a really TCP connection, only to setup the socket)
    if (connect(FSocket, (struct sockaddr*)&RSockAddr, sizeof(sockaddr_in))!=0)
        return false;
    // Send ICMP packet
    if (send(FSocket, (char*)SendPacket, sizeof(TIcmpPacket), MSG_NOSIGNAL)!=int(sizeof(TIcmpPacket)))
        return false;
    // Wait for a reply
    if (!CanRead(Timeout))
        return false;// time expired
    // Get the answer
    if (recv(FSocket, (char*)&IcmpBuffer, ICmpBufferSize, MSG_NOSIGNAL)<int(sizeof(TIcmpReply)))
        return false;
    // Check the answer
    return (Reply->IPH.ip_src==RSockAddr.sin_addr.s_addr) &&  // the peer is what we are looking for
           (Reply->ICmpReply.Header.ic_type==ICMP_ECHORP);    // type = reply
}
//---------------------------------------------------------------------------
//  Pinger
//---------------------------------------------------------------------------
TPinger::TPinger()
{
}
//---------------------------------------------------------------------------
TPinger::~TPinger()
{
}
//---------------------------------------------------------------------------
bool TPinger::RawPing(longword ip_addr, int Timeout)
{
    PRawSocketPinger RawPinger = new TRawSocketPinger();
    bool Result;
    Result=RawPinger->Ping(ip_addr, Timeout);
    delete RawPinger;
    return Result;
}
//---------------------------------------------------------------------------
#ifdef OS_WINDOWS
bool TPinger::WinPing(longword ip_addr, int Timeout)
{
    HANDLE PingHandle;
    TTxBuffer TxBuffer;
    TIcmpBuffer IcmpBuffer;
    bool Result;

    PingHandle = IcmpCreateFile();
    if (PingHandle != INVALID_HANDLE_VALUE)
    {
        memset(&TxBuffer,'\55',sizeof(TTxBuffer));
        Result=(IcmpSendEcho2(PingHandle, NULL, NULL, NULL, ip_addr,
            &TxBuffer, sizeof(TxBuffer), NULL, &IcmpBuffer, ICmpBufferSize, Timeout))>0;
        IcmpCloseHandle(PingHandle);
        return Result;
    }
    else
        return false;
}
#endif
//---------------------------------------------------------------------------
bool TPinger::Ping(char *Host, int Timeout)
{
    longword Addr;
    Addr=inet_addr(Host);
    return Ping(Addr, Timeout);
}
//---------------------------------------------------------------------------
bool TPinger::Ping(longword ip_addr, int Timeout)
{
#ifdef OS_WINDOWS
    if (PingKind==pkWinHelper)
        return WinPing(ip_addr, Timeout);
    else
#endif
    if (PingKind==pkRawSocket)
        return RawPing(ip_addr, Timeout);
    else
        return true; // we still need to continue
}
//---------------------------------------------------------------------------
// Checks if raw sockets are allowed
//---------------------------------------------------------------------------
bool RawSocketsCheck()
{
    socket_t RawSocket;
    bool Result;
    RawSocket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    Result=RawSocket != INVALID_SOCKET;
    if (Result)
    #ifdef OS_WINDOWS
        closesocket(RawSocket);
    #else
        close(RawSocket);
    #endif

    return Result;
}
//---------------------------------------------------------------------------
// Sockets init
// - Winsock Startup (Windows)
// - ICMP Helper Load (Windows)
// - Check for raw socket (Unix or Windows if ICMP load failed)
//---------------------------------------------------------------------------
SocketsLayer::SocketsLayer()
{
#ifdef OS_WINDOWS
    timeBeginPeriod(1); // it's not strictly related to socket but here is a nice place
    WSAStartup(0x202,&wsaData);
    if (IcmpInit())
       PingKind=pkWinHelper;
    else
#endif
    if (RawSocketsCheck())
        PingKind=pkRawSocket;
    else
        PingKind=pkCannotPing;
}

SocketsLayer::~SocketsLayer()
{
#ifdef OS_WINDOWS
    IcmpDone();
    WSACleanup();
    timeEndPeriod(1);
#endif
}


