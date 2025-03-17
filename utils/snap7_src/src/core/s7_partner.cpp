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
#include "s7_partner.h"
//------------------------------------------------------------------------------

static PServersManager ServersManager = NULL;

//------------------------------------------------------------------------------
int ServersManager_GetServer(longword BindAddress, PConnectionServer &Server)
{
    if (ServersManager == NULL)
    {
        ServersManager = new TServersManager();
    }
    return ServersManager->GetServer(BindAddress, Server);
}
//------------------------------------------------------------------------------
void ServersManager_RemovePartner(PConnectionServer Server, PSnap7Partner Partner)
{
    if (ServersManager != NULL)
    {
        ServersManager->RemovePartner(Server, Partner);
        if (ServersManager->ServersCount==0)
        {
            delete ServersManager;
            ServersManager = NULL;
        }
    }
}
//------------------------------------------------------------------------------
// CONNECTION SERVERS MANAGER
//------------------------------------------------------------------------------
TServersManager::TServersManager()
{
    cs = new TSnapCriticalSection;
    memset(Servers,0,sizeof(Servers));
    ServersCount=0;
}
//---------------------------------------------------------------------------
TServersManager::~TServersManager()
{
	int c;
    Lock();
    if (ServersCount>0)
    {
        for (c = 0; c < MaxAdapters; c++)
        {
            if (Servers[c]!=0)
            {
                delete Servers[c];
                Servers[c]=0;
                ServersCount--;
            }
        }
    }
    Unlock();
    delete cs;
}
//---------------------------------------------------------------------------
void TServersManager::Lock()
{
    cs->Enter();
}
//---------------------------------------------------------------------------
void TServersManager::Unlock()
{
    cs->Leave();
}
//---------------------------------------------------------------------------
void TServersManager::AddServer(PConnectionServer Server)
{
    int c;
    Lock();
    for (c = 0; c < MaxAdapters; c++)
    {
        if (Servers[c]==0)
        {
	    Servers[c]=Server;
            ServersCount++;
            break;
		}
    }
    Unlock();
}
//---------------------------------------------------------------------------
int TServersManager::CreateServer(longword BindAddress, PConnectionServer &Server)
{
    in_addr sin;
    sin.s_addr=BindAddress;
    int Result;

    if (ServersCount<MaxAdapters)
    {
        Server = new TConnectionServer();
        Result=Server->StartTo(inet_ntoa(sin));
        if (Result!=0)
        {
            delete Server;
            Server=0;
            return Result;
        }
        AddServer(Server);
        return 0;
    }
    else
        return errServerNoRoom;
}
//---------------------------------------------------------------------------
int TServersManager::GetServer(longword BindAddress, PConnectionServer &Server)
{
    int c;
    Server=0;
    for (c = 0; c < ServersCount; c++)
    {
        if (Servers[c]->LocalBind==BindAddress)
        {
            Server=Servers[c];
            break;
        }
	}
    if (Server==0)
        return CreateServer(BindAddress, Server);
    else
        return 0;
}
//---------------------------------------------------------------------------
void TServersManager::RemovePartner(PConnectionServer Server, PSnap7Partner Partner)
{
    int c;
    Server->RemovePartner(Partner);
    if (Server->PartnersCount==0)
    {
        Lock();
        for (c = 0; c < MaxAdapters; c++)
        {
            if (Servers[c]==Server)
            {
                Servers[c]=0;
                ServersCount--;
                break;
            }
        }
        Unlock();
        delete Server;
    }
}
//---------------------------------------------------------------------------
// CONNECTION SERVER
//------------------------------------------------------------------------------
void TConnListenerThread::Execute()
{
    socket_t Sock;
    bool Valid;

    while (!Terminated)
    {
	if (FListener->CanRead(FListener->WorkInterval))
	{
		Sock=FListener->SckAccept(); // in any case we must accept
	    Valid=Sock!=INVALID_SOCKET;
	    // check if we are not destroying
	    if ((!Terminated) && (!FServer->Destroying))
	    {
		if (Valid)
		    FServer->Incoming(Sock);
	    }
	    else
		if (Valid)
		    Msg_CloseSocket(Sock);
	};
    }
}
//------------------------------------------------------------------------------
TConnectionServer::TConnectionServer()
{
    cs = new TSnapCriticalSection;
    memset(Partners,0,sizeof(Partners));
    FRunning = false;
    PartnersCount = 0;
}
//------------------------------------------------------------------------------
TConnectionServer::~TConnectionServer()
{
    Stop();
    delete cs;
}
//---------------------------------------------------------------------------
void TConnectionServer::Lock()
{
    cs->Enter();
}
void TConnectionServer::Unlock()
{
    cs->Leave();
}
//---------------------------------------------------------------------------
int TConnectionServer::Start()
{
    int Result;
    // Creates the listener
    SockListener = new TMsgSocket();
    strncpy(SockListener->LocalAddress,FLocalAddress,16);
    SockListener->LocalPort=isoTcpPort;
    // Binds
    Result=SockListener->SckBind();
    if (Result==0)
    {
        LocalBind=SockListener->LocalBind;
        // Listen
        Result=SockListener->SckListen();
        if (Result==0)
        {
            // Creates the Listener thread
            ServerThread = new TConnListenerThread(SockListener, this);
            ServerThread->Start();
        }
        else
            delete SockListener;
    }
    else
        delete SockListener;

    FRunning=Result==0;
    return Result;
}
//---------------------------------------------------------------------------
int TConnectionServer::StartTo(const char *Address)
{
    strncpy(FLocalAddress,Address,16);
    return Start();
}
//---------------------------------------------------------------------------
void TConnectionServer::Stop()
{
    if (FRunning)
    {
		// Kills the listener thread
        ServerThread->Terminate();
        if (ServerThread->WaitFor(csTimeout)!=WAIT_OBJECT_0)
           ServerThread->Kill();
        delete ServerThread;
        // Kills the listener
        delete SockListener;
        FRunning = false;
    }
}
//---------------------------------------------------------------------------
PSnap7Partner TConnectionServer::FindPartner(longword Address)
{
    int c;
    PSnap7Partner Result;
    for (c = 0; c < MaxPartners; c++)
    {
        Result=Partners[c];
        if (Result!=NULL)
        {
           if (Result->PeerAddress==Address)
               return Result;
        }
    }
    return NULL;
}
//------------------------------------------------------------------------------
int TConnectionServer::FirstFree()
{
    int i;
    for (i = 0; i < MaxPartners; i++)
    {
        if (Partners[i]==0)
	    return i;
    }
    return -1;
}
//------------------------------------------------------------------------------
int TConnectionServer::RegisterPartner(PSnap7Partner Partner)
{
    PSnap7Partner aPartner;
    int idx;
    // check if already exists a passive partner linked to the same peer address
    aPartner=FindPartner(Partner->PeerAddress);
    if (aPartner==NULL)
    {
       Lock();
       idx=FirstFree();
       if (idx>=0)
       {
           Partners[idx]=Partner;
           PartnersCount++;
       }
       Unlock();
       if (idx>=0)
           return 0;
       else
           return errParNoRoom;
    }
    else
       return errParAddressInUse;
}
//------------------------------------------------------------------------------
void TConnectionServer::RemovePartner(PSnap7Partner Partner)
{
    int c;
    Lock();
	for (c = 0; c < MaxPartners; c++)
    {
        if (Partners[c]==Partner)
        {
            Partners[c]=0;
            PartnersCount--;
            break;
        }
    }
    Unlock();
}
//------------------------------------------------------------------------------
void TConnectionServer::Incoming(socket_t Sock)
{
    longword Address;
    PSnap7Partner Partner;

    Address=Msg_GetSockAddr(Sock);
    // Looks for a partner that is waiting for a connection from this address
    Lock();
    Partner=FindPartner(Address);
    Unlock();
    // if partner exists must not be already connected : a partner can be connected
    // with only one peer at time
    if ((Partner!=NULL) && (!Partner->Stopping) && (!Partner->Connected))
        Partner->SetSocket(Sock);
    else
        Msg_CloseSocket(Sock); // we are not interested
}
//------------------------------------------------------------------------------
// PARTHER THREAD
//------------------------------------------------------------------------------
void TPartnerThread::Execute()
{
    longword TheTime;

    FKaElapsed=SysGetTick();
    while ((!Terminated) && (!FPartner->Destroying))
    {
        // Check connection
        while (!Terminated && !FPartner->Connected && !FPartner->Destroying)
        {
            if (!FPartner->ConnectToPeer())
                SysSleep(FRecoveryTime);
        }
        // Execution
        if ((!Terminated) && (!FPartner->Destroying) && (!FPartner->Execute()))
			SysSleep(FRecoveryTime);
        // Keep Alive
        if (!Terminated && (!FPartner->Destroying) && FPartner->Active && FPartner->Connected)
		{
             TheTime=SysGetTick();
             if (TheTime-FKaElapsed>FPartner->KeepAliveTime)
             {
                 FKaElapsed=TheTime;
                 if (!FPartner->Ping(FPartner->RemoteAddress))
                     FPartner->Disconnect();
             };
        };
    };
}
//------------------------------------------------------------------------------
// S7 PARTNER
//------------------------------------------------------------------------------
TSnap7Partner::TSnap7Partner(bool CreateActive)
{
    // We skip RFC/ISO header, our PDU is the ISO payload
    PDUH_in=PS7ReqHeader(&PDU.Payload);
    FWorkerThread=0;
    OnBRecv = 0;
    OnBSend = 0;
    Active=CreateActive;
    SendEvt = new TSnapEvent(true);
    RecvEvt = new TSnapEvent(true);
    FSendPending = false;
    FRecvPending = false;
    memset(&FRecvStatus,0,sizeof(TRecvStatus));
    memset(&FRecvLast,0,sizeof(TRecvLast));
    FSendElapsed  = 0;
	Destroying    = false;
    // public
    Linked        =false;
    Running       =false;
    BindError     =false;
    BRecvTimeout  =3000;
    BSendTimeout  =3000;
    RecoveryTime  =500;
    KeepAliveTime =5000;
    NextByte      =0;
    PeerAddress   =0;
    SendTime      =0;
    RecvTime      =0;
    BytesSent     =0;
    BytesRecv     =0;
    SendErrors    =0;
    RecvErrors    =0;
}
//------------------------------------------------------------------------------
TSnap7Partner::~TSnap7Partner()
{
    Stop();
    OnBRecv = 0;
    OnBSend = 0;
    delete SendEvt;
    delete RecvEvt;
}
//------------------------------------------------------------------------------
byte TSnap7Partner::GetNextByte()
{
    NextByte++;
    if (NextByte==0xFF)
       NextByte=1;
    return NextByte;
}
//------------------------------------------------------------------------------
int TSnap7Partner::Start()
{
    int Result;
    PeerAddress=inet_addr(RemoteAddress);
    SrcAddress =inet_addr(LocalAddress);
    if (!Running)
    {
      if (!Active)
      {
          Result=ServersManager_GetServer(SrcAddress,FServer);
          if (Result==0)
              FServer->RegisterPartner(this);
          BindError=Result!=0;
      }
      else
      {
          Linked=PeerConnect()==0;
          Result=0; // we need to create the worker thread even tough it's not linked
      };
     // if ok create the worker thread
     if (Result==0)
     {
         FWorkerThread = new TPartnerThread(this, RecoveryTime);
         FWorkerThread->Start();
     }
    }
    else
        Result=0;

    Running=Result==0;

    return Result;
}
//------------------------------------------------------------------------------
int TSnap7Partner::StartTo(const char *LocAddress, const char *RemAddress, word LocTsap, word RemTsap)
{
    SrcTSap=LocTsap;
    DstTSap=RemTsap;
	strcpy(LocalAddress,LocAddress);
	strcpy(RemoteAddress,RemAddress);
    return Start();
}
//------------------------------------------------------------------------------
int TSnap7Partner::Stop()
{
    if (Running)
    {
        Stopping=true; // to prevent incoming connections
        CloseWorker();
        if (!Active && (FServer!=0))
            ServersManager_RemovePartner(FServer, this);
        if (Connected)
            Disconnect();
        Running =false;
        Stopping=false;
    };
    BindError=false;
    return 0;
}
//------------------------------------------------------------------------------
void TSnap7Partner::Disconnect()
{
    PeerDisconnect();
    Linked=false;
}
//------------------------------------------------------------------------------
int TSnap7Partner::GetParam(int ParamNumber, void * pValue)
{
	switch (ParamNumber)
	{
		case p_u16_LocalPort:
			*Puint16_t(pValue)=LocalPort;
			break;
		case p_u16_RemotePort:
			*Puint16_t(pValue)=RemotePort;
			break;
		case p_i32_PingTimeout:
			*Pint32_t(pValue)=PingTimeout;
			break;
		case p_i32_SendTimeout:
			*Pint32_t(pValue)=SendTimeout;
			break;
		case p_i32_RecvTimeout:     
			*Pint32_t(pValue)=RecvTimeout;
			break;
		case p_i32_WorkInterval:
			*Pint32_t(pValue)=WorkInterval;
			break;
		case p_u16_SrcRef:
			*Puint16_t(pValue)=SrcRef;
			break;
		case p_u16_DstRef:
			*Puint16_t(pValue)=DstRef;
			break;
		case p_u16_SrcTSap:
			*Puint16_t(pValue)=SrcTSap;
			break;
		case p_i32_PDURequest:
			*Pint32_t(pValue)=PDURequest;
			break;
		case p_i32_BSendTimeout:
			*Pint32_t(pValue)=BSendTimeout;
			break;
		case p_i32_BRecvTimeout:    
			*Pint32_t(pValue)=BRecvTimeout;
			break;
		case p_u32_RecoveryTime:    
			*Puint32_t(pValue)=RecoveryTime;
			break;
		case p_u32_KeepAliveTime:   
			*Puint32_t(pValue)=KeepAliveTime;
			break;
		default: return errParInvalidParamNumber;
	}
	return 0;
}
//------------------------------------------------------------------------------
int TSnap7Partner::SetParam(int ParamNumber, void * pValue)
{
	switch (ParamNumber)
	{
		case p_u16_RemotePort:
			if (!Connected && Active)
				RemotePort=*Puint16_t(pValue);
			else
				return errParCannotChangeParam;
			break;
		case p_i32_PingTimeout:
			PingTimeout=*Pint32_t(pValue);
			break;
		case p_i32_SendTimeout:
			SendTimeout=*Pint32_t(pValue);
			break;
		case p_i32_RecvTimeout:     
			RecvTimeout=*Pint32_t(pValue);
			break;
		case p_i32_WorkInterval:
			WorkInterval=*Pint32_t(pValue);
			break;
		case p_u16_SrcRef:
			SrcRef=*Puint16_t(pValue);
			break;
		case p_u16_DstRef:
			DstRef=*Puint16_t(pValue);
			break;
		case p_u16_SrcTSap:
			SrcTSap=*Puint16_t(pValue);
			break;
		case p_i32_PDURequest:
			PDURequest=*Pint32_t(pValue);
			break;
		case p_i32_BSendTimeout:
			BSendTimeout=*Pint32_t(pValue);
			break;
		case p_i32_BRecvTimeout:    
			BRecvTimeout=*Pint32_t(pValue);
			break;
		case p_u32_RecoveryTime:    
			RecoveryTime=*Puint32_t(pValue);
			break;
		case p_u32_KeepAliveTime:   
			KeepAliveTime=*Puint32_t(pValue);
			break;
		default: return errParInvalidParamNumber;
	}
	return 0;
}
//------------------------------------------------------------------------------
void TSnap7Partner::ClearRecv()
{
    memset(&FRecvStatus,0,sizeof(TRecvStatus));
    FRecvPending=false;
}
//------------------------------------------------------------------------------
bool TSnap7Partner::ConnectToPeer()
{
    bool Result;
    if (Active)
    {
        Result=PeerConnect()==0;  // try to Connect
        Linked=Result;
    }
    else
        Result =false;     // nothing : we are waiting for a connection

    return Result;
}
//------------------------------------------------------------------------------
bool TSnap7Partner::PerformFunctionNegotiate()
{
    PReqFunNegotiateParams ReqParams;
    PResFunNegotiateParams ResParams;
    TS7Answer23 Answer;
    int Size;

    // Setup pointers
    ReqParams=PReqFunNegotiateParams(pbyte(PDUH_in)+sizeof(TS7ReqHeader));
    ResParams=PResFunNegotiateParams(pbyte(&Answer)+sizeof(TS7ResHeader23));
    // We are here only because we found a PduType_request, the partner can only
    // handle Bs} requests and pdu negotiation requests.
    // So, now we must check the incoming function
    if (ReqParams->FunNegotiate!=pduNegotiate)
    {
        LastError=errParInvalidPDU;
        return false;
    };
    // Prepares the answer
    Answer.Header.P=0x32;
    Answer.Header.PDUType =0x03;
    Answer.Header.AB_EX   =0x0000;
    Answer.Header.Sequence=PDUH_in->Sequence;
    Answer.Header.ParLen  =SwapWord(sizeof(TResFunNegotiateParams));
    Answer.Header.DataLen =0x0000;
    Answer.Header.Error   =0x0000;
    // Params point at the } of the header
    ResParams->FunNegotiate=pduNegotiate;
    ResParams->Unknown=0x0;
    // Checks PDU request length
    if (SwapWord(ResParams->PDULength)>IsoPayload_Size)
        ResParams->PDULength=SwapWord(IsoPayload_Size);
    else
        ResParams->PDULength=ReqParams->PDULength;
    // We offer the same
    ResParams->ParallelJobs_1=ReqParams->ParallelJobs_1;
    ResParams->ParallelJobs_2=ReqParams->ParallelJobs_2;
    // And store the value
    PDULength=SwapWord(ResParams->PDULength);
    // Sends the answer
    Size=sizeof(TS7ResHeader23) + sizeof(TResFunNegotiateParams);
    if (isoSendBuffer(&Answer, Size)!=0)
        SetError(errParNegotiatingPDU);

    Linked=LastError==0;
    return Linked;
}
//------------------------------------------------------------------------------
void TSnap7Partner::CloseWorker()
{
     int Timeout;
     if (FWorkerThread)
     {
          FWorkerThread->Terminate();
          if (FRecvPending || FSendPending)
             Timeout=3000;
          else
             Timeout=1000;

          if (FWorkerThread->WaitFor(Timeout)!=WAIT_OBJECT_0)
             FWorkerThread->Kill();
          try {
             delete FWorkerThread;
          }
          catch (...){
          }
          FWorkerThread=0;
     }
}
//------------------------------------------------------------------------------
bool TSnap7Partner::BlockSend()
{
    PBSendReqParams ReqParams;
    PBSendReqParams ResParams;
    PBsendRequestData DataSendReq;
    int TotalSize;
    int SentSize;
    int Slice;
    int MaxSlice;
    uintptr_t Offset;
    pbyte Source;
    bool First, Last;
    byte Seq_IN;
    int TxIsoSize;
    pbyte Data;
    pword TotalPackSize;
    int DataPtrOffset;
    word Extra;

    ClrError();
    TotalSize=TxBuffer.Size;
    SentSize =TotalSize;
    Offset=0;
    First =true;
    Seq_IN=0x00;

  // With BSend we can transfer up to 32k (S7300) or 64k (S7400), but splitted
  // into slice that cannot exced the PDU size negotiated (including various headers).
    MaxSlice=PDULength-sizeof(TS7ReqHeader)-sizeof(TBSendParams)-sizeof(TBsendRequestData)-2;

    ReqParams=PBSendReqParams(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
    ResParams=ReqParams; // pdu 7 is symmetrical

    while ((TotalSize>0) && (LastError==0))
    {
		Source=pbyte(&TxBuffer.Data)+Offset;
		Slice=TotalSize;

		if (Slice>MaxSlice)
			Slice=MaxSlice;

		TotalSize-=Slice;
		Offset+=Slice;
		Last=TotalSize==0;

		// Prepare send
		DataPtrOffset=sizeof(TS7ReqHeader)+sizeof(TBSendParams);
		// Header
		PDUH_out->P=0x32;                     // Always 0x32
		PDUH_out->PDUType=PduType_userdata;  // 7
		PDUH_out->AB_EX=0x0000;               // Always 0x0000
		PDUH_out->Sequence=GetNextWord();      // Autoinc
		PDUH_out->ParLen=SwapWord(sizeof(TBSendParams)); // 16 bytes

		ReqParams->Head[0]=0x00;
		ReqParams->Head[1]=0x01;
		ReqParams->Head[2]=0x12;
		ReqParams->Plen   =0x08; // length from here up the end of the record
		ReqParams->Uk     =0x12;
		ReqParams->Tg     =grBSend; // 0x46
		ReqParams->SubFun =0x01;
		ReqParams->Seq    =Seq_IN;
		ReqParams->Err    =0x0000;
		if (Last)
			ReqParams->EoS  =0x00;
		else
			ReqParams->EoS  =0x01;
		// Next byte is auto inc and not zero for partial sequences
		// Is zero for lonely sequences.
		if (First && Last)
			ReqParams->IDSeq=0x00;
		else
			ReqParams->IDSeq=GetNextByte();

		DataSendReq=PBsendRequestData(pbyte(PDUH_out)+DataPtrOffset);
		if (First)
		{
			// in the first pdu, after data header there is the whole packet length
			TotalPackSize=pword(pbyte(DataSendReq)+sizeof(TBsendRequestData));
			Data=pbyte(TotalPackSize)+sizeof(word);
			*TotalPackSize=SwapWord(word(TxBuffer.Size));
			Extra=2; // extra bytes (total pack size indicator)
		}
		else
		{
			Data=pbyte(DataSendReq)+sizeof(TBsendRequestData);
			Extra=0;
		};

		PDUH_out->DataLen=SwapWord(word(sizeof(TBsendRequestData))+Slice+Extra);
		DataSendReq->Len =SwapWord(Slice+8+Extra);
		TxIsoSize=Slice+sizeof(TS7ReqHeader)+sizeof(TBSendParams)+sizeof(TBsendRequestData)+Extra;

		DataSendReq->FF      =0xFF;
		DataSendReq->TRSize  =TS_ResOctet;
		DataSendReq->DHead[0]=0x12;
		DataSendReq->DHead[1]=0x06;
		DataSendReq->DHead[2]=0x13;
		DataSendReq->DHead[3]=0x00;
		DataSendReq->R_ID    =SwapDWord(TxBuffer.R_ID);
		memcpy(Data, Source ,Slice);

		if (isoExchangeBuffer(NULL, TxIsoSize)!=0)
			SetError(errParSendingBlock);

		if (LastError==0)
		{
		   Seq_IN=ResParams->Seq;
		   if (SwapWord(ResParams->Err)!=0)
			   LastError=errParSendRefused;
		}

		if (First)
		{
			First =false;
			MaxSlice+=2; // only in the first frame we have the extra info
		};
	};

	SendTime=SysGetTick()-FSendElapsed;
	if (LastError==0)
		BytesSent+=SentSize;

	return LastError==0;
}
//------------------------------------------------------------------------------
bool TSnap7Partner::PickData()
{
	PBSendReqParams   ReqParams;
	PBSendReqParams   ResParams;
	PBSendResData     ResData;
	PBsendRequestData DataSendReq;
	pbyte Source, Target;
	pword TotalPackSize;
	word Slice;
	int AnswerLen;

	ClrError();
	// Setup pointers
	ReqParams  =PBSendReqParams(pbyte(PDUH_in)+sizeof(TS7ReqHeader));
	ResParams  =ReqParams; // pdu 7 is symmetrical
	DataSendReq=PBsendRequestData(pbyte(ReqParams)+sizeof(TBSendParams));

	// Checks if PDU is a BSend request
	if ((PDUH_in->PDUType!=PduType_userdata) || (ReqParams->Tg!=grBSend))
	{
		LastError=errParInvalidPDU;
		return false;
	}

	if (FRecvStatus.First)
	{
		TotalPackSize=(word*)(pbyte(DataSendReq)+sizeof(TBsendRequestData));
		FRecvStatus.TotalLength=SwapWord(*TotalPackSize);
		Source=pbyte(DataSendReq)+sizeof(TBsendRequestData)+2;
		FRecvStatus.In_R_ID=SwapDWord(DataSendReq->R_ID);
		FRecvStatus.Offset=0;
		Slice=SwapWord(DataSendReq->Len)-10;
	}
	else {
		Slice=SwapWord(DataSendReq->Len)-8;
		Source=pbyte(DataSendReq)+sizeof(TBsendRequestData);
	}

	FRecvStatus.Done=ReqParams->EoS==0x00;

	Target=pbyte(&RxBuffer)+FRecvStatus.Offset;
	memcpy(Target, Source, Slice);
	FRecvStatus.Offset+=Slice;

	ResData =PBSendResData(pbyte(ResParams)+sizeof(TBSendParams));
	// Send Answer
	PDUH_out->ParLen  =SwapWord(sizeof(TBSendParams));
	PDUH_out->DataLen =SwapWord(sizeof(TBSendResData));

	ResParams->Head[0]=0x00;
	ResParams->Head[1]=0x01;
	ResParams->Head[2]=0x12;
	ResParams->Plen   =0x08; // length from here up the end of the record
	ResParams->Uk     =0x12;
	ResParams->Tg     =0x86;
	ResParams->SubFun =0x01;
	ResParams->Seq    =FRecvStatus.Seq_Out;
	ResParams->Err    =0x0000;
	ResParams->EoS    =0x00;
	ResParams->IDSeq  =0x00;

	ResData->DHead[0] =0x0A;
	ResData->DHead[1] =0x00;
	ResData->DHead[2] =0x00;
	ResData->DHead[3] =0x00;

	AnswerLen=sizeof(TS7ReqHeader)+sizeof(TBSendParams)+sizeof(TBSendResData);
	if (isoSendBuffer(NULL,AnswerLen)!=0)
		SetError(errParRecvingBlock);

	return LastError==0;
}
//------------------------------------------------------------------------------
bool TSnap7Partner::BlockRecv()
{
	bool Result;
    if (!FRecvPending) // Start sequence
    {
        FRecvPending=true;
        FRecvStatus.First=true;
        FRecvStatus.Done =false;
        FRecvStatus.Seq_Out =GetNextByte();
        FRecvStatus.Elapsed =SysGetTick();
        FRecvLast.Done=false;
        FRecvLast.Result=0;
        FRecvLast.R_ID=0;
        FRecvLast.Size=0;
        RecvTime =0;
        FRecvLast.Count++;
        if (FRecvLast.Count==0xFFFFFFFF)
          FRecvLast.Count=0;
    };

	Result=PickData();
    FRecvStatus.First=false;

    if (!Result || FRecvStatus.Done)
    {
        FRecvLast.Result=LastError;
        if (Result)
        {
            BytesRecv+=FRecvStatus.TotalLength;
            RecvTime=SysGetTick()-FRecvStatus.Elapsed;
            FRecvLast.R_ID=FRecvStatus.In_R_ID;
            FRecvLast.Size=FRecvStatus.TotalLength;
        };
        RecvEvt->Set();
        if ((OnBRecv!=NULL) && !Destroying)
            OnBRecv(FRecvUsrPtr, FRecvLast.Result, FRecvLast.R_ID, &RxBuffer, FRecvLast.Size);
        FRecvLast.Done=true;
        ClearRecv();
    };
    return Result;
}
//------------------------------------------------------------------------------
bool TSnap7Partner::ConnectionConfirm()
{
    if (FRecvPending)
        ClearRecv();
    IsoConfirmConnection(pdu_type_CC); // <- Connection confirm
    return LastTcpError!=WSAECONNRESET;
}
//------------------------------------------------------------------------------
int TSnap7Partner::Status()
{
    if (Running)
    {
        if (Linked)
        {
            if (FRecvPending)
                return par_receiving;
            else
                if (FSendPending)
                    return par_sending;
                else
                    return par_linked;
        }
        else
            if (Active)
                return par_connecting;
            else
                return par_waiting;
    }
    else{
        if ((!Active) && BindError)
            return par_binderror;
        else
            return par_stopped;
    }
}
//------------------------------------------------------------------------------
bool TSnap7Partner::Execute()
{
    TPDUKind PduKind;
    bool RTimeout;
    bool Result =true;

    // Checks if there is something to send (and we are not receiving...)
    if (FSendPending && !FRecvPending)
    {
        Result=BlockSend();
        SendEvt->Set();
        if ((OnBSend!=NULL) && (!Destroying))
            OnBSend(FSendUsrPtr, LastError);
        FSendPending=false;
    }

	if (Destroying)
		return false;

    // Checks if there is something to recv
    if (Result && CanRead(WorkInterval))
    {
        // Peeks info and returns PDU Kind
        isoRecvPDU(&PDU);
        if (LastTcpError==0)
        {
            // First check valid data incoming (most likely situation)
            IsoPeek(&PDU,PduKind);
            if (PduKind==pkValidData)
            {
                if (PDUH_in->PDUType==PduType_request)
                {
                    if (FRecvPending)
                        ClearRecv();
                    Result=PerformFunctionNegotiate();
                }
                else // Pdu type userdata
                    Result=BlockRecv();
            }
            else
                if (PduKind==pkConnectionRequest)
                    Result=ConnectionConfirm();
                else // nothing else
                    Purge();
        }
        else
            Result=false;
    };

    if (LastTcpError==WSAECONNRESET)
    {
        Result=false;
        Linked=false;
    }
    else
        if (!Result)
            Disconnect();

    // Check BRecv sequence timeout
    RTimeout= FRecvPending && (SysGetTick()-FRecvStatus.Elapsed>longword(BRecvTimeout));

    if (RTimeout)
    {
        LastError=errParFrameTimeout;
        RecvEvt->Set();
        if ((OnBRecv!=NULL) && !Destroying)
            OnBRecv(FRecvUsrPtr, LastError, 0, &RxBuffer,0);
    };

  if (!Result || RTimeout)
      ClearRecv();   // parframetimeout

  return Result;
}

//------------------------------------------------------------------------------
int TSnap7Partner::BSend(longword R_ID, void *pUsrData, int Size)
{
    // The block send is managed into the worker thread.
    // Sync Bsend consists of AsBSend+WaitAsCompletion
    int Result=AsBSend(R_ID, pUsrData, Size);
    if (Result==0)
        Result=WaitAsBSendCompletion(BSendTimeout);
    return Result;
}
//------------------------------------------------------------------------------
int TSnap7Partner::AsBSend(longword R_ID, void *pUsrData, int Size)
{
    SendTime=0;
    if (Linked)
    {
      if (!FSendPending)
      {
          memcpy(&TxBuffer.Data, pUsrData, Size);
          TxBuffer.R_ID=R_ID;
          TxBuffer.Size=Size;
          SendEvt->Reset();
          FSendPending=true;
          FSendElapsed=SysGetTick();
          return 0;
      }
      else
          return errParBusy;
    }
    else
        return SetError(errParNotLinked);
}
//------------------------------------------------------------------------------
bool TSnap7Partner::CheckAsBSendCompletion(int &opResult)
{
    if (!Destroying)
	{
		if (!FSendPending)
			opResult=LastError;
		else
			opResult=errParBusy;

		return !FSendPending;
	}
	else
	{
		opResult=errParDestroying;
		return true;
    }

}
//------------------------------------------------------------------------------
int TSnap7Partner::WaitAsBSendCompletion(longword Timeout)
{
   if (SendEvt->WaitFor(BSendTimeout)==WAIT_OBJECT_0)
   {
		if (!Destroying)
			return LastError;
		else
			return SetError(errParDestroying);
   }
   else
       return SetError(errParSendTimeout);
}
//------------------------------------------------------------------------------
int TSnap7Partner::SetSendCallback(pfn_ParBSendCompletion pCompletion, void *usrPtr)
{
    OnBSend=pCompletion;
    FSendUsrPtr=usrPtr;
    return 0;
}
//------------------------------------------------------------------------------
int TSnap7Partner::BRecv(longword &R_ID, void *pData, int &Size, longword Timeout)
{
     int Result=0;
     if (RecvEvt->WaitFor(Timeout)==WAIT_OBJECT_0)
     {
         R_ID =FRecvLast.R_ID;
         Size =FRecvLast.Size;
         if (FRecvLast.Result==0)
         {
             if (pData!=NULL)
                 memcpy(pData, &RxBuffer, Size);
             else
                 Result=errParInvalidParams;
         }
         else
             Result=FRecvLast.Result;
         RecvEvt->Reset();
     }
     else
         Result=errParRecvTimeout;

     return SetError(Result);
}
//------------------------------------------------------------------------------
bool TSnap7Partner::CheckAsBRecvCompletion(int &opResult, longword &R_ID,
    void *pData, int &Size)
{
    if (Destroying)
	{
		Size=0;
		opResult=errParDestroying;
		return true;
	}
	
	bool Result=FRecvLast.Done;
    if (Result)
    {
        Size=FRecvLast.Size;
        R_ID=FRecvLast.R_ID;
        opResult=FRecvLast.Result;
        if ((pData!=NULL) && (Size>0) && (opResult==0))
           memcpy(pData, &RxBuffer, Size);
        FRecvLast.Done=false;
    }
    return Result;
}
//------------------------------------------------------------------------------
int TSnap7Partner::SetRecvCallback(pfn_ParBRecvCallBack pCompletion, void *usrPtr)
{
    OnBRecv=pCompletion;
    FRecvUsrPtr=usrPtr;
    return 0;
}
//------------------------------------------------------------------------------

