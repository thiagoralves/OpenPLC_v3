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
#ifndef snap_msgsock_h
#define snap_msgsock_h
//---------------------------------------------------------------------------
#include "snap_platform.h"
#include "snap_sysutils.h"
//----------------------------------------------------------------------------
#if defined(OS_WINDOWS) || defined (OS_SOLARIS) || defined(OS_OSX)
# define MSG_NOSIGNAL    0
#endif
//----------------------------------------------------------------------------
// Non blocking connection to avoid root priviledges under UNIX 
// i.e. raw socket pinger is not more used.
// Thanks to Rolf Stalder that made it ;)
//----------------------------------------------------------------------------
#ifdef PLATFORM_UNIX
    #define NON_BLOCKING_CONNECT
#endif
#ifdef NON_BLOCKING_CONNECT
    #include <fcntl.h>
#endif
//----------------------------------------------------------------------------
/*
  In Windows sizeof socket varies depending of the platform :
    win32 -> sizeof(SOCKET) = 4
    win64 -> sizeof(SOCKET) = 8

  Even though sizeof(SOCKET) is 8, should be safe to cast it to int, because
  the value constitutes an index in per-process table of limited size
  and not a real pointer.

  Other Os define the socket as int regardless of the processor.

  We want to sleep peacefully, so it's better to define a portable socket.
*/

#ifdef OS_WINDOWS
typedef SOCKET socket_t;
#else
typedef int socket_t;
#endif

//----------------------------------------------------------------------------
#define  SD_RECEIVE      0x00
#define  SD_SEND         0x01
#define  SD_BOTH         0x02
#define  MaxPacketSize   65536

//----------------------------------------------------------------------------
// For other platform we need to re-define next constants
#if defined(PLATFORM_UNIX) || defined(OS_OSX)

#define INVALID_SOCKET (socket_t)(~0)
#define SOCKET_ERROR             (-1)

#define WSAEINTR             EINTR
#define WSAEBADF             EBADF
#define WSAEACCES            EACCES
#define WSAEFAULT            EFAULT
#define WSAEINVAL            EINVAL
#define WSAEMFILE            EMFILE
#define WSAEWOULDBLOCK       EWOULDBLOCK
#define WSAEINPROGRESS       EINPROGRESS
#define WSAEALREADY          EALREADY
#define WSAENOTSOCK          ENOTSOCK
#define WSAEDESTADDRREQ      EDESTADDRREQ
#define WSAEMSGSIZE          EMSGSIZE
#define WSAEPROTOTYPE        EPROTOTYPE
#define WSAENOPROTOOPT       ENOPROTOOPT
#define WSAEPROTONOSUPPORT   EPROTONOSUPPORT
#define WSAESOCKTNOSUPPORT   ESOCKTNOSUPPORT
#define WSAEOPNOTSUPP        EOPNOTSUPP
#define WSAEPFNOSUPPORT      EPFNOSUPPORT
#define WSAEAFNOSUPPORT      EAFNOSUPPORT
#define WSAEADDRINUSE        EADDRINUSE
#define WSAEADDRNOTAVAIL     EADDRNOTAVAIL
#define WSAENETDOWN          ENETDOWN
#define WSAENETUNREACH       ENETUNREACH
#define WSAENETRESET         ENETRESET
#define WSAECONNABORTED      ECONNABORTED
#define WSAECONNRESET        ECONNRESET
#define WSAENOBUFS           ENOBUFS
#define WSAEISCONN           EISCONN
#define WSAENOTCONN          ENOTCONN
#define WSAESHUTDOWN         ESHUTDOWN
#define WSAETOOMANYREFS      ETOOMANYREFS
#define WSAETIMEDOUT         ETIMEDOUT
#define WSAECONNREFUSED      ECONNREFUSED
#define WSAELOOP             ELOOP
#define WSAENAMETOOLONG      ENAMETOOLONG
#define WSAEHOSTDOWN         EHOSTDOWN
#define WSAEHOSTUNREACH      EHOSTUNREACH
#define WSAENOTEMPTY         ENOTEMPTY
#define WSAEUSERS            EUSERS
#define WSAEDQUOT            EDQUOT
#define WSAESTALE            ESTALE
#define WSAEREMOTE           EREMOTE
#endif

#define WSAEINVALIDADDRESS   12001

#define ICmpBufferSize 4096
typedef byte TIcmpBuffer[ICmpBufferSize];

// Ping result
#define PR_CANNOT_PERFORM -1  // cannot ping :
                              //   unix    : no root rights or SUID flag set to
                              //             open raw sockets
                              //   windows : neither helper DLL found nor raw
                              //             sockets can be opened (no administrator rights)
                              //  In this case the execution continues whitout
                              //  the benefit of the smart-connect.

#define PR_SUCCESS         0  // Host found
#define PR_ERROR           1  // Ping Error, Ping was performed but ...
                              //   - host didn't replied (not found)
                              //   - routing error
                              //   - TTL expired
                              //   - ... all other icmp error that we don't need
                              //      to know.

// Ping Kind
#define pkCannotPing  1  // see PR_CANNOT_PERFORM comments
#define pkWinHelper   2  // use iphlpapi.dll (only windows)
#define pkRawSocket   3  // use raw sockets (unix/windows)

const byte ICMP_ECHORP  = 0;  // ECHO Reply
const byte ICMP_ECHORQ  = 8;  // ECHO Request
//---------------------------------------------------------------------------
// RAW SOCKET PING STRUCTS
//---------------------------------------------------------------------------
#pragma pack(1)
typedef struct{
    byte ip_hl_v;
    byte ip_tos;
    word ip_len;
    word ip_id ;
    word ip_off;
    byte ip_ttl;
    byte ip_p;
    word ip_sum;
    longword ip_src;
    longword ip_dst;
}TIPHeader;

typedef struct{
    byte ic_type;  // Type of message
    byte ic_code;  // Code
    word ic_cksum; // 16 bit checksum
    word ic_id;    // ID (ic1 : ipv4)
    word ic_seq;   // Sequence
}TIcmpHeader;

typedef struct{
    TIcmpHeader Header;
    byte Data[32]; // use the well known default
}TIcmpPacket, *PIcmpPacket;

typedef struct{
    TIPHeader IPH;
    TIcmpPacket ICmpReply;
}TIcmpReply, *PIcmpReply;
#pragma pack()

//---------------------------------------------------------------------------
class TRawSocketPinger
{
private:
    socket_t FSocket;
    PIcmpPacket SendPacket;
    TIcmpBuffer IcmpBuffer;
    word FId, FSeq;
    void InitPacket();
    word PacketChecksum();
    bool CanRead(int Timeout);
public:
    bool Ping(longword ip_addr, int Timeout);
    TRawSocketPinger();
    ~TRawSocketPinger();
};
typedef TRawSocketPinger *PRawSocketPinger;
//---------------------------------------------------------------------------
class TPinger
{
private:
    PRawSocketPinger RawPinger;
    bool RawAvail;
#ifdef OS_WINDOWS
	bool WinPing(longword ip_addr, int Timeout);
#endif
	bool RawPing(longword ip_addr, int Timeout);
public:
    TPinger();
    ~TPinger();
	bool Ping(char *Host, int Timeout);
	bool Ping(longword ip_addr, int Timeout);
};
typedef TPinger *PPinger;
//---------------------------------------------------------------------------
class TSnapBase // base class endian-aware
{
private:
		bool LittleEndian;
protected:
		longword SwapDWord(longword Value);
		word SwapWord(word Value);
public:
		TSnapBase();
};
//---------------------------------------------------------------------------
class TMsgSocket : public TSnapBase
{
private:
        PPinger Pinger;
        int GetLastSocketError();
        int SockCheck(int SockResult);
        void DestroySocket();
        void SetSocketOptions();
        bool CanWrite(int Timeout);
        void GetLocal();
        void GetRemote();
        void SetSin(sockaddr_in &sin, char *Address, u_short Port);
        void GetSin(sockaddr_in sin, char *Address, u_short &Port);
protected:
        socket_t FSocket;
        sockaddr_in LocalSin;
        sockaddr_in RemoteSin;
        //--------------------------------------------------------------------------
        // low level socket
        void CreateSocket();
        // Called when a socket is assigned externally
        void GotSocket();
        // Returns how many bytes are ready to be read in the winsock buffer
        int WaitingData();
        // Waits until there at least "size" bytes ready to be read or until receive timeout occurs
        int WaitForData(int Size, int Timeout);
        // Clear socket input buffer
        void Purge();
public:
        longword ClientHandle;
        longword LocalBind;
        // Coordinates Address:Port
        char LocalAddress[16];
        char RemoteAddress[16];
        word LocalPort;
        word RemotePort;
        // "speed" of the socket listener (used server-side)
        int WorkInterval;
        // Timeouts : 3 different values for fine tuning.
        // Send timeout should be small since with work with small packets and TCP_NO_DELAY
        //   option, so we don't expect "time to wait".
        // Recv timeout depends of equipment's processing time : we send a packet, the equipment
        //   processes the message, finally it sends the answer. In any case Recv timeout > Send Timeout.
        // PingTimeout is the maximum time interval during which we expect that the PLC answers.
        //   By default is 750 ms, increase it if there are many switch/repeaters.
        int PingTimeout;
        int RecvTimeout;
        int SendTimeout;
        //int ConnTimeout;
        // Output : Last operation error
        int LastTcpError;
        // Output : Connected to the remote Host/Peer/Client
        bool Connected;
        //--------------------------------------------------------------------------
        TMsgSocket();
        virtual ~TMsgSocket();
        // Returns true if "something" can be read during the Timeout interval..
        bool CanRead(int Timeout);
        // Connects to a peer (using RemoteAddress and RemotePort)
        int SckConnect(); // (client-side)
        // Disconnects from a peer (gracefully)
        void SckDisconnect();
        // Disconnects RAW
        void ForceClose();
        // Binds to a local adapter (using LocalAddress and LocalPort) (server-side)
        int SckBind();
        // Listens for an incoming connection (server-side)
        int SckListen();
        // Set an external socket reference (tipically from a listener)
        void SetSocket(socket_t s);
        // Accepts an incoming connection returning a socket descriptor (server-side)
        socket_t SckAccept();
        // Pings the peer before connecting
        bool Ping(char *Host);
        bool Ping(sockaddr_in Addr);
        // Sends a packet
        int SendPacket(void *Data,  int Size);
        // Returns true if a Packet at least of "Size" bytes is ready to be read
        bool PacketReady(int Size);
        // Receives everything
        int Receive(void *Data, int BufSize, int & SizeRecvd);
        // Receives a packet of size specified.
        int RecvPacket(void *Data, int Size);
        // Peeks a packet of size specified without extract it from the socket queue
        int PeekPacket(void *Data, int Size);
        virtual bool Execute();
};

typedef TMsgSocket *PMsgSocket;
//---------------------------------------------------------------------------
void Msg_CloseSocket(socket_t FSocket);
longword Msg_GetSockAddr(socket_t FSocket);
//---------------------------------------------------------------------------
class SocketsLayer
{
private:
#ifdef OS_WINDOWS
    WSADATA wsaData;
#endif
public:
    SocketsLayer();
    ~SocketsLayer();
};

#endif // snap_msgsock_h
