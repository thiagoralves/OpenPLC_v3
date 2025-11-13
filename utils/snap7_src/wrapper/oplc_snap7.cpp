/*=============================================================================|
|  PROJECT SNAP7                                                         1.4.3 |
|==============================================================================|
|  Copyright (C) 2013, 2025 Davide Nardella                                    |
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
|  This file is a modified wrapper which contains OpenPLC interface            |
|                                                                              |
|=============================================================================*/
#include <stdio.h>
#include "oplc_snap7.h"
#include "ladder.h"

//==============================================================================
// CLIENT
//==============================================================================
TS7Client::TS7Client()
{
    Client=Cli_Create();
}
//---------------------------------------------------------------------------
TS7Client::~TS7Client()
{
    Cli_Destroy(&Client);
}
//---------------------------------------------------------------------------
int TS7Client::Connect()
{
    return Cli_Connect(Client);
}
//---------------------------------------------------------------------------
int TS7Client::ConnectTo(const char *RemAddress, int Rack, int Slot)
{
    return Cli_ConnectTo(Client, RemAddress, Rack, Slot);
}
//---------------------------------------------------------------------------
int TS7Client::SetConnectionParams(const char *RemAddress, word LocalTSAP, word RemoteTSAP)
{
    return Cli_SetConnectionParams(Client, RemAddress, LocalTSAP, RemoteTSAP);
}
//---------------------------------------------------------------------------
int TS7Client::SetConnectionType(word ConnectionType)
{
    return Cli_SetConnectionType(Client, ConnectionType);
}
//---------------------------------------------------------------------------
int TS7Client::Disconnect()
{
    return Cli_Disconnect(Client);
}
//---------------------------------------------------------------------------
int TS7Client::GetParam(int ParamNumber, void *pValue)
{
    return Cli_GetParam(Client, ParamNumber, pValue);
}
//---------------------------------------------------------------------------
int TS7Client::SetParam(int ParamNumber, void *pValue)
{
    return Cli_SetParam(Client, ParamNumber, pValue);
}
//---------------------------------------------------------------------------
int TS7Client::ReadArea(int Area, int DBNumber, int Start, int Amount, int WordLen, void *pUsrData)
{
    return Cli_ReadArea(Client, Area, DBNumber, Start, Amount, WordLen, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::WriteArea(int Area, int DBNumber, int Start, int Amount, int WordLen, void *pUsrData)
{
    return Cli_WriteArea(Client, Area, DBNumber, Start, Amount, WordLen, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::ReadMultiVars(PS7DataItem Item, int ItemsCount)
{
    return Cli_ReadMultiVars(Client, Item, ItemsCount);
}
//---------------------------------------------------------------------------
int TS7Client::WriteMultiVars(PS7DataItem Item, int ItemsCount)
{
    return Cli_WriteMultiVars(Client, Item, ItemsCount);
}
//---------------------------------------------------------------------------
int TS7Client::DBRead(int DBNumber, int Start, int Size, void *pUsrData)
{
    return Cli_DBRead(Client, DBNumber, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::DBWrite(int DBNumber, int Start, int Size, void *pUsrData)
{
    return Cli_DBWrite(Client, DBNumber, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::MBRead(int Start, int Size, void *pUsrData)
{
    return Cli_MBRead(Client, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::MBWrite(int Start, int Size, void *pUsrData)
{
    return Cli_MBWrite(Client, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::EBRead(int Start, int Size, void *pUsrData)
{
    return Cli_EBRead(Client, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::EBWrite(int Start, int Size, void *pUsrData)
{
    return Cli_EBWrite(Client, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::ABRead(int Start, int Size, void *pUsrData)
{
    return Cli_ABRead(Client, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::ABWrite(int Start, int Size, void *pUsrData)
{
    return Cli_ABWrite(Client, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::TMRead(int Start, int Amount, void *pUsrData)
{
    return Cli_TMRead(Client, Start, Amount, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::TMWrite(int Start, int Amount, void *pUsrData)
{
    return Cli_TMWrite(Client, Start, Amount, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::CTRead(int Start, int Amount, void *pUsrData)
{
    return Cli_CTRead(Client, Start, Amount, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::CTWrite(int Start, int Amount, void *pUsrData)
{
    return Cli_CTWrite(Client, Start, Amount, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::ListBlocks(PS7BlocksList pUsrData)
{
    return Cli_ListBlocks(Client, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::GetAgBlockInfo(int BlockType, int BlockNum, PS7BlockInfo pUsrData)
{
    return Cli_GetAgBlockInfo(Client, BlockType, BlockNum, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::GetPgBlockInfo(void *pBlock, PS7BlockInfo pUsrData, int Size)
{
    return Cli_GetPgBlockInfo(Client, pBlock, pUsrData, Size);
}
//---------------------------------------------------------------------------
int TS7Client::ListBlocksOfType(int BlockType, TS7BlocksOfType *pUsrData, int *ItemsCount)
{
    return Cli_ListBlocksOfType(Client, BlockType, pUsrData, ItemsCount);
}
//---------------------------------------------------------------------------
int TS7Client::Upload(int BlockType, int BlockNum, void *pUsrData, int *Size)
{
    return Cli_Upload(Client, BlockType, BlockNum, pUsrData, Size);
}
//---------------------------------------------------------------------------
int TS7Client::FullUpload(int BlockType, int BlockNum, void *pUsrData, int *Size)
{
    return Cli_FullUpload(Client, BlockType, BlockNum, pUsrData, Size);
}
//---------------------------------------------------------------------------
int TS7Client::Download(int BlockNum, void *pUsrData, int Size)
{
    return Cli_Download(Client, BlockNum, pUsrData, Size);
}
//---------------------------------------------------------------------------
int TS7Client::Delete(int BlockType, int BlockNum)
{
    return Cli_Delete(Client, BlockType, BlockNum);
}
//---------------------------------------------------------------------------
int TS7Client::DBGet(int DBNumber, void *pUsrData, int *Size)
{
    return Cli_DBGet(Client, DBNumber, pUsrData, Size);
}
//---------------------------------------------------------------------------
int TS7Client::DBFill(int DBNumber, int FillChar)
{
    return Cli_DBFill(Client, DBNumber, FillChar);
}
//---------------------------------------------------------------------------
int TS7Client::GetPlcDateTime(s7tm *DateTime)
{
    return Cli_GetPlcDateTime(Client, DateTime);
}
//---------------------------------------------------------------------------
int TS7Client::SetPlcDateTime(s7tm *DateTime)
{
    return Cli_SetPlcDateTime(Client, DateTime);
}
//---------------------------------------------------------------------------
int TS7Client::SetPlcSystemDateTime()
{
    return Cli_SetPlcSystemDateTime(Client);
}
//---------------------------------------------------------------------------
int TS7Client::GetOrderCode(PS7OrderCode pUsrData)
{
    return Cli_GetOrderCode(Client, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::GetCpuInfo(PS7CpuInfo pUsrData)
{
    return Cli_GetCpuInfo(Client, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::GetCpInfo(PS7CpInfo pUsrData)
{
    return Cli_GetCpInfo(Client, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::ReadSZL(int ID, int Index, PS7SZL pUsrData, int *Size)
{
    return Cli_ReadSZL(Client, ID, Index, pUsrData, Size);
}
//---------------------------------------------------------------------------
int TS7Client::ReadSZLList(PS7SZLList pUsrData, int *ItemsCount)
{
    return Cli_ReadSZLList(Client, pUsrData, ItemsCount);
}
//---------------------------------------------------------------------------
int TS7Client::PlcHotStart()
{
    return Cli_PlcHotStart(Client);
}
//---------------------------------------------------------------------------
int TS7Client::PlcColdStart()
{
    return Cli_PlcColdStart(Client);
}
//---------------------------------------------------------------------------
int TS7Client::PlcStop()
{
    return Cli_PlcStop(Client);
}
//---------------------------------------------------------------------------
int TS7Client::CopyRamToRom(int Timeout)
{
    return Cli_CopyRamToRom(Client, Timeout);
}
//---------------------------------------------------------------------------
int TS7Client::Compress(int Timeout)
{
    return Cli_Compress(Client, Timeout);
}
//---------------------------------------------------------------------------
int TS7Client::GetProtection(PS7Protection pUsrData)
{
    return Cli_GetProtection(Client, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::SetSessionPassword(char *Password)
{
    return Cli_SetSessionPassword(Client, Password);
}
//---------------------------------------------------------------------------
int TS7Client::ClearSessionPassword()
{
    return Cli_ClearSessionPassword(Client);
}
//---------------------------------------------------------------------------
int TS7Client::ExecTime()
{
    int Time;
    int Result = Cli_GetExecTime(Client, &Time);
    if (Result==0)
        return Time;
    else
        return Result;
}
//---------------------------------------------------------------------------
int TS7Client::LastError()
{
    int LastError;
    int Result =Cli_GetLastError(Client, &LastError);
    if (Result==0)
       return LastError;
    else
       return Result;
}
//---------------------------------------------------------------------------
int TS7Client::PDULength()
{
    int Requested, Negotiated;
    if (Cli_GetPduLength(Client, &Requested, &Negotiated)==0)
        return Negotiated;
    else
        return 0;
}
//---------------------------------------------------------------------------
int TS7Client::PDURequested()
{
    int Requested, Negotiated;
    if (Cli_GetPduLength(Client, &Requested, &Negotiated)==0)
        return Requested;
    else
        return 0;
}
//---------------------------------------------------------------------------
int TS7Client::PlcStatus()
{
    int Status;
    int Result = Cli_GetPlcStatus(Client, &Status);
    if (Result==0)
        return Status;
    else
        return Result;
}
//---------------------------------------------------------------------------
bool TS7Client::Connected()
{
	int ClientStatus;
	if (Cli_GetConnected(Client ,&ClientStatus)==0)
		return ClientStatus!=0;
	else
		return false;
}
//---------------------------------------------------------------------------
int TS7Client::SetAsCallback(pfn_CliCompletion pCompletion, void *usrPtr)
{
    return Cli_SetAsCallback(Client, pCompletion, usrPtr);
}
//---------------------------------------------------------------------------
bool TS7Client::CheckAsCompletion(int *opResult)
{
	return Cli_CheckAsCompletion(Client ,opResult)==JobComplete;
}
//---------------------------------------------------------------------------
int TS7Client::WaitAsCompletion(longword Timeout)
{
    return Cli_WaitAsCompletion(Client, Timeout);
}
//---------------------------------------------------------------------------
int TS7Client::AsReadArea(int Area, int DBNumber, int Start, int Amount, int WordLen, void *pUsrData)
{
    return Cli_AsReadArea(Client, Area, DBNumber, Start, Amount, WordLen, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsWriteArea(int Area, int DBNumber, int Start, int Amount, int WordLen, void *pUsrData)
{
    return Cli_AsWriteArea(Client, Area, DBNumber, Start, Amount, WordLen, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsListBlocksOfType(int BlockType,  PS7BlocksOfType pUsrData, int *ItemsCount)
{
    return Cli_AsListBlocksOfType(Client, BlockType,  pUsrData, ItemsCount);
}
//---------------------------------------------------------------------------
int TS7Client::AsReadSZL(int ID, int Index,  PS7SZL pUsrData, int *Size)
{
    return Cli_AsReadSZL(Client, ID, Index, pUsrData, Size);
}
//---------------------------------------------------------------------------
int TS7Client::AsReadSZLList(PS7SZLList pUsrData, int *ItemsCount)
{
    return Cli_AsReadSZLList(Client, pUsrData, ItemsCount);
}
//---------------------------------------------------------------------------
int TS7Client::AsUpload(int BlockType, int BlockNum, void *pUsrData, int *Size)
{
    return Cli_AsUpload(Client, BlockType, BlockNum, pUsrData, Size);
}
//---------------------------------------------------------------------------
int TS7Client::AsFullUpload(int BlockType, int BlockNum, void *pUsrData, int *Size)
{
    return Cli_AsFullUpload(Client, BlockType, BlockNum, pUsrData, Size);
}
//---------------------------------------------------------------------------
int TS7Client::AsDownload(int BlockNum, void *pUsrData, int Size)
{
    return Cli_AsDownload(Client, BlockNum, pUsrData, Size);
}
//---------------------------------------------------------------------------
int TS7Client::AsCopyRamToRom(int Timeout)
{
    return Cli_AsCopyRamToRom(Client, Timeout);
}
//---------------------------------------------------------------------------
int TS7Client::AsCompress(int Timeout)
{
    return Cli_AsCompress(Client, Timeout);
}
//---------------------------------------------------------------------------
int TS7Client::AsDBRead(int DBNumber, int Start, int Size, void *pUsrData)
{
    return Cli_AsDBRead(Client, DBNumber, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsDBWrite(int DBNumber, int Start, int Size, void *pUsrData)
{
    return Cli_AsDBWrite(Client, DBNumber, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsMBRead(int Start, int Size, void *pUsrData)
{
    return Cli_AsMBRead(Client, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsMBWrite(int Start, int Size, void *pUsrData)
{
    return Cli_AsMBWrite(Client, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsEBRead(int Start, int Size, void *pUsrData)
{
    return Cli_AsEBRead(Client, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsEBWrite(int Start, int Size, void *pUsrData)
{
    return Cli_AsEBWrite(Client, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsABRead(int Start, int Size, void *pUsrData)
{
    return Cli_AsABRead(Client, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsABWrite(int Start, int Size, void *pUsrData)
{
    return Cli_AsABWrite(Client, Start, Size, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsTMRead(int Start, int Amount, void *pUsrData)
{
    return Cli_AsTMRead(Client, Start, Amount, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsTMWrite(int Start, int Amount, void *pUsrData)
{
    return Cli_AsTMWrite(Client, Start, Amount, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsCTRead(int Start, int Amount, void *pUsrData)
{
    return Cli_AsCTRead(Client, Start, Amount, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsCTWrite(int Start, int Amount, void *pUsrData)
{
    return Cli_AsCTWrite(Client, Start, Amount, pUsrData);
}
//---------------------------------------------------------------------------
int TS7Client::AsDBGet(int DBNumber, void *pUsrData, int *Size)
{
    return Cli_AsDBGet(Client, DBNumber, pUsrData, Size);
}
//---------------------------------------------------------------------------
int TS7Client::AsDBFill(int DBNumber, int FillChar)
{
    return Cli_AsDBFill(Client, DBNumber, FillChar);
}
//==============================================================================
// SERVER
//==============================================================================
TS7Server::TS7Server()
{
    Server=Srv_Create();
}
//---------------------------------------------------------------------------
TS7Server::~TS7Server()
{
    Srv_Destroy(&Server);
}
//---------------------------------------------------------------------------
int TS7Server::Start()
{
    return Srv_Start(Server);
}
//---------------------------------------------------------------------------
int TS7Server::StartTo(const char *Address)
{
    return Srv_StartTo(Server, Address);
}
//---------------------------------------------------------------------------
int TS7Server::Stop()
{
    return Srv_Stop(Server);
}
//---------------------------------------------------------------------------
int TS7Server::GetParam(int ParamNumber, void *pValue)
{
    return Srv_GetParam(Server, ParamNumber, pValue);
}
//---------------------------------------------------------------------------
int TS7Server::SetParam(int ParamNumber, void *pValue)
{
    return Srv_SetParam(Server, ParamNumber, pValue);
}
//---------------------------------------------------------------------------
int TS7Server::SetEventsCallback(pfn_SrvCallBack PCallBack, void *UsrPtr)
{
    return Srv_SetEventsCallback(Server, PCallBack, UsrPtr);
}
//---------------------------------------------------------------------------
int TS7Server::SetReadEventsCallback(pfn_SrvCallBack PCallBack, void *UsrPtr)
{
	return Srv_SetReadEventsCallback(Server, PCallBack, UsrPtr);
}
//---------------------------------------------------------------------------
int TS7Server::SetRWAreaCallback(pfn_RWAreaCallBack PCallBack, void *UsrPtr)
{
	return Srv_SetRWAreaCallback(Server, PCallBack, UsrPtr);
}
//---------------------------------------------------------------------------
bool TS7Server::PickEvent(TSrvEvent *pEvent)
{
    int EvtReady;
    if (Srv_PickEvent(Server, pEvent, &EvtReady)==0)
       return EvtReady!=0;
    else
       return false;
}
//---------------------------------------------------------------------------
void TS7Server::ClearEvents()
{
    Srv_ClearEvents(Server);
}
//---------------------------------------------------------------------------
longword TS7Server::GetEventsMask()
{
    longword Mask;
    int Result = Srv_GetMask(Server, mkEvent, &Mask);
    if (Result==0)
        return Mask;
    else
        return 0;
}
//---------------------------------------------------------------------------
longword TS7Server::GetLogMask()
{
    longword Mask;
    int Result = Srv_GetMask(Server, mkLog, &Mask);
    if (Result==0)
        return Mask;
    else
        return 0;
}
//---------------------------------------------------------------------------
void TS7Server::SetEventsMask(longword Mask)
{
    Srv_SetMask(Server, mkEvent, Mask);
}
//---------------------------------------------------------------------------
void TS7Server::SetLogMask(longword Mask)
{
    Srv_SetMask(Server, mkLog, Mask);
}
//---------------------------------------------------------------------------
int TS7Server::RegisterArea(int AreaCode, word Index, void *pUsrData, word Size)
{
    return Srv_RegisterArea(Server, AreaCode, Index, pUsrData, Size);
}
//---------------------------------------------------------------------------
int TS7Server::UnregisterArea(int AreaCode, word Index)
{
    return Srv_UnregisterArea(Server, AreaCode, Index);
}
//---------------------------------------------------------------------------
int TS7Server::LockArea(int AreaCode, word Index)
{
    return Srv_LockArea(Server, AreaCode, Index);
}
//---------------------------------------------------------------------------
int TS7Server::UnlockArea(int AreaCode, word Index)
{
    return Srv_UnlockArea(Server, AreaCode, Index);
}
//---------------------------------------------------------------------------
int TS7Server::ServerStatus()
{
    int ServerStatus, CpuStatus, ClientsCount;
    int Result =Srv_GetStatus(Server, &ServerStatus, &CpuStatus, &ClientsCount);
    if (Result==0)
        return ServerStatus;
    else
        return Result;
}
//---------------------------------------------------------------------------
int TS7Server::GetCpuStatus()
{
    int ServerStatus, CpuStatus, ClientsCount;
    int Result =Srv_GetStatus(Server, &ServerStatus, &CpuStatus, &ClientsCount);
    if (Result==0)
            return CpuStatus;
    else
            return Result;
}
//---------------------------------------------------------------------------
int TS7Server::ClientsCount()
{
    int ServerStatus, CpuStatus, ClientsCount;
    int Result =Srv_GetStatus(Server, &ServerStatus, &CpuStatus, &ClientsCount);
    if (Result==0)
        return ClientsCount;
    else
        return Result;
}
//---------------------------------------------------------------------------
int TS7Server::SetCpuStatus(int Status)
{
    return Srv_SetCpuStatus(Server, Status);
}
//==============================================================================
// PARTNER
//==============================================================================
TS7Partner::TS7Partner(bool Active)
{
    Partner=Par_Create(int(Active));
}
//---------------------------------------------------------------------------
TS7Partner::~TS7Partner()
{
    Par_Destroy(&Partner);
}
//---------------------------------------------------------------------------
int TS7Partner::GetParam(int ParamNumber, void *pValue)
{
    return Par_GetParam(Partner, ParamNumber, pValue);
}
//---------------------------------------------------------------------------
int TS7Partner::SetParam(int ParamNumber, void *pValue)
{
    return Par_SetParam(Partner, ParamNumber, pValue);
}
//---------------------------------------------------------------------------
int TS7Partner::Start()
{
    return Par_Start(Partner);
}
//---------------------------------------------------------------------------
int TS7Partner::StartTo(const char *LocalAddress, const char *RemoteAddress, int LocalTSAP, int RemoteTSAP)
{
    return Par_StartTo(Partner, LocalAddress, RemoteAddress, LocalTSAP, RemoteTSAP);
}
//---------------------------------------------------------------------------
int TS7Partner::Stop()
{
    return Par_Stop(Partner);
}
//---------------------------------------------------------------------------
int TS7Partner::BSend(longword R_ID, void *pUsrData, int Size)
{
    return Par_BSend(Partner, R_ID, pUsrData, Size);
}
//---------------------------------------------------------------------------
int TS7Partner::AsBSend(longword R_ID, void *pUsrData, int Size)
{
    return Par_AsBSend(Partner, R_ID, pUsrData, Size);
}
//---------------------------------------------------------------------------
bool TS7Partner::CheckAsBSendCompletion(int *opResult)
{
    return Par_CheckAsBSendCompletion(Partner ,opResult)==JobComplete;
}
//---------------------------------------------------------------------------
int TS7Partner::WaitAsBSendCompletion(longword Timeout)
{
    return Par_WaitAsBSendCompletion(Partner, Timeout);
}
//---------------------------------------------------------------------------
int TS7Partner::SetSendCallback(pfn_ParSendCompletion pCompletion, void *usrPtr)
{
    return Par_SetSendCallback(Partner, pCompletion, usrPtr);
}
//---------------------------------------------------------------------------
int TS7Partner::BRecv(longword *R_ID, void *pUsrData, int *Size, longword Timeout)
{
    return Par_BRecv(Partner, R_ID, pUsrData, Size, Timeout);
}
//---------------------------------------------------------------------------
bool TS7Partner::CheckAsBRecvCompletion(int *opResult, longword *R_ID, void *pUsrData, int *Size)
{
    return Par_CheckAsBRecvCompletion(Partner, opResult, R_ID, pUsrData, Size) == JobComplete;
}
//---------------------------------------------------------------------------
int TS7Partner::SetRecvCallback(pfn_ParRecvCallBack pCallback, void *usrPtr)
{
    return Par_SetRecvCallback(Partner, pCallback, usrPtr);
}
//---------------------------------------------------------------------------
int TS7Partner::Status()
{
    int ParStatus;
    int Result = Par_GetStatus(Partner, &ParStatus);
    if (Result==0)
        return ParStatus;
    else
        return Result;
}
//---------------------------------------------------------------------------
int TS7Partner::LastError()
{
    int Error;
    int Result = Par_GetLastError(Partner, &Error);
    if (Result==0)
        return Error;
    else
        return Result;
}
//---------------------------------------------------------------------------
int TS7Partner::GetTimes(longword *SendTime, longword *RecvTime)
{
    return Par_GetTimes(Partner, SendTime, RecvTime);
}
//---------------------------------------------------------------------------
int TS7Partner::GetStats(longword *BytesSent, longword *BytesRecv, longword *ErrSend, longword *ErrRecv)
{
    return Par_GetStats(Partner, BytesSent, BytesRecv, ErrSend, ErrRecv);
}
//---------------------------------------------------------------------------
bool TS7Partner::Linked()
{
    return Status()==par_linked;
}

//******************************************************************************
//                               OpenPLC interface
//******************************************************************************

TS7Server *Server = NULL;
bool s7Inited = false;
#define MAX_S7IO BUFFER_SIZE*8
#define MK_SIZE 16   

typedef uint8_t* pByteArray[];
typedef uint16_t* pWordArray[];
typedef uint32_t* pDWordArray[];

IEC_BOOL DI[MAX_S7IO];
IEC_BOOL DQ[MAX_S7IO];
IEC_UINT DB2[BUFFER_SIZE];
IEC_UINT DB102[BUFFER_SIZE];
IEC_UINT DB1002[BUFFER_SIZE];
IEC_UDINT DB1004[BUFFER_SIZE];
IEC_ULINT DB1008[BUFFER_SIZE];

// Sometime WinCC request the access to low merkers. I guess to check if this 
// is a Siemens real hardware or for watchdog purpose, since Merkers exist 
// in *every* CPU even if it's empty.
IEC_BYTE MK[MK_SIZE];

//------------------------------------------------------------------------------
// Returns the byte size of given Siemens WordLength
//------------------------------------------------------------------------------
int DataSizeByte(int WordLength)
{
    switch (WordLength) {
    case S7WLBit: return 1;  // S7 sends 1 byte per bit
    case S7WLByte: return 1;
    case S7WLChar: return 1;
    case S7WLWord: return 2;
    case S7WLDWord: return 4;
    case S7WLInt: return 2;
    case S7WLDInt: return 4;
    case S7WLReal: return 4;
    case S7WLCounter: return 2;
    case S7WLTimer: return 2;
    default: return 0;
    }
}
//------------------------------------------------------------------------------
// Events callback: it's fired after the completion of a S7 transaction or 
// after a system operation.
// Srv_EventText supplies a good enough default description, if you want to
// customise it have a look at Snap7 Reference Manual pag.45
//------------------------------------------------------------------------------
void S7API EventCallBack(void* usrPtr, PSrvEvent PEvent, int Size)
{
    char s7text[512];
    char log_msg[1000];
    // log the event
    Srv_EventText(PEvent, s7text, sizeof(s7text));
    sprintf(log_msg, "Snap7: %s\n", s7text);
    openplc_log(log_msg);
};

//------------------------------------------------------------------------------
// Map unused vars.
//------------------------------------------------------------------------------
// The S7 driver cannot work on the holed memory, this would cause problems
// for the client (HMI) that could drop access to the entire resource.
// So we allocate everything left over from GlueVars and the modbus driver.
//
// It might seem like a waste of memory to allocate a buffer for I / O since we 
// know that modbus already did it. But in a modularity perspective we cannot 
// tie ourselves to other drivers. If one day it is decided to make everything
// modular, the S7 driver must be able to continue to function autonomously.
//------------------------------------------------------------------------------
void s7mapUnusedVars()
{
    // Clears memory
    memset(&DI, 0, sizeof(DI));             // %DI
    memset(&DQ, 0, sizeof(DQ));             // %DQ
    memset(&DB2, 0, sizeof(DB2));           // %IW
    memset(&DB102, 0, sizeof(DB102));       // %QW
    memset(&DB1002, 0, sizeof(DB1002));     // %MW 
    memset(&DB1004, 0, sizeof(DB1004));     // %MD
    
    memset(&MK, 0, sizeof(MK));             // Internal

    for (int c = 0; c < MAX_S7IO; c++)
        if (bool_input[c / 8][c % 8] == NULL) bool_input[c / 8][c % 8] = &DI[c];

    for (int c = 0; c < MAX_S7IO; c++)
        if (bool_output[c / 8][c % 8] == NULL) bool_output[c / 8][c % 8] = &DQ[c];

    for (int c = 0; c < BUFFER_SIZE; c++)
        if (int_input[c] == NULL) int_input[c] = &DB2[c];
    for (int c = 0; c < BUFFER_SIZE; c++)
        if (int_output[c] == NULL) int_output[c] = &DB102[c];

    for (int c = 0; c < BUFFER_SIZE; c++)
        if (int_memory[c] == NULL) int_memory[c] = &DB1002[c];
    for (int c = 0; c < BUFFER_SIZE; c++)
        if (dint_memory[c] == NULL) dint_memory[c] = &DB1004[c];
}
//------------------------------------------------------------------------------
// Data access functions
//------------------------------------------------------------------------------
int getBytesFromByteArray(pByteArray byteArray, int Start, int Size, pbyte pUsrData)
{
    if (Start + Size >= BUFFER_SIZE)
        return _s7error;

    for (int c = 0; c < Size; c++)
        *(pUsrData + c) = *byteArray[Start + c];

    return _s7ok;
}
//------------------------------------------------------------------------------
int putBytesToByteArray(pByteArray byteArray, int Start, int Size, pbyte pUsrData)
{
    if (Start + Size >= BUFFER_SIZE)
        return _s7error;

    for (int c = 0; c < Size; c++)
        *byteArray[Start + c] = *(pUsrData + c);

    return _s7ok;
}
//------------------------------------------------------------------------------
int getBytesFromWordArray(pWordArray wordArray, int Start, int Size, pbyte pUsrData)
{
    int WordStart = Start / 2;
    int WordAmount = Size / 2;
    uint16_t MyWord;
    int idx = 0;

    if (WordStart + WordAmount >= BUFFER_SIZE)
        return _s7error;

    for (int c = 0; c < WordAmount; c++)
    {
        MyWord = *wordArray[WordStart + c];
        *(pUsrData + idx++) = (MyWord >> 8) & 0xFF;
        *(pUsrData + idx++) = MyWord & 0xFF;
    }

    return _s7ok;
}
//------------------------------------------------------------------------------
int putBytesToWordArray(pWordArray wordArray, int Start, int Size, pbyte pUsrData)
{
    int WordStart = Start / 2;
    int WordAmount = Size / 2;
    uint16_t WL, WH;
    int idx = 0;

    if (WordStart + WordAmount >= BUFFER_SIZE)
        return _s7error;

    for (int c = 0; c < WordAmount; c++)
    {
        WH = *(pUsrData + idx++);
        WL = *(pUsrData + idx++);
        *wordArray[WordStart + c] = (WH << 8) | WL;
    }

    return _s7ok;
}
//------------------------------------------------------------------------------
int getBytesFromDWordArray(pDWordArray dwordArray, int Start, int Size, pbyte pUsrData)
{
    int DWordStart = Start / 4;
    int DWordAmount = Size / 4;
    uint32_t MyDWord;
    int idx = 0;

    if (DWordStart + DWordAmount >= BUFFER_SIZE)
        return _s7error;

    for (int c = 0; c < DWordAmount; c++)
    {
        MyDWord = *dwordArray[DWordStart + c];
        *(pUsrData + idx++) = (MyDWord >> 24) & 0xFF;
        *(pUsrData + idx++) = (MyDWord >> 16) & 0xFF;
        *(pUsrData + idx++) = (MyDWord >> 8) & 0xFF;
        *(pUsrData + idx++) = MyDWord & 0xFF;
    }

    return _s7ok;
}
//------------------------------------------------------------------------------
int putBytesToDWordArray(pDWordArray dwordArray, int Start, int Size, pbyte pUsrData)
{
    int DWordStart = Start / 4;
    int DWordAmount = Size / 4;
    int idx = 0;
    uint32_t DW3, DW2, DW1, DW0;

    if (DWordStart + DWordAmount >= BUFFER_SIZE)
        return _s7error;

    for (int c = 0; c < DWordAmount; c++)
    {
        DW0 = *(pUsrData + idx++);
        DW1 = *(pUsrData + idx++);
        DW2 = *(pUsrData + idx++);
        DW3 = *(pUsrData + idx++);

        *dwordArray[DWordStart + c] = (DW0 << 24) | (DW1 << 16) | (DW2 << 8) | DW3;
    }

    return _s7ok;
}
//------------------------------------------------------------------------------
int getBitFromWordArray(pWordArray wordArray, int bitStart, pbyte pUsrData)
{
    uint16_t mask[16] = { 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
                          0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000 };

    int WordIndex = bitStart / 16;
    int BitIndex  = bitStart % 16;

    if (WordIndex >= BUFFER_SIZE)
        return _s7error;
    
    *pUsrData = (*wordArray[WordIndex] & mask[BitIndex]) != 0 ? 1 : 0;
     
    return _s7ok;
}
//------------------------------------------------------------------------------
int putBitToWordArray(pWordArray wordArray, int bitStart, pbyte pUsrData)
{
    uint16_t mask[16] = { 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
                          0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000 };

    int WordIndex = bitStart / 16;
    int BitIndex = bitStart % 16;

    if (WordIndex >= BUFFER_SIZE)
        return _s7error;

    if (*pUsrData != 0)
        *wordArray[WordIndex] = *wordArray[WordIndex] | mask[BitIndex];
    else
        *wordArray[WordIndex] = *wordArray[WordIndex] & ~mask[BitIndex];

    return _s7ok;
}
//------------------------------------------------------------------------------
int getBitFromDWordArray(pDWordArray dwordArray, int bitStart, pbyte pUsrData)
{
    uint32_t mask[32] = { 0x00000001, 0x00000002, 0x00000004, 0x00000008, 0x00000010, 0x00000020, 0x00000040, 0x00000080,
                          0x00000100, 0x00000200, 0x00000400, 0x00000800, 0x00001000, 0x00002000, 0x00004000, 0x00008000,
                          0x00010000, 0x00020000, 0x00040000, 0x00080000, 0x00100000, 0x00200000, 0x00400000, 0x00800000,
                          0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000 };

    int DWordIndex = bitStart / 32;
    int BitIndex = bitStart % 32;

    if (DWordIndex >= BUFFER_SIZE)
        return _s7error;

    *pUsrData = (*dwordArray[DWordIndex] & mask[BitIndex]) != 0 ? 1 : 0;

    return _s7ok;
}
//------------------------------------------------------------------------------
int putBitToDWordArray(pDWordArray dwordArray, int bitStart, pbyte pUsrData)
{
    uint32_t mask[32] = { 0x00000001, 0x00000002, 0x00000004, 0x00000008, 0x00000010, 0x00000020, 0x00000040, 0x00000080,
                          0x00000100, 0x00000200, 0x00000400, 0x00000800, 0x00001000, 0x00002000, 0x00004000, 0x00008000,
                          0x00010000, 0x00020000, 0x00040000, 0x00080000, 0x00100000, 0x00200000, 0x00400000, 0x00800000,
                          0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000 };

    int DWordIndex = bitStart / 32;
    int BitIndex = bitStart % 32;

    if (DWordIndex >= BUFFER_SIZE)
        return _s7error;

    if (*pUsrData != 0)
        *dwordArray[DWordIndex] = *dwordArray[DWordIndex] | mask[BitIndex];
    else
        *dwordArray[DWordIndex] = *dwordArray[DWordIndex] & ~mask[BitIndex];

    return _s7ok;
}
//------------------------------------------------------------------------------
int getMerker(PS7Tag PTag, pbyte pUsrData)
{
    int Start;
    uint8_t mask[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
    if (PTag->WordLen == S7WLBit)
    {
        Start = PTag->Start / 8;
        if (Start >= MK_SIZE)
            return _s7error;
        *pUsrData = (MK[Start] & mask[PTag->Start % 8]) != 0 ? 1 : 0;
    }
    else {
        if (PTag->Start >= MK_SIZE)
            return _s7error;
        *pUsrData = MK[PTag->Start];
    }
    return _s7ok;
}
//------------------------------------------------------------------------------
int putMerker(PS7Tag PTag, pbyte pUsrData)
{
    int Start;
    uint8_t mask[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
    if (PTag->WordLen == S7WLBit)
    {
        Start = PTag->Start / 8;
        if (Start >= MK_SIZE)
            return _s7error;
        if (*pUsrData != 0)
            MK[Start] = MK[Start] | mask[PTag->Start % 8];
        else
            MK[Start] = MK[Start] & ~mask[PTag->Start % 8];
    }
    else {
        if (PTag->Start >= MK_SIZE)
            return _s7error;
        MK[PTag->Start] = *pUsrData;
    }
    return _s7ok;
}
//------------------------------------------------------------------------------
int getIOBit(bool isInput, int bitStart, pbyte pUsrData)
{
    uint16_t byteStart = bitStart / 8;
    if (byteStart >= BUFFER_SIZE)
        return _s7error;

    *pUsrData = isInput ? *bool_input[byteStart][bitStart % 8] : *bool_output[byteStart][bitStart % 8];

    return _s7ok;
}
//------------------------------------------------------------------------------
int putIOBit(bool isInput, int bitStart, pbyte pUsrData)
{
    int byteStart = bitStart / 8;
    if (byteStart >= BUFFER_SIZE)
        return _s7error;

    if (isInput)
        *bool_input[byteStart][bitStart % 8] = *pUsrData != 0 ? 1 : 0;
    else
        *bool_output[byteStart][bitStart % 8] = *pUsrData != 0 ? 1 : 0;

    return _s7ok;
}
//------------------------------------------------------------------------------
int getIOBytes(bool isInput, int Start, int Size, pbyte pUsrData)
{
    uint8_t mask[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

    if (Start + Size >= BUFFER_SIZE)
        return _s7error;

    for (int x = 0; x < Size; x++)
    {
        *(pUsrData + x) = 0;
        for (int c = 0; c < 8; c++)
        {
            if (isInput)
            {
                if (*bool_input[Start + x][c]) *(pUsrData + x) |= mask[c];
            }
            else {
                if (*bool_output[Start + x][c]) *(pUsrData + x) |= mask[c];
            }
        }
    }
    return _s7ok;
}
//------------------------------------------------------------------------------
int putIOBytes(bool isInput, int Start, int Size, pbyte pUsrData)
{
    uint8_t mask[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

    if (Start + Size >= BUFFER_SIZE)
        return _s7error;

    for (int x = 0; x < Size; x++)
    {
        for (int c = 0; c < 8; c++)
        {
            if (isInput)
                *bool_input[Start + x][c] = *(pUsrData + x) & mask[c] ? 1 : 0;
            else
                *bool_output[Start + x][c] = *(pUsrData + x) & mask[c] ? 1 : 0;
        }
    }
    return _s7ok;
}
//------------------------------------------------------------------------------
// Read/Write callback
//------------------------------------------------------------------------------
// Data R/W callback: it's fired during(*) a S7 transaction
//
// (*) Warning, performing operations that take too long (such as accessing 
//     disk or other slow devices) may cause the client to time out.
//     Memory to memory operations are OK.
// 
// usrPtr : user pointer, used to contain an the object reference if an OOP 
//          pattern is used (i.e. to share the "C" callback across classes).
//          Here it's NULL.
//
// Sender : IP Address of the client (to be used with inet_ntoa() if needed).
//
// Operation: OperationRead or OperationWrite
// 
// PTag : Pointer to the Tag info (see definition in oplc_snap7.h)
// 
// pUsrData : Snap7 Server internal buffer.
// 
// Note : for each transaction the maximum data size is that of the negotiated 
//        Siemens PDU which can be max 480 bytes. 
//------------------------------------------------------------------------------
int S7API RWAreaCallBack(void* usrPtr, int Sender, int Operation, PS7Tag PTag, void* pUsrData)
{
    int Size = DataSizeByte(PTag->WordLen) * PTag->Elements;
    if (Size == 0 || PTag->WordLen == S7WLCounter || PTag->WordLen == S7WLTimer) // Wrong WordLen or Elements = 0 
        return _s7error;

    bool DoRead = Operation == OperationRead;
    int Result = _s7ok;

    // Lock buffer
    pthread_mutex_lock(&bufferLock);

    switch (PTag->Area)
    {
    case S7AreaPE:
    case S7AreaPA:
        if (PTag->WordLen == S7WLBit)
        {
            Result = DoRead ? getIOBit(PTag->Area == S7AreaPE, PTag->Start, pbyte(pUsrData)) : putIOBit(PTag->Area == S7AreaPE, PTag->Start, pbyte(pUsrData));
        }
        else {
            Result = DoRead ? getIOBytes(PTag->Area == S7AreaPE, PTag->Start, Size, pbyte(pUsrData)) : putIOBytes(PTag->Area == S7AreaPE, PTag->Start, Size, pbyte(pUsrData));
        }
        break;
    case S7AreaMK:
        Result = DoRead ? getMerker(PTag, pbyte(pUsrData)) : putMerker(PTag, pbyte(pUsrData));
        break;
    case S7AreaDB:
        switch (PTag->DBNumber)
        {
        case 2:
            if (PTag->WordLen == S7WLBit)
            {
                Result = DoRead ? getBitFromWordArray(int_input, PTag->Start, pUsrData) : putBitToWordArray(int_input, PTag->Start, pUsrData);
            }
            else {
                Result = DoRead ? getBytesFromWordArray(int_input, PTag->Start, Size, pbyte(pUsrData)) :
                    putBytesToWordArray(int_input, PTag->Start, Size, pbyte(pUsrData));
            }
            break;
        case 102:
            if (PTag->WordLen == S7WLBit)
            {
                Result = DoRead ? getBitFromWordArray(int_output, PTag->Start, pUsrData) : putBitToWordArray(int_output, PTag->Start, pUsrData);
            }
            else {
                Result = DoRead ? getBytesFromWordArray(int_output, PTag->Start, Size, pbyte(pUsrData)) :
                    putBytesToWordArray(int_output, PTag->Start, Size, pbyte(pUsrData));
            }
            break;
        case 1002:
            if (PTag->WordLen == S7WLBit)
            {
                Result = DoRead ? getBitFromWordArray(int_memory, PTag->Start, pUsrData) : putBitToWordArray(int_memory, PTag->Start, pUsrData);
            }
            else {
                Result = DoRead ? getBytesFromWordArray(int_memory, PTag->Start, Size, pbyte(pUsrData)) :
                    putBytesToWordArray(int_memory, PTag->Start, Size, pbyte(pUsrData));
            }
            break;
        case 1004:
            if (PTag->WordLen == S7WLBit)
            {
                Result = DoRead ? getBitFromDWordArray(dint_memory, PTag->Start, pUsrData) : putBitToDWordArray(dint_memory, PTag->Start, pUsrData);
            }
            else {
                Result = DoRead ? getBytesFromDWordArray(dint_memory, PTag->Start, Size, pbyte(pUsrData)) :
                    putBytesToDWordArray(dint_memory, PTag->Start, Size, pbyte(pUsrData));
            }
            break;
        default: // Illegal DB Number
            Result = _s7error;
        }
        break;
    default: // Unknown or unsupported Area 
        Result = _s7error;
    }
    // Unlock buffer
    pthread_mutex_unlock(&bufferLock);

    return Result;
}
//------------------------------------------------------------------------------
// Snap7 Server initialization
//------------------------------------------------------------------------------
void initializeSnap7()
{
    if (!s7Inited)
    {
        s7mapUnusedVars();

        Server = new TS7Server;
        // With the next function we can limit the events to start/stop/client added etc.
        // For a deep debug comment the line
        Server->SetEventsMask(0x3ff);
        // Set the Server events callback
        Server->SetEventsCallback(EventCallBack, NULL);

        // Shared resources:
        // We cannot directly share OpenPLC internal buffers, because:
        // 1 - They are array of pointers to vars
        // 2 - The access would be not synchronized (i.e. not consistent)
        // So, we will use a callback and, inside it, we will perform a synchronized data transfer.
        Server->SetRWAreaCallback(RWAreaCallBack, NULL);
        s7Inited = true;
    }
 }

//------------------------------------------------------------------------------
// Snap7 Server start
//------------------------------------------------------------------------------
void startSnap7()
{
    // Listen on S7 Port 102. 
    // If Server is already running the command will be ignored.
    if (s7Inited)
        Server->StartTo("0.0.0.0"); // Success or fail will be logged into EventCallBack   
}

//------------------------------------------------------------------------------
// Snap7 Server stop
//------------------------------------------------------------------------------
void stopSnap7()
{
    // If Server is already stopped the command will be ignored.
    if (s7Inited)
        Server->Stop();
}

//------------------------------------------------------------------------------
// Snap7 Server destruction
//------------------------------------------------------------------------------
void finalizeSnap7()
{
    if (s7Inited)
    {
        s7Inited = false;
        Server->Stop();
        delete Server;
        Server = NULL;
    }
}
