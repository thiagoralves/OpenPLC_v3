/*=============================================================================|
|  PROJECT SNAP7                                                         1.4.1 |
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
#include "snap7_libmain.h"

#ifndef OS_WINDOWS
void libinit(void) __attribute__((constructor));
void libdone(void) __attribute__((destructor));
#endif

static bool libresult = true;

void libinit(void)
{
     // in future expansions here can be inserted some initialization code
     libresult=true;
}

void libdone(void)
{
     // in future expansions here can be inserted some destruction code
}

#ifdef OS_WINDOWS
BOOL APIENTRY DllMain (HINSTANCE hInst,
                       DWORD reason,
                       LPVOID reserved)
{
    switch (reason)
    {
      case DLL_PROCESS_ATTACH:
        libinit();
        break;
      case DLL_PROCESS_DETACH:
        libdone();
        break;
      case DLL_THREAD_ATTACH:
        break;
      case DLL_THREAD_DETACH:
        break;
    }
    return libresult;
}
#endif

//***************************************************************************
// CLIENT
//***************************************************************************
S7Object S7API Cli_Create()
{    
    return S7Object(new TSnap7Client());
}
//---------------------------------------------------------------------------
void S7API Cli_Destroy(S7Object &Client)
{
    if (Client)
    {
        delete PSnap7Client(Client);
        Client=0;
    }
}
//---------------------------------------------------------------------------
int S7API Cli_SetConnectionParams(S7Object Client, const char *Address, word LocalTSAP, word RemoteTSAP)
{
    if (Client)
    {
        PSnap7Client(Client)->SetConnectionParams(Address, LocalTSAP, RemoteTSAP);
        return 0;
    }
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_SetConnectionType(S7Object Client, word ConnectionType)
{
    if (Client)
    {
        PSnap7Client(Client)->SetConnectionType(ConnectionType);
        return 0;
    }
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_ConnectTo(S7Object Client, const char *Address, int Rack, int Slot)
{
    if (Client)
        return PSnap7Client(Client)->ConnectTo(Address, Rack, Slot);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_Connect(S7Object Client)
{
    if (Client)
        return PSnap7Client(Client)->Connect();
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_Disconnect(S7Object Client)
{
    if (Client)
        return PSnap7Client(Client)->Disconnect();
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_GetParam(S7Object Client, int ParamNumber, void *pValue)
{
    if (Client)
        return PSnap7Client(Client)->GetParam(ParamNumber, pValue);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_SetParam(S7Object Client, int ParamNumber, void *pValue)
{
    if (Client)
        return PSnap7Client(Client)->SetParam(ParamNumber, pValue);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_SetAsCallback(S7Object Client, pfn_CliCompletion pCompletion, void *usrPtr)
{
    if (Client)
        return PSnap7Client(Client)->SetAsCallback(pCompletion, usrPtr);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_ReadArea(S7Object Client, int Area, int DBNumber, int Start, int Amount, int WordLen, void *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->ReadArea(Area, DBNumber, Start, Amount, WordLen, pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_WriteArea(S7Object Client, int Area, int DBNumber, int Start, int Amount, int WordLen, void *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->WriteArea(Area, DBNumber, Start, Amount, WordLen, pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_ReadMultiVars(S7Object Client, PS7DataItem Item, int ItemsCount)
{
    if (Client)
        return PSnap7Client(Client)->ReadMultiVars(Item, ItemsCount);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_WriteMultiVars(S7Object Client, PS7DataItem Item, int ItemsCount)
{
    if (Client)
        return PSnap7Client(Client)->WriteMultiVars(Item, ItemsCount);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_DBRead(S7Object Client, int DBNumber, int Start, int Size, void *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->DBRead(DBNumber, Start, Size, pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_DBWrite(S7Object Client, int DBNumber, int Start, int Size, void *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->DBWrite(DBNumber, Start, Size, pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_MBRead(S7Object Client, int Start, int Size, void *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->MBRead(Start, Size, pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_MBWrite(S7Object Client, int Start, int Size, void *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->MBWrite(Start, Size, pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_EBRead(S7Object Client, int Start, int Size, void *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->EBRead(Start, Size, pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_EBWrite(S7Object Client, int Start, int Size, void *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->EBWrite(Start, Size, pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_ABRead(S7Object Client, int Start, int Size, void *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->ABRead(Start, Size, pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_ABWrite(S7Object Client, int Start, int Size, void *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->ABWrite(Start, Size, pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_TMRead(S7Object Client, int Start, int Amount, void *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->TMRead(Start, Amount, pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_TMWrite(S7Object Client, int Start, int Amount, void *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->TMWrite(Start, Amount, pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_CTRead(S7Object Client, int Start, int Amount, void *pUsrData)
{
    if (Client)
         return PSnap7Client(Client)->CTRead(Start, Amount, pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_CTWrite(S7Object Client, int Start, int Amount, void *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->CTWrite(Start, Amount, pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_ListBlocks(S7Object Client, TS7BlocksList *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->ListBlocks(pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_GetAgBlockInfo(S7Object Client, int BlockType, int BlockNum, TS7BlockInfo *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->GetAgBlockInfo(BlockType, BlockNum, pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_GetPgBlockInfo(S7Object Client, void *pBlock, TS7BlockInfo *pUsrData, int Size)
{
    if (Client)
        return PSnap7Client(Client)->GetPgBlockInfo(pBlock, pUsrData, Size);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_ListBlocksOfType(S7Object Client, int BlockType, TS7BlocksOfType *pUsrData, int &ItemsCount)
{
    if (Client)
        return PSnap7Client(Client)->ListBlocksOfType(BlockType, pUsrData, ItemsCount);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_Upload(S7Object Client, int BlockType, int BlockNum, void *pUsrData, int &Size)
{
    if (Client)
        return PSnap7Client(Client)->Upload(BlockType, BlockNum, pUsrData, Size);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_FullUpload(S7Object Client, int BlockType, int BlockNum, void *pUsrData, int &Size)
{
    if (Client)
        return PSnap7Client(Client)->FullUpload(BlockType, BlockNum, pUsrData, Size);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_Download(S7Object Client, int BlockNum, void *pUsrData, int Size)
{
    if (Client)
        return PSnap7Client(Client)->Download(BlockNum, pUsrData, Size);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_Delete(S7Object Client, int BlockType, int BlockNum)
{
    if (Client)
        return PSnap7Client(Client)->Delete(BlockType, BlockNum);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_DBGet(S7Object Client, int DBNumber, void *pUsrData, int &Size)
{
    if (Client)
        return PSnap7Client(Client)->DBGet(DBNumber, pUsrData, Size);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_DBFill(S7Object Client, int DBNumber, int FillChar)
{
    if (Client)
        return PSnap7Client(Client)->DBFill(DBNumber, FillChar);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_GetPlcDateTime(S7Object Client, tm &DateTime)
{
    if (Client)
        return PSnap7Client(Client)->GetPlcDateTime(DateTime);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_SetPlcDateTime(S7Object Client, tm *DateTime)
{
    if (Client)
        return PSnap7Client(Client)->SetPlcDateTime(DateTime);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_SetPlcSystemDateTime(S7Object Client)
{
    if (Client)
        return PSnap7Client(Client)->SetPlcSystemDateTime();
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_GetOrderCode(S7Object Client, TS7OrderCode *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->GetOrderCode(pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_GetCpuInfo(S7Object Client, TS7CpuInfo *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->GetCpuInfo(pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_GetCpInfo(S7Object Client, TS7CpInfo *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->GetCpInfo(pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_ReadSZL(S7Object Client, int ID, int Index, TS7SZL *pUsrData, int &Size)
{
    if (Client)
        return PSnap7Client(Client)->ReadSZL(ID, Index, pUsrData, Size);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_ReadSZLList(S7Object Client, TS7SZLList *pUsrData, int &ItemsCount)
{
    if (Client)
        return PSnap7Client(Client)->ReadSZLList(pUsrData, ItemsCount);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_PlcHotStart(S7Object Client)
{
    if (Client)
        return PSnap7Client(Client)->PlcHotStart();
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_PlcColdStart(S7Object Client)
{
    if (Client)
        return PSnap7Client(Client)->PlcColdStart();
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_PlcStop(S7Object Client)
{
    if (Client)
        return PSnap7Client(Client)->PlcStop();
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_CopyRamToRom(S7Object Client, int Timeout)
{
    if (Client)
        return PSnap7Client(Client)->CopyRamToRom(Timeout);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_Compress(S7Object Client, int Timeout)
{
    if (Client)
        return PSnap7Client(Client)->Compress(Timeout);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_GetPlcStatus(S7Object Client, int &Status)
{
    if (Client)
        return PSnap7Client(Client)->GetPlcStatus(Status);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_GetProtection(S7Object Client, TS7Protection *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->GetProtection(pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_SetSessionPassword(S7Object Client, char *Password)
{
    if (Client)
        return PSnap7Client(Client)->SetSessionPassword(Password);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_ClearSessionPassword(S7Object Client)
{
    if (Client)
        return PSnap7Client(Client)->ClearSessionPassword();
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_IsoExchangeBuffer(S7Object Client, void *pUsrData, int &Size)
{
    if (Client)
        return PSnap7Client(Client)->isoExchangeBuffer(pUsrData, Size);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_GetExecTime(S7Object Client, int &Time)
{
    if (Client)
    {
        Time=PSnap7Client(Client)->Time();
        return 0;
    }
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_GetLastError(S7Object Client, int &LastError)
{
    if (Client)
    {
        LastError=PSnap7Client(Client)->LastError;
        return 0;
    }
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_GetPduLength(S7Object Client, int &Requested, int &Negotiated)
{
    if (Client)
    {
        Negotiated=PSnap7Client(Client)->PDULength;
        Requested =PSnap7Client(Client)->PDURequest;
        return 0;
    }
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_ErrorText(int Error, char *Text, int TextLen)
{
	try{
		ErrCliText(Error, Text, TextLen);
		Text[TextLen-1] = '\0';
	}
	catch (...){
		return errLibInvalidParam;
	}
	return 0;
}
//---------------------------------------------------------------------------
int S7API Cli_GetConnected(S7Object Client, int &Connected)
{
    Connected=0;
	if (Client)
    {
		Connected=PSnap7Client(Client)->Connected;
		return 0;
	}
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_AsReadArea(S7Object Client, int Area, int DBNumber, int Start, int Amount, int WordLen, void *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->AsReadArea(Area, DBNumber, Start, Amount, WordLen, pUsrData);
    else
        return errLibInvalidParam;
}
//---------------------------------------------------------------------------
int S7API Cli_AsWriteArea(S7Object Client, int Area, int DBNumber, int Start, int Amount, int WordLen, void *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->AsWriteArea(Area, DBNumber, Start, Amount, WordLen, pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_AsDBRead(S7Object Client, int DBNumber, int Start, int Size, void *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->AsDBRead(DBNumber, Start, Size, pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_AsDBWrite(S7Object Client, int DBNumber, int Start, int Size, void *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->AsDBWrite(DBNumber, Start, Size, pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_AsMBRead(S7Object Client, int Start, int Size, void *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->AsMBRead(Start, Size, pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_AsMBWrite(S7Object Client, int Start, int Size, void *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->AsMBWrite(Start, Size, pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_AsEBRead(S7Object Client, int Start, int Size, void *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->AsEBRead(Start, Size, pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_AsEBWrite(S7Object Client, int Start, int Size, void *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->AsEBWrite(Start, Size, pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_AsABRead(S7Object Client, int Start, int Size, void *pUsrData)
{
	if (Client)
        return PSnap7Client(Client)->AsABRead(Start, Size, pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_AsABWrite(S7Object Client, int Start, int Size, void *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->AsABWrite(Start, Size, pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_AsTMRead(S7Object Client, int Start, int Amount, void *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->AsTMRead(Start, Amount, pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_AsTMWrite(S7Object Client, int Start, int Amount, void *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->AsTMWrite(Start, Amount, pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_AsCTRead(S7Object Client, int Start, int Amount, void *pUsrData)
{
    if (Client)
        return PSnap7Client(Client)->AsCTRead(Start, Amount, pUsrData);
	else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_AsCTWrite(S7Object Client, int Start, int Amount, void *pUsrData)
{
	if (Client)
        return PSnap7Client(Client)->AsCTWrite(Start, Amount, pUsrData);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_AsListBlocksOfType(S7Object Client, int BlockType, TS7BlocksOfType *pUsrData, int &ItemsCount)
{
    if (Client)
        return PSnap7Client(Client)->AsListBlocksOfType(BlockType, pUsrData, ItemsCount);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_AsReadSZL(S7Object Client, int ID, int Index, TS7SZL *pUsrData, int &Size)
{
    if (Client)
        return PSnap7Client(Client)->AsReadSZL(ID, Index, pUsrData, Size);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_AsReadSZLList(S7Object Client, TS7SZLList *pUsrData, int &ItemsCount)
{
    if (Client)
        return PSnap7Client(Client)->AsReadSZLList(pUsrData, ItemsCount);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_AsUpload(S7Object Client, int BlockType, int BlockNum, void *pUsrData, int &Size)
{
    if (Client)
        return PSnap7Client(Client)->AsUpload(BlockType, BlockNum, pUsrData, Size);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_AsFullUpload(S7Object Client, int BlockType, int BlockNum, void *pUsrData, int &Size)
{
    if (Client)
        return PSnap7Client(Client)->AsFullUpload(BlockType, BlockNum, pUsrData, Size);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_AsDownload(S7Object Client, int BlockNum, void *pUsrData, int Size)
{
    if (Client)
        return PSnap7Client(Client)->AsDownload(BlockNum, pUsrData, Size);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_AsCopyRamToRom(S7Object Client, int Timeout)
{
    if (Client)
        return PSnap7Client(Client)->AsCopyRamToRom(Timeout);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_AsCompress(S7Object Client, int Timeout)
{
    if (Client)
        return PSnap7Client(Client)->Compress(Timeout);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_AsDBGet(S7Object Client, int DBNumber, void *pUsrData, int &Size)
{
	if (Client)
        return PSnap7Client(Client)->AsDBGet(DBNumber, pUsrData, Size);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_AsDBFill(S7Object Client, int DBNumber, int FillChar)
{
    if (Client)
        return PSnap7Client(Client)->AsDBFill(DBNumber, FillChar);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_CheckAsCompletion(S7Object Client, int &opResult)
{
    if (Client)
    {
        if (PSnap7Client(Client)->CheckAsCompletion(opResult))
            return JobComplete;
        else
            return JobPending;
    }
    else
		return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Cli_WaitAsCompletion(S7Object Client, int Timeout)
{
    if (Client)
        return PSnap7Client(Client)->WaitAsCompletion(Timeout);
    else
        return errLibInvalidObject;
}
//***************************************************************************
// SERVER
//***************************************************************************
S7Object S7API Srv_Create()
{
    return S7Object(new TSnap7Server());
}
//---------------------------------------------------------------------------
void S7API Srv_Destroy(S7Object &Server)
{
    if (Server)
    {
        delete PSnap7Server(Server);
        Server=0;
    }
}
//---------------------------------------------------------------------------
int S7API Srv_GetParam(S7Object Server, int ParamNumber, void *pValue)
{
    if (Server)
        return PSnap7Server(Server)->GetParam(ParamNumber, pValue);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Srv_SetParam(S7Object Server, int ParamNumber, void *pValue)
{
    if (Server)
        return PSnap7Server(Server)->SetParam(ParamNumber, pValue);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Srv_StartTo(S7Object Server, const char *Address)
{
    if (Server)
        return PSnap7Server(Server)->StartTo(Address);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Srv_Start(S7Object Server)
{
    if (Server)
        return PSnap7Server(Server)->Start();
    else
		return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Srv_Stop(S7Object Server)
{
    if (Server)
    {
        PSnap7Server(Server)->Stop();
        return 0;
    }
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Srv_RegisterArea(S7Object Server, int AreaCode, word Index, void *pUsrData, int Size)
{
    if (Server)
        return PSnap7Server(Server)->RegisterArea(AreaCode, Index, pUsrData, Size);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Srv_UnregisterArea(S7Object Server, int AreaCode, word Index)
{
    if (Server)
        return PSnap7Server(Server)->UnregisterArea(AreaCode, Index);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Srv_LockArea(S7Object Server, int AreaCode, word Index)
{
    if (Server)
        return PSnap7Server(Server)->LockArea(AreaCode, Index);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Srv_UnlockArea(S7Object Server, int AreaCode, word Index)
{
    if (Server)
        return PSnap7Server(Server)->UnlockArea(AreaCode, Index);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Srv_GetStatus(S7Object Server, int &ServerStatus, int &CpuStatus, int &ClientsCount)
{
    if (Server)
    {
        ServerStatus=PSnap7Server(Server)->Status;
        CpuStatus=PSnap7Server(Server)->CpuStatus;
        ClientsCount=PSnap7Server(Server)->ClientsCount;
        return 0;
	}
	else
		return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Srv_SetCpuStatus(S7Object Server, int CpuStatus)
{
	if (Server)
	{
		PSnap7Server(Server)->CpuStatus=CpuStatus;
		return 0;
	}
	else
		return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Srv_ErrorText(int Error, char *Text, int TextLen)
{
	try{
		ErrSrvText(Error, Text, TextLen);
		Text[TextLen-1] = '\0';
	}
	catch (...){
		return errLibInvalidParam;
	}
	return 0;
}
//---------------------------------------------------------------------------
int S7API Srv_EventText(TSrvEvent &Event, char *Text, int TextLen)
{
	try{
		EvtSrvText(Event, Text, TextLen);
		//Text[TextLen] = '\0';
	}
	catch (...){
		return errLibInvalidParam;
	}
	return 0;
}
//---------------------------------------------------------------------------
int S7API Srv_PickEvent(S7Object Server, TSrvEvent *pEvent, int &EvtReady)
{
	EvtReady=0;
	if (Server)
	{
		EvtReady=int(PSnap7Server(Server)->PickEvent(pEvent));
		return 0;
	}
	else
		return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Srv_ClearEvents(S7Object Server)
{
	if (Server)
	{
		PSnap7Server(Server)->EventsFlush();
		return 0;
	}
	else
		return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Srv_GetMask(S7Object Server, int MaskKind, longword &Mask)
{
	if (Server)
	{
		Mask=0;
		if ((MaskKind==mkEvent) || (MaskKind==mkLog))
		{
			if (MaskKind==mkEvent)
			   Mask=PSnap7Server(Server)->EventMask;
			else
			   Mask=PSnap7Server(Server)->LogMask;
			return 0;
		}
		else
			return errLibInvalidParam;
	}
	else
		return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Srv_SetMask(S7Object Server, int MaskKind, longword Mask)
{
	if (Server)
	{
		if ((MaskKind==mkEvent) || (MaskKind==mkLog))
		{
			if (MaskKind==mkEvent)
			   PSnap7Server(Server)->EventMask=Mask;
			else
			   PSnap7Server(Server)->LogMask=Mask;
			return 0;
		}
		else
			return errLibInvalidParam;
	}
	else
		return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Srv_SetEventsCallback(S7Object Server, pfn_SrvCallBack pCallback, void *usrPtr)
{
	if (Server)
		return PSnap7Server(Server)->SetEventsCallBack(pCallback, usrPtr);
	else
		return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Srv_SetReadEventsCallback(S7Object Server, pfn_SrvCallBack pCallback, void *usrPtr)
{
	if (Server)
		return PSnap7Server(Server)->SetReadEventsCallBack(pCallback, usrPtr);
	else
		return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Srv_SetRWAreaCallback(S7Object Server, pfn_RWAreaCallBack pCallback, void *usrPtr)
{
	if (Server)
		return PSnap7Server(Server)->SetRWAreaCallBack(pCallback, usrPtr);
	else
		return errLibInvalidObject;
}
//***************************************************************************
// PARTNER
//***************************************************************************
S7Object S7API Par_Create(int Active)
{
	return S7Object(new TSnap7Partner(Active!=0));
}
//---------------------------------------------------------------------------
void S7API Par_Destroy(S7Object &Partner)
{
	if (Partner)
	{
		delete PSnap7Partner(Partner);
        Partner=0;
    }
}
//---------------------------------------------------------------------------
int S7API Par_GetParam(S7Object Partner, int ParamNumber, void *pValue)
{
    if (Partner)
        return PSnap7Partner(Partner)->GetParam(ParamNumber, pValue);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Par_SetParam(S7Object Partner, int ParamNumber, void *pValue)
{
    if (Partner)
        return PSnap7Partner(Partner)->SetParam(ParamNumber, pValue);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Par_StartTo(S7Object Partner, const char *LocalAddress, const char *RemoteAddress,
    word LocTsap, word RemTsap)
{
    if (Partner)
        return PSnap7Partner(Partner)->StartTo(LocalAddress, RemoteAddress, LocTsap, RemTsap);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Par_Start(S7Object Partner)
{
    if (Partner)
        return PSnap7Partner(Partner)->Start();
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Par_Stop(S7Object Partner)
{
    if (Partner)
        return PSnap7Partner(Partner)->Stop();
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Par_BSend(S7Object Partner, longword R_ID, void *pUsrData, int Size)
{
    if (Partner)
        return PSnap7Partner(Partner)->BSend(R_ID, pUsrData, Size);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Par_AsBSend(S7Object Partner, longword R_ID, void *pUsrData, int Size)
{
    if (Partner)
        return PSnap7Partner(Partner)->AsBSend(R_ID, pUsrData, Size);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Par_CheckAsBSendCompletion(S7Object Partner, int &opResult)
{
	if (Partner)
    {
        if (PSnap7Partner(Partner)->CheckAsBSendCompletion(opResult))
            return JobComplete;
        else
            return JobPending;
    }
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Par_WaitAsBSendCompletion(S7Object Partner, longword Timeout)
{
    if (Partner)
        return PSnap7Partner(Partner)->WaitAsBSendCompletion(Timeout);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Par_SetSendCallback(S7Object Partner, pfn_ParBSendCompletion pCompletion, void *usrPtr)
{
    if (Partner)
        return PSnap7Partner(Partner)->SetSendCallback(pCompletion, usrPtr);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Par_BRecv(S7Object Partner, longword &R_ID, void *pData, int &Size, longword Timeout)
{
    if (Partner)
        return PSnap7Partner(Partner)->BRecv(R_ID, pData, Size, Timeout);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Par_CheckAsBRecvCompletion(S7Object Partner, int &opResult, longword &R_ID,
	void *pData, int &Size)
{
    if (Partner)
    {
        if (PSnap7Partner(Partner)->CheckAsBRecvCompletion(opResult, R_ID, pData, Size))
            return JobComplete;
        else
            return JobPending;
    }
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Par_SetRecvCallback(S7Object Partner, pfn_ParBRecvCallBack pCompletion, void *usrPtr)
{
    if (Partner)
        return PSnap7Partner(Partner)->SetRecvCallback(pCompletion, usrPtr);
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Par_GetTimes(S7Object Partner, longword &SendTime, longword &RecvTime)
{
    if (Partner)
    {
        SendTime=PSnap7Partner(Partner)->SendTime;
        RecvTime=PSnap7Partner(Partner)->RecvTime;
        return 0;
    }
    else
        return errLibInvalidObject;

}
//---------------------------------------------------------------------------
int S7API Par_GetStats(S7Object Partner, longword &BytesSent, longword &BytesRecv,
    longword &SendErrors, longword &RecvErrors)
{
    if (Partner)
    {
        BytesSent=PSnap7Partner(Partner)->BytesSent;
        BytesRecv=PSnap7Partner(Partner)->BytesRecv;
        SendErrors=PSnap7Partner(Partner)->SendErrors;
        RecvErrors=PSnap7Partner(Partner)->RecvErrors;
        return 0;
    }
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Par_GetLastError(S7Object Partner, int &LastError)
{
    if (Partner)
    {
        LastError=PSnap7Partner(Partner)->LastError;
        return 0;
    }
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Par_GetStatus(S7Object Partner, int &Status)
{
    if (Partner)
    {
        Status=PSnap7Partner(Partner)->Status();
        return 0;
    }
    else
        return errLibInvalidObject;
}
//---------------------------------------------------------------------------
int S7API Par_ErrorText(int Error, char *Text, int TextLen)
{
	try{
		ErrParText(Error, Text, TextLen);
		Text[TextLen - 1] = '\0';
	}
	catch (...){
		return errLibInvalidParam;
	}
	return 0;
}

