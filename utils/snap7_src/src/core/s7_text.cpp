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
#include "s7_text.h"
//---------------------------------------------------------------------------
#ifndef OS_WINDOWS
static char* itoa(int value, char* result, int base) {
	// check that the base if valid
	if (base < 2 || base > 36){
		*result = '\0'; return result;

	}
	char* ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;

	do {
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
	} while ( value );

	// Apply negative sign
	if (tmp_value < 0) *ptr++ = '-';
	*ptr-- = '\0';
	while(ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}
#endif
//---------------------------------------------------------------------------
char* NumToString(int Value, int Base, int Len, char* Result)
{
	char CNumber[64];
	char Pad[65] = "0000000000000000000000000000000000000000000000000000000000000000";
	itoa(Value, CNumber, Base);
	
	if (Len > 0)
	{
		int Delta = Len - strlen(CNumber); // Len is max 8 in this program
		if (Delta > 0)
		{
			strncpy(Result, Pad, Delta);
			Result[Delta] = '\0';
			strcat(Result, CNumber);
		}
		else
			strcpy(Result, CNumber);
	}
	else
		strcpy(Result, CNumber);

	return Result;
}
//---------------------------------------------------------------------------
char* IntToString(int Value, char* Result)
{
	return NumToString(Value, 10, 0, Result);
}
//---------------------------------------------------------------------------
char* TimeToString(time_t dt, char* Result)
{
	struct tm * DateTime = localtime(&dt);
	if (DateTime != NULL) 
		strftime(Result, 50, "%Y-%m-%d %H:%M:%S", DateTime);
	else
		*Result = '\0';
	return Result;
}

//---------------------------------------------------------------------------
char* IpAddressToString(int IP, char* Result)
{
	in_addr Addr;
	Addr.s_addr = IP;
	strcpy(Result, inet_ntoa(Addr));
	return Result;
}
//---------------------------------------------------------------------------
#define WSAEINVALIDADDRESS   12001

char* TcpTextOf(int Error, char* Result)
{
	switch (Error)
	{
	case 0:                   *Result='\0';break;
	case WSAEINTR:            strcpy(Result," TCP : Interrupted system call\0");break;
	case WSAEBADF:            strcpy(Result," TCP : Bad file number\0");break;
	case WSAEACCES:           strcpy(Result," TCP : Permission denied\0");break;
	case WSAEFAULT:           strcpy(Result," TCP : Bad address\0");break;
	case WSAEINVAL:           strcpy(Result," TCP : Invalid argument\0");break;
	case WSAEMFILE:           strcpy(Result," TCP : Too many open files\0");break;
	case WSAEWOULDBLOCK:      strcpy(Result," TCP : Operation would block\0");break;
	case WSAEINPROGRESS:      strcpy(Result," TCP : Operation now in progress\0");break;
	case WSAEALREADY:         strcpy(Result," TCP : Operation already in progress\0");break;
	case WSAENOTSOCK:         strcpy(Result," TCP : Socket operation on non socket\0");break;
	case WSAEDESTADDRREQ:     strcpy(Result," TCP : Destination address required\0");break;
	case WSAEMSGSIZE:         strcpy(Result," TCP : Message too long\0");break;
	case WSAEPROTOTYPE:       strcpy(Result," TCP : Protocol wrong type for Socket\0");break;
	case WSAENOPROTOOPT:      strcpy(Result," TCP : Protocol not available\0");break;
	case WSAEPROTONOSUPPORT:  strcpy(Result," TCP : Protocol not supported\0");break;
	case WSAESOCKTNOSUPPORT:  strcpy(Result," TCP : Socket not supported\0");break;
	case WSAEOPNOTSUPP:       strcpy(Result," TCP : Operation not supported on Socket\0");break;
	case WSAEPFNOSUPPORT:     strcpy(Result," TCP : Protocol family not supported\0");break;
	case WSAEAFNOSUPPORT:     strcpy(Result," TCP : Address family not supported\0");break;
	case WSAEADDRINUSE:       strcpy(Result," TCP : Address already in use\0");break;
	case WSAEADDRNOTAVAIL:    strcpy(Result," TCP : Can't assign requested address\0");break;
	case WSAENETDOWN:         strcpy(Result," TCP : Network is down\0");break;
	case WSAENETUNREACH:      strcpy(Result," TCP : Network is unreachable\0");break;
	case WSAENETRESET:        strcpy(Result," TCP : Network dropped connection on reset\0");break;
	case WSAECONNABORTED:     strcpy(Result," TCP : Software caused connection abort\0");break;
	case WSAECONNRESET:       strcpy(Result," TCP : Connection reset by peer\0");break;
	case WSAENOBUFS:          strcpy(Result," TCP : No Buffer space available\0");break;
	case WSAEISCONN:          strcpy(Result," TCP : Socket is already connected\0");break;
	case WSAENOTCONN:         strcpy(Result," TCP : Socket is not connected\0");break;
	case WSAESHUTDOWN:        strcpy(Result," TCP : Can't send after Socket shutdown\0");break;
	case WSAETOOMANYREFS:     strcpy(Result," TCP : Too many references:can't splice\0");break;
	case WSAETIMEDOUT:        strcpy(Result," TCP : Connection timed out\0");break;
	case WSAECONNREFUSED:     strcpy(Result," TCP : Connection refused\0");break;
	case WSAELOOP:            strcpy(Result," TCP : Too many levels of symbolic links\0");break;
	case WSAENAMETOOLONG:     strcpy(Result," TCP : File name is too long\0");break;
	case WSAEHOSTDOWN:        strcpy(Result," TCP : Host is down\0");break;
	case WSAEHOSTUNREACH:     strcpy(Result," TCP : Unreachable peer\0");break;
	case WSAENOTEMPTY:        strcpy(Result," TCP : Directory is not empty\0");break;
	case WSAEUSERS:           strcpy(Result," TCP : Too many users\0");break;
	case WSAEDQUOT:           strcpy(Result," TCP : Disk quota exceeded\0");break;
	case WSAESTALE:           strcpy(Result," TCP : Stale NFS file handle\0");break;
	case WSAEREMOTE:          strcpy(Result," TCP : Too many levels of remote in path\0");break;
	#ifdef OS_WINDOWS
	case WSAEPROCLIM:         strcpy(Result," TCP : Too many processes\0");break;
	case WSASYSNOTREADY:      strcpy(Result," TCP : Network subsystem is unusable\0");break;
	case WSAVERNOTSUPPORTED:  strcpy(Result," TCP : Winsock DLL cannot support this application\0");break;
	case WSANOTINITIALISED:   strcpy(Result," TCP : Winsock not initialized\0");break;
	case WSAEDISCON:          strcpy(Result," TCP : Disconnect\0");break;
	case WSAHOST_NOT_FOUND:   strcpy(Result," TCP : Host not found\0");break;
	case WSATRY_AGAIN:        strcpy(Result," TCP : Non authoritative - host not found\0");break;
	case WSANO_RECOVERY:      strcpy(Result," TCP : Non recoverable error\0");break;
	case WSANO_DATA:          strcpy(Result," TCP : Valid name, no data record of requested type\0");break;
	#endif
	case WSAEINVALIDADDRESS:  strcpy(Result," TCP : Invalid address\0");break;
	default:
		{
			char CNumber[16];
			strcpy(Result, " TCP : Other Socket error (");
			strcat(Result, IntToString(Error, CNumber));
			strcat(Result, ")");
			break;
		}
	}
	return Result;
}
//---------------------------------------------------------------------------
char* IsoTextOf(int Error, char* Result)
{
	switch (Error)
	{
		case 0 :                     *Result='\0';break;
		case errIsoConnect:          strcpy(Result," ISO : Connection error\0");break;
		case errIsoDisconnect:       strcpy(Result," ISO : Disconnect error\0");break;
		case errIsoInvalidPDU:       strcpy(Result," ISO : Bad PDU format\0");break;
		case errIsoInvalidDataSize:  strcpy(Result," ISO : Datasize passed to send/recv buffer is invalid\0");break;
		case errIsoNullPointer:      strcpy(Result," ISO : Null passed as pointer\0");break;
		case errIsoShortPacket:      strcpy(Result," ISO : A short packet received\0");break;
		case errIsoTooManyFragments: strcpy(Result," ISO : Too many packets without EoT flag\0");break;
		case errIsoPduOverflow:      strcpy(Result," ISO : The sum of fragments data exceded maximum packet size\0");break;
		case errIsoSendPacket:       strcpy(Result," ISO : An error occurred during send\0");break;
		case errIsoRecvPacket:       strcpy(Result," ISO : An error occurred during recv\0");break;
		case errIsoInvalidParams:    strcpy(Result," ISO : Invalid connection params (wrong TSAPs)\0");break;
		default:
		{
			char CNumber[16];
			strcpy(Result, " ISO : Unknown error (0x");
			strcat(Result, NumToString(Error, 16, 8, CNumber));
			strcat(Result, ")");
			break;
		}
	}
	return Result;
}
//---------------------------------------------------------------------------
char* CliTextOf(int Error, char* Result)
{
	switch (Error)
	{
	  case 0 :                            *Result='\0';break;
	  case errNegotiatingPDU            : strcpy(Result,"CPU : Error in PDU negotiation\0");break;
	  case errCliInvalidParams          : strcpy(Result,"CLI : invalid param(s) supplied\0");break;
	  case errCliJobPending             : strcpy(Result,"CLI : Job pending\0");break;
	  case errCliTooManyItems           : strcpy(Result,"CLI : too may items (>20) in multi read/write\0");break;
	  case errCliInvalidWordLen         : strcpy(Result,"CLI : invalid WordLength\0");break;
	  case errCliPartialDataWritten     : strcpy(Result,"CLI : Partial data written\0");break;
	  case errCliSizeOverPDU            : strcpy(Result,"CPU : total data exceeds the PDU size\0");break;
	  case errCliInvalidPlcAnswer       : strcpy(Result,"CLI : invalid CPU answer\0");break;
	  case errCliAddressOutOfRange      : strcpy(Result,"CPU : Address out of range\0");break;
	  case errCliInvalidTransportSize   : strcpy(Result,"CPU : Invalid Transport size\0");break;
	  case errCliWriteDataSizeMismatch  : strcpy(Result,"CPU : Data size mismatch\0");break;
	  case errCliItemNotAvailable       : strcpy(Result,"CPU : Item not available\0");break;
	  case errCliInvalidValue           : strcpy(Result,"CPU : Invalid value supplied\0");break;
	  case errCliCannotStartPLC         : strcpy(Result,"CPU : Cannot start PLC\0");break;
	  case errCliAlreadyRun             : strcpy(Result,"CPU : PLC already RUN\0");break;
	  case errCliCannotStopPLC          : strcpy(Result,"CPU : Cannot stop PLC\0");break;
	  case errCliCannotCopyRamToRom     : strcpy(Result,"CPU : Cannot copy RAM to ROM\0");break;
	  case errCliCannotCompress         : strcpy(Result,"CPU : Cannot compress\0");break;
	  case errCliAlreadyStop            : strcpy(Result,"CPU : PLC already STOP\0");break;
	  case errCliFunNotAvailable        : strcpy(Result,"CPU : Function not available\0");break;
	  case errCliUploadSequenceFailed   : strcpy(Result,"CPU : Upload sequence failed\0");break;
	  case errCliInvalidDataSizeRecvd   : strcpy(Result,"CLI : Invalid data size received\0");break;
	  case errCliInvalidBlockType       : strcpy(Result,"CLI : Invalid block type\0");break;
	  case errCliInvalidBlockNumber     : strcpy(Result,"CLI : Invalid block number\0");break;
	  case errCliInvalidBlockSize       : strcpy(Result,"CLI : Invalid block size\0");break;
	  case errCliDownloadSequenceFailed : strcpy(Result,"CPU : Download sequence failed\0");break;
	  case errCliInsertRefused          : strcpy(Result,"CPU : block insert refused\0");break;
	  case errCliDeleteRefused          : strcpy(Result,"CPU : block delete refused\0");break;
	  case errCliNeedPassword           : strcpy(Result,"CPU : Function not authorized for current protection level\0");break;
	  case errCliInvalidPassword        : strcpy(Result,"CPU : Invalid password\0");break;
	  case errCliNoPasswordToSetOrClear : strcpy(Result,"CPU : No password to set or clear\0");break;
	  case errCliJobTimeout             : strcpy(Result,"CLI : Job Timeout\0");break;
	  case errCliFunctionRefused        : strcpy(Result,"CLI : function refused by CPU (Unknown error)\0");break;
	  case errCliPartialDataRead        : strcpy(Result,"CLI : Partial data read\0");break;
	  case errCliBufferTooSmall         : strcpy(Result,"CLI : The buffer supplied is too small to accomplish the operation\0");break;
	  case errCliDestroying             : strcpy(Result,"CLI : Cannot perform (destroying)\0");break;
	  case errCliInvalidParamNumber     : strcpy(Result,"CLI : Invalid Param Number\0");break;
	  case errCliCannotChangeParam      : strcpy(Result,"CLI : Cannot change this param now\0");break;
	  default                           :
	  {
		  char CNumber[16];
		  strcpy(Result, "CLI : Unknown error (0x");
		  strcat(Result, NumToString(Error, 16, 8, CNumber));
		  strcat(Result, ")");
		  break;
	  }
	};
	return Result;
}
//---------------------------------------------------------------------------
char* SrvTextOf(int Error, char* Result)
{
	switch (Error)
	{
	case 0:                        *Result = '\0'; break;
	case errSrvCannotStart:        strcpy(Result, "SRV : Server cannot start\0"); break;
	case errSrvDBNullPointer:      strcpy(Result, "SRV : Null passed as area pointer\0"); break;
	case errSrvAreaAlreadyExists:  strcpy(Result, "SRV : Cannot register area since already exists\0"); break;
	case errSrvUnknownArea:        strcpy(Result, "SRV : Unknown Area code\0"); break;
	case errSrvInvalidParams:      strcpy(Result, "SRV : Invalid param(s) supplied\0"); break;
	case errSrvTooManyDB:          strcpy(Result, "SRV : DB Limit reached\0"); break;
	case errSrvInvalidParamNumber: strcpy(Result, "SRV : Invalid Param Number\0"); break;
	case errSrvCannotChangeParam:  strcpy(Result, "SRV : Cannot change this param now\0");break;
	default: 
		{
			char CNumber[16];
			strcpy(Result, "SRV : Unknown error (0x");
			strcat(Result, NumToString(Error, 16, 8, CNumber));
			strcat(Result, ")");
			break;
		}
	};
	return Result;
}
//---------------------------------------------------------------------------
char* ParTextOf(int Error, char* Result)
{
	switch(Error)
	{
		case 0: *Result = '\0'; break;
		case errParAddressInUse      : strcpy(Result, "PAR : Local address already in use");break;
		case errParNoRoom            : strcpy(Result, "PAR : No more partners available");break;
		case errServerNoRoom         : strcpy(Result, "PAR : No more servers available");break;
		case errParInvalidParams     : strcpy(Result, "PAR : Invalid parameter supplied");break;
		case errParNotLinked         : strcpy(Result, "PAR : Cannot perform, Partner not linked");break;
		case errParBusy              : strcpy(Result, "PAR : Cannot perform, Partner Busy");break;
		case errParFrameTimeout      : strcpy(Result, "PAR : Frame timeout");break;
		case errParInvalidPDU        : strcpy(Result, "PAR : Invalid PDU received");break;
		case errParSendTimeout       : strcpy(Result, "PAR : Send timeout");break;
		case errParRecvTimeout       : strcpy(Result, "PAR : Recv timeout");break;
		case errParSendRefused       : strcpy(Result, "PAR : Send refused by peer");break;
		case errParNegotiatingPDU    : strcpy(Result, "PAR : Error negotiating PDU");break;
		case errParSendingBlock      : strcpy(Result, "PAR : Error Sending Block");break;
		case errParRecvingBlock      : strcpy(Result, "PAR : Error Receiving Block");break;
		case errParBindError         : strcpy(Result, "PAR : Error Binding");break;
		case errParDestroying        : strcpy(Result, "PAR : Cannot perform (destroying)");break;
		case errParInvalidParamNumber: strcpy(Result, "PAR : Invalid Param Number");break;
		case errParCannotChangeParam : strcpy(Result, "PAR : Cannot change this param now");break;
		case errParBufferTooSmall    : strcpy(Result, "PAR : The buffer supplied is too small to accomplish the operation");break;
		default:
		{
			char CNumber[16];
			strcpy(Result, "PAR : Unknown error (0x");
			strcat(Result, NumToString(Error, 16, 8, CNumber));
			strcat(Result, ")");
			break;
		}
	}
	return Result;
}
//---------------------------------------------------------------------------
char* ErrCliText(int Error, char * Result, int TextLen)
{
	char TcpError[128];
	char IsoError[128];
	char CliError[256];
	if (Error != 0)
	{
		switch (Error)
		{
			case errLibInvalidParam  : strncpy(Result,"LIB : Invalid param supplied\0",TextLen);break;
			case errLibInvalidObject: strncpy(Result, "LIB : Invalid object supplied\0", TextLen); break;
			default :
			{
				CliTextOf(Error & ErrS7Mask, CliError);
				strcat(CliError, IsoTextOf(Error & ErrIsoMask, IsoError));
				strcat(CliError, TcpTextOf(Error & ErrTcpMask, TcpError));
				strncpy(Result, CliError, TextLen);
			}
		}
	}
	else
		strncpy(Result, "OK\0", TextLen);
	return Result;
}
//---------------------------------------------------------------------------
char* ErrSrvText(int Error, char* Result, int TextLen)
{
	char TcpError[128];
	char IsoError[128];
	char SrvError[256];
	if (Error != 0)
	{
		switch (Error)
		{
		case errLibInvalidParam: strncpy(Result, "LIB : Invalid param supplied\0", TextLen); break;
		case errLibInvalidObject: strncpy(Result, "LIB : Invalid object supplied\0", TextLen); break;
		default:
		{
			SrvTextOf(Error & ErrS7Mask, SrvError);
			strcat(SrvError, IsoTextOf(Error & ErrIsoMask, IsoError));
			strcat(SrvError, TcpTextOf(Error & ErrTcpMask, TcpError));
			strncpy(Result, SrvError, TextLen);
		}
		}
	}
	else
		strncpy(Result, "OK\0", TextLen);
	return Result;
}
//---------------------------------------------------------------------------
char* ErrParText(int Error, char* Result, int TextLen)
{
	char TcpError[128];
	char IsoError[128];
	char ParError[256];
	if (Error != 0)
	{
		switch (Error)
		{
		case errLibInvalidParam: strncpy(Result, "LIB : Invalid param supplied\0", TextLen); break;
		case errLibInvalidObject: strncpy(Result, "LIB : Invalid object supplied\0", TextLen); break;
		default:
		{
			ParTextOf(Error & ErrS7Mask, ParError);
			strcat(ParError, IsoTextOf(Error & ErrIsoMask, IsoError));
			strcat(ParError, TcpTextOf(Error & ErrTcpMask, TcpError));
			strncpy(Result, ParError, TextLen);
		}
		}
	}
	else
		strncpy(Result, "OK\0", TextLen);
	return Result;
}
//---------------------------------------------------------------------------
//                               SERVER EVENTS TEXT
//---------------------------------------------------------------------------
char* SenderText(TSrvEvent &Event, char* Result)
{
	char Buf[64];
	char Add[16];
	TimeToString(Event.EvtTime, Buf);
	if (Event.EvtSender != 0)
	{
		strcat(Buf, " [");
		strcat(Buf, IpAddressToString(Event.EvtSender, Add));
		strcat(Buf, "] ");
	}
	else
		strcat(Buf, " Server ");
	strcpy(Result, Buf);
	return Result;
}
//---------------------------------------------------------------------------
char* TcpServerEventText(TSrvEvent &Event, char* Result)
{
    char S[256];
	char Buf[128];
	
	strcpy(S, SenderText(Event, Buf));

	switch (Event.EvtCode)
    {
      case evcServerStarted       : strcat(S,"started");break;
      case evcServerStopped       : strcat(S,"stopped");break;
	  case evcListenerCannotStart:
		  strcat(S, "Cannot start listener - Socket Error : ");
		  strcat(S, TcpTextOf(Event.EvtRetCode,Buf));  
		  break;
      case evcClientAdded         : strcat(S,"Client added");break;
      case evcClientRejected      : strcat(S,"Client refused");break;
      case evcClientNoRoom        : strcat(S,"A client was refused due to maximum connections number");break;
      case evcClientException     : strcat(S,"Client exception");break;
      case evcClientDisconnected  : strcat(S,"Client disconnected by peer");break;
      case evcClientTerminated    : strcat(S,"Client terminated");break;
	  case evcClientsDropped:
		  strcat(S, IntToString(Event.EvtParam1, Buf));
		  strcat(S, " clients have been dropped bacause unresponsive");
		  break;
	  default:
		  strcat(S, "Unknown event (");
		  strcat(S, IntToString(Event.EvtCode, Buf));
		  strcat(S,")"); 
		  break;
    };
	strcpy(Result, S);
	return Result;
}
//---------------------------------------------------------------------------
char* PDUText(TSrvEvent &Event, char* Result)
{
	char S[256];
	char Buf[128];
	switch (Event.EvtRetCode)
	{
		case evrFragmentRejected:
			strcpy(S, "Fragment of ");
			strcat(S, IntToString(Event.EvtParam1, Buf));
			strcat(S, " bytes rejected");
			break;
		case evrMalformedPDU:
			strcpy(S, "Malformed PDU of ");
			strcat(S, IntToString(Event.EvtParam1, Buf));
			strcat(S, " bytes rejected");
			break;
		case evrSparseBytes:
			strcpy(S, "Message of sparse ");
			strcat(S, IntToString(Event.EvtParam1, Buf));
			strcat(S, " bytes rejected");
			break;
		case evrCannotHandlePDU:
			strcpy(S, "Cannot handle this PDU");
			break;
		case evrNotImplemented:
			switch (Event.EvtParam1)
			{
				case grCyclicData:
					strcpy(S, "Function group cyclic data not yet implemented");
					break;
				case grProgrammer:
					strcpy(S, "Function group programmer not yet implemented");
					break;
			}
			break;
		default:
			strcpy(S, "Unknown Return code (");
			strcat(S, IntToString(Event.EvtRetCode, Buf));
			strcat(S, ")");
			break;
	}
	strcpy(Result, S);
	return Result;
}
//---------------------------------------------------------------------------
char* TxtArea(TSrvEvent &Event, char* Result)
{
	char S[64];
	char Buf[32];
	switch (Event.EvtParam1)
    {
		case S7AreaPE: strcpy(S, "Area : PE, "); break;
		case S7AreaPA: strcpy(S, "Area : PA, "); break;
		case S7AreaMK: strcpy(S, "Area : MK, "); break;
		case S7AreaCT: strcpy(S, "Area : CT, "); break;
		case S7AreaTM: strcpy(S, "Area : TM, "); break;
		case S7AreaDB: 
			strcpy(S, "Area : DB");
			strcat(S, IntToString(Event.EvtParam2, Buf));
			strcat(S,", "); 
			break;
		default: 
			strcpy(S, "Unknown area (");
			strcat(S, IntToString(Event.EvtParam2, Buf));
			strcat(S,")");
			break;
    }
	strcpy(Result, S);
	return Result;
}
//---------------------------------------------------------------------------
char* TxtStartSize(TSrvEvent &Event, char* Result)
{
	char N[32];
	strcpy(Result, "Start : ");
	strcat(Result, IntToString(Event.EvtParam3, N));
	strcat(Result, ", Size : ");
	strcat(Result, IntToString(Event.EvtParam4, N));
	return Result;
}
//---------------------------------------------------------------------------
char* TxtDataResult(TSrvEvent &Event, char* Result)
{
	char N[32];
	switch (Event.EvtRetCode)
    {
        case evrNoError:
			strcpy(Result," --> OK");
			break;
		case evrErrException: 
			strcpy(Result, " --> Exception error");
			break;
		case evrErrAreaNotFound: 
			strcpy(Result, " --> Area not found");
			break;
        case evrErrOutOfRange: 
			strcpy(Result, " --> Out of range");
			break;
        case evrErrOverPDU:
			strcpy(Result, " --> Data size exceeds PDU size");
			break;
		case evrErrTransportSize: 
			strcpy(Result, " --> Invalid transport size");
			break;
		case evrDataSizeMismatch: 
			strcpy(Result, " --> Data size mismatch");
			break;
		default: 
			strcpy(Result, " --> Unknown error code (");
			strcat(Result, IntToString(Event.EvtRetCode, N));
			strcat(Result,")");
			break;
    };
	return Result;
}
//---------------------------------------------------------------------------
char* ControlText(word Code, char* Result)
{
	char N[64];
	strcpy(Result, "CPU Control request : ");
    switch (Code)
    {
        case CodeControlUnknown: 
			strcat(Result,"Unknown");
			break;
		case CodeControlColdStart: 
			strcat(Result, "Cold START --> OK");
			break;
		case CodeControlWarmStart: 
			strcat(Result, "Warm START --> OK");
			break;
		case CodeControlStop: 
			strcat(Result, "STOP --> OK");
			break;
		case CodeControlCompress: 
			strcat(Result, "Memory compress --> OK");
			break;
		case CodeControlCpyRamRom: 
			strcat(Result, "Copy Ram to Rom --> OK");
			break;
		case CodeControlInsDel: 
			strcat(Result, "Block Insert or Delete --> OK");
			break;
        default : 
			strcat(Result, "Unknown control code (");
			strcat(Result, IntToString(Code, N));
			strcat(Result,")");
    }
	return Result;
}
//---------------------------------------------------------------------------
char* ClockText(word Code, char* Result)
{
    if (Code==evsGetClock)
        strcpy(Result,"System clock read requested");
    else
		strcpy(Result, "System clock write requested");
	return Result;
}
//---------------------------------------------------------------------------
char* ReadSZLText(TSrvEvent &Event, char* Result)
{
	char S[128];
	char N[64];
	strcpy(S, "Read SZL request, ID:0x");
	strcat(S, NumToString(Event.EvtParam1, 16, 4, N));
	strcat(S, " INDEX:0x");
	strcat(S, NumToString(Event.EvtParam2, 16, 4, N));
	
	if (Event.EvtRetCode == evrNoError)
		strcat(S, " --> OK");
    else
		strcat(S, " --> NOT AVAILABLE");
	strcpy(Result, S);
	return Result;
}
//---------------------------------------------------------------------------
char* UploadText(TSrvEvent &Event, char* Result)
{
   strcpy(Result,"Block upload requested --> NOT PERFORMED (due to invalid security level)");
   return Result;
}
//---------------------------------------------------------------------------
char* DownloadText(TSrvEvent &Event, char* Result)
{
	strcpy(Result, "Block download requested --> NOT PERFORMED (due to invalid security level)");
	return Result;
}
//---------------------------------------------------------------------------
char* StrBlockType(word Code, char* Result)
{
	char N[64];
	switch (Code)
    {
		case Block_OB: 
			strcpy(Result, "OB");
			break;
		case Block_DB: 
			strcpy(Result, "DB");
			break;
		case Block_SDB: 
			strcpy(Result, "SDB");
			break;
		case Block_FC: 
			strcpy(Result, "FC");
			break;
		case Block_SFC: 
			strcpy(Result, "SFC");
			break;
		case Block_FB: 
			strcpy(Result, "FB");
			break;
		case Block_SFB: 
			strcpy(Result, "SFB");
			break;
		default: 
			strcpy(Result, "[Unknown 0x");
			strcat(Result, NumToString(Code, 16, 4, N));
			strcat(Result,"]");
			break;
    };
	return Result;
}
//---------------------------------------------------------------------------
char* BlockInfoText(TSrvEvent &Event, char* Result)
{
	char S[64];
	switch (Event.EvtParam1)
    {
		case evsGetBlockList: 
			strcpy(Result, "Block list requested");
			break;
		case evsStartListBoT:
			strcpy(Result, "Block of type ");
			strcat(Result, StrBlockType(Event.EvtParam2,S));
			strcat(Result, " list requested (start sequence)");
			break;
		case evsListBoT: 
			strcpy(Result, "Block of type ");
			strcat(Result, StrBlockType(Event.EvtParam2, S));
			strcat(Result, " list requested (next part)");
			break;
		case evsGetBlockInfo: 
			strcpy(Result, "Block info requested ");
			strcat(Result, StrBlockType(Event.EvtParam2, S));
			strcat(Result, " ");
			strcat(Result, IntToString(Event.EvtParam3,S));
			break;
    };
    if (Event.EvtRetCode == evrNoError)
		strcat(Result, " --> OK");
    else
		strcat(Result, " --> NOT AVAILABLE");
	return Result;
}
//---------------------------------------------------------------------------
char* SecurityText(TSrvEvent &Event, char* Result)
{
    switch (Event.EvtParam1)
    {
        case evsSetPassword:
			strcpy(Result,"Security request : Set session password --> OK");
			break;
		case evsClrPassword: 
			strcpy(Result, "Security request : Clear session password --> OK");
			break;
		default: 
			strcpy(Result, "Security request : Unknown Subfunction");
			break;
    };
	return Result;
}
//---------------------------------------------------------------------------
char* EvtSrvText(TSrvEvent &Event, char* Result, int TextLen)
{
	char S[256];
	char C[128];

	if (Event.EvtCode > evcSnap7Base)
    {
		strcpy(S, SenderText(Event, C));
		switch (Event.EvtCode)
        {
            case evcPDUincoming:
				strcat(S, "PDU incoming : ");
				strcat(S,PDUText(Event,C));
				break;
			case evcDataRead: 
				strcat(S, "Read request, ");
				strcat(S, TxtArea(Event, C));
				strcat(S, TxtStartSize(Event, C));
				strcat(S, TxtDataResult(Event, C));
				break;
			case evcDataWrite: 
				strcat(S, "Write request, ");
				strcat(S, TxtArea(Event, C));
				strcat(S, TxtStartSize(Event, C));
				strcat(S, TxtDataResult(Event, C));
				break;
			case evcNegotiatePDU:
				strcat(S, "The client requires a PDU size of ");
				strcat(S, IntToString(Event.EvtParam1, C));
				strcat(S," bytes");
				break;
			case evcControl: 
				strcat(S, ControlText(Event.EvtParam1,C));
				break;
			case evcReadSZL: 
				strcat(S, ReadSZLText(Event,C));
				break;
			case evcClock: 
				strcat(S, ClockText(Event.EvtParam1,C));
				break;
			case evcUpload: 
				strcat(S, UploadText(Event,C));
				break;
			case evcDownload: 
				strcat(S, DownloadText(Event,C));
				break;
			case evcDirectory: 
				strcat(S, BlockInfoText(Event,C));
				break;
			case evcSecurity: 
				strcat(S, SecurityText(Event,C));
				break;
			default:
				strcat(S, "Unknown event (");
				strcat(S, IntToString(Event.EvtCode, C));
				strcat(S,")");
				break;
        }
    }
    else
        strcpy(S,TcpServerEventText(Event,C));

	strncpy(Result, S, TextLen);
	return Result;
}

