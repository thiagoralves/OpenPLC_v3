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
#include "s7_client.h"

//---------------------------------------------------------------------------
TSnap7Client::TSnap7Client()
{
     FThread = 0;
     CliCompletion = 0;
	 EvtJob = NULL;
     EvtComplete = NULL;
	 FThread=NULL;
	 ThreadCreated = false;
}
//---------------------------------------------------------------------------
TSnap7Client::~TSnap7Client()
{
    Destroying=true;
    Disconnect();
    CliCompletion=NULL;
	if (ThreadCreated)
	{
	    CloseThread();
	    delete EvtComplete;
	    delete EvtJob;
		ThreadCreated=false;
	}
}
//---------------------------------------------------------------------------
void TSnap7Client::CloseThread()
{
     int Timeout;

     if (FThread)
     {
          FThread->Terminate();
          if (Job.Pending)
              Timeout=3000;
          else
              Timeout=1000;
          EvtJob->Set();
		  if (FThread->WaitFor(Timeout)!=WAIT_OBJECT_0)
              FThread->Kill();
          try {
             delete FThread;
          }
          catch (...){
          }

          FThread=0;
     }
}
//---------------------------------------------------------------------------
void TSnap7Client::OpenThread()
{
    FThread = new TClientThread(this);
    FThread->Start();
}
//---------------------------------------------------------------------------
int TSnap7Client::Reset(bool DoReconnect)
{
    bool WasConnected = Connected;
    if (ThreadCreated)
	{
		CloseThread();
		Disconnect();
		OpenThread();
	}
	else
		Disconnect();
	
	if (DoReconnect || WasConnected)
       return Connect();
    else
       return 0;
}
//---------------------------------------------------------------------------
void TSnap7Client::DoCompletion()
{
    if ((CliCompletion!=NULL) && !Destroying)
    {
      try{
          CliCompletion(FUsrPtr, Job.Op, Job.Result);
      }catch (...)
	  {
      }
    }
}
//---------------------------------------------------------------------------
int TSnap7Client::SetAsCallback(pfn_CliCompletion pCompletion, void * usrPtr)
{
    CliCompletion=pCompletion;
    FUsrPtr=usrPtr;
    return 0;
}
//---------------------------------------------------------------------------
int TSnap7Client::GetParam(int ParamNumber, void * pValue)
{
    // Actually there are no specific client params, maybe in future...
	return TSnap7MicroClient::GetParam(ParamNumber, pValue);
}
//---------------------------------------------------------------------------
int TSnap7Client::SetParam(int ParamNumber, void * pValue)
{
    // Actually there are no specific client params, maybe in future...
	return TSnap7MicroClient::SetParam(ParamNumber, pValue);
}
//---------------------------------------------------------------------------
bool TSnap7Client::CheckAsCompletion(int &opResult)
{
    if (!Job.Pending)
        opResult=Job.Result;
    else
        if (!Destroying)
            opResult=errCliJobPending; // don't set LastError here
        else
        {
            opResult=errCliDestroying;
            return true;
        }

    return !Job.Pending;
}
//---------------------------------------------------------------------------
int TSnap7Client::AsReadArea(int Area, int DBNumber, int Start, int Amount, int WordLen, void * pUsrData)
{
     if (!Job.Pending)
     {
          Job.Pending  = true;
          Job.Op       = s7opReadArea;
          Job.Area     = Area;
          Job.Number   = DBNumber;
          Job.Start    = Start;
          Job.Amount   = Amount;
          Job.WordLen  = WordLen;
          Job.pData    = pUsrData;
          JobStart     = SysGetTick();
          StartAsyncJob();
          return 0;
     }
     else
          return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7Client::AsWriteArea(int Area, int DBNumber, int Start, int Amount, int WordLen, void * pUsrData)
{
    int ByteSize, TotalSize;

    if (!Job.Pending)
    {
        Job.Pending =true;
        Job.Op      =s7opWriteArea;
        Job.Area    =Area;
        Job.Number  =DBNumber;
        Job.Start   =Start;
        // Performs some check first to copy the data
        ByteSize=DataSizeByte(WordLen);
        TotalSize=ByteSize*Amount; // Total size in bytes
        if (ByteSize==0)
            return SetError(errCliInvalidWordLen);
        if ((TotalSize < 1) || (TotalSize > int(sizeof(opData))))
            return SetError(errCliInvalidParams);
        Job.Amount  =Amount;
        Job.WordLen =WordLen;
        // Doublebuffering
        memcpy(&opData, pUsrData, TotalSize);
        Job.pData =&opData;
        JobStart  =SysGetTick();
        StartAsyncJob();
        return 0;
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7Client::AsListBlocksOfType(int BlockType, PS7BlocksOfType pUsrData, int & ItemsCount)
{
    if (!Job.Pending)
    {
        Job.Pending  =true;
        Job.Op       =s7opListBlocksOfType;
        Job.Area     =BlockType;
        Job.pData    =pUsrData;
        Job.pAmount  =&ItemsCount;
        JobStart     =SysGetTick();
        StartAsyncJob();
        return 0;
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7Client::AsReadSZL(int ID, int Index, PS7SZL pUsrData, int & Size)
{
    if (!Job.Pending)
    {
        Job.Pending  =true;
        Job.Op       =s7opReadSZL;
        Job.ID       =ID;
        Job.Index    =Index;
        Job.pData    =pUsrData;
        Job.pAmount  =&Size;
        Job.Amount   =Size;
        Job.IParam   =1; // Data has to be copied into user buffer
        JobStart     =SysGetTick();
        StartAsyncJob();
        return 0;
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7Client::AsReadSZLList(PS7SZLList pUsrData, int &ItemsCount)
{
    if (!Job.Pending)
    {
        Job.Pending  =true;
        Job.Op       =s7opReadSzlList;
        Job.pData    =pUsrData;
        Job.pAmount  =&ItemsCount;
        Job.Amount   =ItemsCount;
        JobStart     =SysGetTick();
        StartAsyncJob();
        return 0;
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7Client::AsUpload(int BlockType, int BlockNum, void * pUsrData, int & Size)
{
    if (!Job.Pending)
     {
        Job.Pending  =true;
        Job.Op       =s7opUpload;
        Job.Area     =BlockType;
        Job.pData    =pUsrData;
        Job.pAmount  =&Size;
        Job.Amount   =Size;
        Job.Number   =BlockNum;
        Job.IParam   =0; // not full upload, only data
        JobStart     =SysGetTick();
        StartAsyncJob();
        return 0;
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7Client::AsFullUpload(int BlockType, int BlockNum, void * pUsrData, int & Size)
{
    if (!Job.Pending)
    {
        Job.Pending  =true;
        Job.Op       =s7opUpload;
        Job.Area     =BlockType;
        Job.pData    =pUsrData;
        Job.pAmount  =&Size;
        Job.Amount   =Size;
        Job.Number   =BlockNum;
        Job.IParam   =1; // full upload
        JobStart     =SysGetTick();
        StartAsyncJob();
        return 0;
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7Client::AsDownload(int BlockNum, void * pUsrData, int Size)
{
    if (!Job.Pending)
    {
        // Checks the size : here we only need a size>0 to avoid problems during
        // doublebuffering, the real test of the block size will be done in
        // Checkblock.
        if (Size<1)
            return SetError(errCliInvalidBlockSize);
        Job.Pending  =true;
        Job.Op       =s7opDownload;
        // Doublebuffering
        memcpy(&opData, pUsrData, Size);
        Job.Number   =BlockNum;
        Job.Amount   =Size;
        JobStart     =SysGetTick();
        StartAsyncJob();
        return 0;
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7Client::AsCopyRamToRom(int Timeout)
{
    if (!Job.Pending)
    {
        Job.Pending  =true;
        Job.Op       =s7opCopyRamToRom;
        if (Timeout>0)
        {
          Job.IParam   =Timeout;
          JobStart     =SysGetTick();
          StartAsyncJob();
          return 0;
        }
        else
            return SetError(errCliInvalidParams);
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7Client::AsCompress(int Timeout)
{
    if (!Job.Pending)
    {
        Job.Pending  =true;
        Job.Op       =s7opCompress;
        if (Timeout>0)
        {
            Job.IParam   =Timeout;
            JobStart     =SysGetTick();
            StartAsyncJob();
            return 0;
        }
        else
            return SetError(errCliInvalidParams);
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7Client::AsDBRead(int DBNumber, int Start, int Size, void * pUsrData)
{
    return AsReadArea(S7AreaDB, DBNumber, Start, Size, S7WLByte, pUsrData);
}
//---------------------------------------------------------------------------
int TSnap7Client::AsDBWrite(int DBNumber, int Start, int Size, void * pUsrData)
{
    return AsWriteArea(S7AreaDB, DBNumber, Start, Size, S7WLByte, pUsrData);
}
//---------------------------------------------------------------------------
int TSnap7Client::AsMBRead(int Start, int Size, void * pUsrData)
{
    return AsReadArea(S7AreaMK, 0, Start, Size, S7WLByte, pUsrData);
}
//---------------------------------------------------------------------------
int TSnap7Client::AsMBWrite(int Start, int Size, void * pUsrData)
{
    return AsWriteArea(S7AreaMK, 0, Start, Size, S7WLByte, pUsrData);
}
//---------------------------------------------------------------------------
int TSnap7Client::AsEBRead(int Start, int Size, void * pUsrData)
{
    return AsReadArea(S7AreaPE, 0, Start, Size, S7WLByte, pUsrData);
}
//---------------------------------------------------------------------------
int TSnap7Client::AsEBWrite(int Start, int Size, void * pUsrData)
{
    return AsWriteArea(S7AreaPE, 0, Start, Size, S7WLByte, pUsrData);
}
//---------------------------------------------------------------------------
int TSnap7Client::AsABRead(int Start, int Size, void * pUsrData)
{
    return AsReadArea(S7AreaPA, 0, Start, Size, S7WLByte, pUsrData);
}
//---------------------------------------------------------------------------
int TSnap7Client::AsABWrite(int Start, int Size, void * pUsrData)
{
    return AsWriteArea(S7AreaPA, 0, Start, Size, S7WLByte, pUsrData);
}
//---------------------------------------------------------------------------
int TSnap7Client::AsTMRead(int Start, int Amount, void * pUsrData)
{
    return AsReadArea(S7AreaTM, 0, Start, Amount, S7WLTimer, pUsrData);
}
//---------------------------------------------------------------------------
int TSnap7Client::AsTMWrite(int Start, int Amount, void * pUsrData)
{
    return AsWriteArea(S7AreaTM, 0, Start, Amount, S7WLTimer, pUsrData);
}
//---------------------------------------------------------------------------
int TSnap7Client::AsCTRead(int Start, int Amount, void * pUsrData)
{
    return AsReadArea(S7AreaCT, 0, Start, Amount, S7WLCounter, pUsrData);
}
//---------------------------------------------------------------------------
int TSnap7Client::AsCTWrite(int Start, int Amount, void * pUsrData)
{
    return AsWriteArea(S7AreaCT, 0, Start, Amount, S7WLCounter, pUsrData);
}
//---------------------------------------------------------------------------
int TSnap7Client::AsDBGet(int DBNumber, void * pUsrData, int &Size)
{
    if (!Job.Pending)
    {
        if (Size<=0)
            return SetError(errCliInvalidBlockSize);
        Job.Pending  =true;
        Job.Op       =s7opDBGet;
        Job.Number   =DBNumber;
        Job.pData    =pUsrData;
        Job.pAmount  =&Size;
        Job.Amount   =Size;
        JobStart     =SysGetTick();
        StartAsyncJob();
        return 0;
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7Client::AsDBFill(int DBNumber, int FillChar)
{
    if (!Job.Pending)
    {
        Job.Pending  =true;
        Job.Op       =s7opDBFill;
        Job.Number   =DBNumber;
        Job.IParam   =FillChar;
        JobStart     =SysGetTick();
        StartAsyncJob();
        return 0;
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
void TSnap7Client::StartAsyncJob()
{
    ClrError();
	if (!ThreadCreated)
	{
		EvtJob =  new TSnapEvent(false);
		EvtComplete = new TSnapEvent(false);
	    OpenThread();
		ThreadCreated=true;
	}
	EvtComplete->Reset(); // reset if previously was not called WaitAsCompletion
    EvtJob->Set();
}
//---------------------------------------------------------------------------
int TSnap7Client::WaitAsCompletion(unsigned long Timeout)
{
    if (Job.Pending)
    {
        if (ThreadCreated)
		{
			if (EvtComplete->WaitFor(Timeout)==WAIT_OBJECT_0)
				return Job.Result;
			else
			{  
				if (Destroying)
					return errCliDestroying;
				else
					return SetError(errCliJobTimeout);
			}
		}
		else
			return SetError(errCliJobTimeout);
    }
    else
        return Job.Result;
}
//---------------------------------------------------------------------------
void TClientThread::Execute()
{
     while (!Terminated)
     {
          FClient->EvtJob->WaitForever();
          if (!Terminated)
          {
               FClient->PerformOperation();
               FClient->EvtComplete->Set();
               // Notify the caller the end of job (if callback is set)
               FClient->DoCompletion();
          }
     };
}

