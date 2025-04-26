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
#include "s7_server.h"
#include "s7_firmware.h"

const byte BitMask[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

//------------------------------------------------------------------------------
// ISO/TCP WORKER  CLASS
//------------------------------------------------------------------------------
bool TIsoTcpWorker::IsoPerformCommand(int &Size)
{
    return true;
}
//---------------------------------------------------------------------------
bool TIsoTcpWorker::ExecuteSend()
{
    return true;
}
//---------------------------------------------------------------------------
bool TIsoTcpWorker::ExecuteRecv()
{
    TPDUKind PduKind;
    int PayloadSize;

    if (CanRead(WorkInterval)) // should be Small to avoid time wait during the close
    {
        isoRecvPDU(&PDU);
        if (LastTcpError==0)
        {
            IsoPeek(&PDU,PduKind);
            // First check valid data incoming (most likely situation)
            if (PduKind==pkValidData)
            {
                PayloadSize=PDUSize(&PDU)-DataHeaderSize;
                return IsoPerformCommand(PayloadSize);
            };
            // Connection request incoming
            if (PduKind==pkConnectionRequest)
            {
                IsoConfirmConnection(pdu_type_CC); // <- Connection confirm
                return LastTcpError!=WSAECONNRESET;
            };
            // Disconnect request incoming (only for isotcp full complient equipment, not S7)
            if (PduKind==pkDisconnectRequest)
            {
                IsoConfirmConnection(pdu_type_DC); // <- Disconnect confirm
                return false;
            };
            // Empty fragment, maybe an ACK
            if (PduKind==pkEmptyFragment)
            {
                PayloadSize=0;
                return IsoPerformCommand(PayloadSize);
            };
            // Valid PDU format but we have to discard it
            if (PduKind==pkUnrecognizedType)
            {
                return LastTcpError!=WSAECONNRESET;
            };
            // Here we have an Invalid PDU
            Purge();
            return true;
        }
        else
            return LastTcpError!=WSAECONNRESET;
    }
    else
        return true;
}
//---------------------------------------------------------------------------
bool TIsoTcpWorker::Execute()
{
    return ExecuteSend() && ExecuteRecv();
}
//------------------------------------------------------------------------------
// S7 WORKER CLASS
//------------------------------------------------------------------------------
TS7Worker::TS7Worker()
{
    // We skip RFC/ISO header, our PDU is the payload
    PDUH_in   =PS7ReqHeader(&PDU.Payload);
    FPDULength=2048;
    DBCnt     =0;
    LastBlk   =Block_DB;
}

bool TS7Worker::ExecuteRecv()
{
    WorkInterval=FServer->WorkInterval;
    return TIsoTcpWorker::ExecuteRecv();
}
//------------------------------------------------------------------------------
bool TS7Worker::CheckPDU_in(int PayloadSize)
{
    // Checks the size : packet size must match with header infos
    int Size=SwapWord(PDUH_in->ParLen)+SwapWord(PDUH_in->DataLen)+ReqHeaderSize;
    if (Size!=PayloadSize)
        return false;
    // Checks PDUType : must be 1 or 7
    if ((PDUH_in->PDUType!=PduType_request) &&
        (PDUH_in->PDUType!=PduType_userdata))
        return false;
    else
        return true;
}
//------------------------------------------------------------------------------
byte TS7Worker::BCD(word Value)
{
    return ((Value / 10) << 4) + (Value % 10);
}
//------------------------------------------------------------------------------
void TS7Worker::FillTime(PS7Time PTime)
{
    time_t Now;
    time(&Now);
    struct tm *DT = localtime (&Now);

    PTime->bcd_year=BCD(DT->tm_year-100);
    PTime->bcd_mon =BCD(DT->tm_mon+1);
    PTime->bcd_day =BCD(DT->tm_mday);
    PTime->bcd_hour=BCD(DT->tm_hour);
    PTime->bcd_min =BCD(DT->tm_min);
    PTime->bcd_sec =BCD(DT->tm_sec);
    PTime->bcd_himsec=0;
    PTime->bcd_dow =BCD(DT->tm_wday);
}
//------------------------------------------------------------------------------
void TS7Worker::DoEvent(longword Code, word RetCode, word Param1, word Param2,
  word Param3, word Param4)
{
    FServer->DoEvent(ClientHandle,Code,RetCode,Param1,Param2,Param3,Param4);
}
//------------------------------------------------------------------------------
void TS7Worker::DoReadEvent(longword Code, word RetCode, word Param1, word Param2,
  word Param3, word Param4)
{
    FServer->DoReadEvent(ClientHandle,Code,RetCode,Param1,Param2,Param3,Param4);
}
//------------------------------------------------------------------------------
void TS7Worker::FragmentSkipped(int Size)
{
// do nothing could be used for debug purpose
}
//------------------------------------------------------------------------------
bool TS7Worker::IsoPerformCommand(int &Size)
{
    // Checks for Ack fragment
    if (Size==0)
        return PerformPDUAck(Size);
    // First checks PDU consistence
    if (CheckPDU_in(Size))
    {
        switch (PDUH_in->PDUType)
        {
            case PduType_request    : return PerformPDURequest(Size);
            case PduType_userdata   : return PerformPDUUsrData(Size);
        }
    }
    else
        DoEvent(evcPDUincoming, evrMalformedPDU, Size, 0, 0, 0);
    return false;
}
//------------------------------------------------------------------------------
bool TS7Worker::PerformPDUAck(int &Size)
{
    // here we could keep track of ack empty fragment for debug purpose
    return true;
}
//------------------------------------------------------------------------------
bool TS7Worker::PerformPDURequest(int &Size)
{
    pbyte P;
    byte PDUFun;
    bool Result = true;

    // We have to store PDUfun since it will be overwritten
    P = pbyte(PDUH_in)+ReqHeaderSize;
    PDUFun=*P;
    // Watches the function
    switch (PDUFun)
    {
        case pduFuncRead    : Result=PerformFunctionRead();
             break;
        case pduFuncWrite   : Result=PerformFunctionWrite();
             break;
		case pduNegotiate   : Result=PerformFunctionNegotiate();
             break;
        case pduStart       :
        case pduStop        : Result=PerformFunctionControl(PDUFun);
             break;
		case pduStartUpload :
        case pduUpload      :
        case pduEndUpload   : Result=PerformFunctionUpload();
             break;
        case pduReqDownload : Result=PerformFunctionDownload();
             break;
        // <-- Further (custom) functions can be added here
        default:
             DoEvent(evcPDUincoming, evrCannotHandlePDU, Size, 0, 0, 0);
    };
    return Result;
}
//------------------------------------------------------------------------------
bool TS7Worker::PerformPDUUsrData(int &Size)
{
    PS7ReqParams7 ReqParams;
    byte Tg, SubFun;
    bool Result = true;
    // Set Pointer to request params
    ReqParams=PS7ReqParams7(pbyte(PDUH_in)+ReqHeaderSize);
    Tg=ReqParams->Tg;
    SubFun=ReqParams->SubFun;
    // Switch type_group
    switch (Tg)
    {
      case grProgrammer : Result=PerformGroupProgrammer();
          break;
      case grCyclicData : Result=PerformGroupCyclicData();
          break;
      case grBlocksInfo : Result=PerformGroupBlockInfo();
          break;
      case grSZL        : Result=PerformGroupSZL();
          break;
      case grPassword   : Result=PerformGroupSecurity();
          break;
      case grClock      : switch (SubFun)
                         {
                             case 0x01 : Result=PerformGetClock();
                                 break;
                             case 0x02 : Result=PerformSetClock();
                                 break;
                         };
                         break;
      default:
          DoEvent(evcPDUincoming, evrInvalidGroupUData, Tg, 0, 0, 0);
    };
    return Result;
}
//------------------------------------------------------------------------------
int TS7Worker::DataSizeByte(int WordLength)
{
	switch (WordLength){
		case S7WLBit     : return 1;  // S7 sends 1 byte per bit
		case S7WLByte    : return 1;
		case S7WLChar    : return 1;
		case S7WLWord    : return 2;
		case S7WLDWord   : return 4;
		case S7WLInt     : return 2;
		case S7WLDInt    : return 4;
		case S7WLReal    : return 4;
		case S7WLCounter : return 2;
		case S7WLTimer   : return 2;
		default          : return 0;
     }
}
//==============================================================================
// FUNCTION READ
//==============================================================================
word TS7Worker::RA_NotFound(PResFunReadItem ResItem, TEv &EV)
{
    ResItem->DataLength=SwapWord(0x0004);
    ResItem->ReturnCode=Code7ResItemNotAvailable;
    ResItem->TransportSize=0x00;
    EV.EvRetCode=evrErrAreaNotFound;
    return 0;
}
//------------------------------------------------------------------------------
word TS7Worker::RA_OutOfRange(PResFunReadItem ResItem, TEv &EV)
{
    ResItem->DataLength=SwapWord(0x0004);
    ResItem->ReturnCode=Code7AddressOutOfRange;
    ResItem->TransportSize=0x00;
    EV.EvRetCode=evrErrOutOfRange;
    return 0;
}
//------------------------------------------------------------------------------
word TS7Worker::RA_SizeOverPDU(PResFunReadItem ResItem, TEv &EV)
{
    ResItem->DataLength=SwapWord(0x0004);
    ResItem->ReturnCode=byte(SwapWord(Code7DataOverPDU));
    ResItem->TransportSize=0x00;
    EV.EvRetCode=evrErrOverPDU;
    return 0;
}
//------------------------------------------------------------------------------
PS7Area TS7Worker::GetArea(byte S7Code, word index)
{
	switch(S7Code)
    {
	case S7AreaPE : return FServer->HA[srvAreaPE];
	case S7AreaPA : return FServer->HA[srvAreaPA];
	case S7AreaMK : return FServer->HA[srvAreaMK];
	case S7AreaCT : return FServer->HA[srvAreaCT];
	case S7AreaTM : return FServer->HA[srvAreaTM];
	case S7AreaDB : return FServer->FindDB(index);
	default       : return NULL;
    };
}
//------------------------------------------------------------------------------
word TS7Worker::ReadArea(PResFunReadItem ResItemData, PReqFunReadItem ReqItemPar,
     int &PDURemainder, TEv &EV)
{
    PS7Area P;
	word DBNum = 0;
	word Elements;
    longword Start, Size, ASize, AStart;
    longword *PAdd;
    byte BitIndex, ByteVal;
	int Multiplier;
    void *Source = NULL;
    PSnapCriticalSection pcs;

    P=NULL;
    EV.EvStart   =0;
    EV.EvSize    =0;
    EV.EvRetCode =0;
    EV.EvIndex   =0;

    EV.EvArea=ReqItemPar->Area;
    // Get Pointer to selected Area

	if (ReqItemPar->Area==S7AreaDB)
	{
        DBNum=SwapWord(ReqItemPar->DBNumber);
        EV.EvIndex=DBNum;
	};

	if (!FServer->ResourceLess)
	{
		P = GetArea(ReqItemPar->Area, DBNum);
		if (P == NULL)
			return RA_NotFound(ResItemData, EV);
	}

    // Calcs the amount
	Multiplier = DataSizeByte(ReqItemPar->TransportSize);
	if (Multiplier==0)
		return RA_OutOfRange(ResItemData, EV);

    // Checks timers/counters coherence
    if ((ReqItemPar->Area==S7AreaTM) ^ (ReqItemPar->TransportSize==S7WLTimer))
        return RA_OutOfRange(ResItemData, EV);

    if ((ReqItemPar->Area==S7AreaCT) ^ (ReqItemPar->TransportSize==S7WLCounter))
        return RA_OutOfRange(ResItemData, EV);

    // Calcs size
	Elements = SwapWord(ReqItemPar->Length);
	Size=Multiplier*Elements;   
	EV.EvSize=Size;

    // The sum of the items must not exceed the PDU size negotiated
    if (PDURemainder-Size<=0)
        return RA_SizeOverPDU(ResItemData, EV);
    else
        PDURemainder-=Size;

    // More then 1 bit is not supported by S7 CPU 
    if ((ReqItemPar->TransportSize==S7WLBit) && (Size>1))
        return RA_OutOfRange(ResItemData, EV);

	// Calcs the start point
    PAdd=(longword*)(&ReqItemPar->Area);   // points to area since we need 4 bytes for a pointer
    Start=SwapDWord(*PAdd & 0xFFFFFF00);

    // Checks if the address is not multiple of 8 when transport size is neither bit nor timer nor counter
    if (
		(ReqItemPar->TransportSize!=S7WLBit) &&
		(ReqItemPar->TransportSize!=S7WLTimer) &&
		(ReqItemPar->TransportSize!=S7WLCounter) &&
		((Start % 8) !=0)
	   )
		return RA_OutOfRange(ResItemData, EV);

    // AStart is only for callback
	if ((ReqItemPar->TransportSize != S7WLBit) && (ReqItemPar->TransportSize != S7WLCounter) && (ReqItemPar->TransportSize != S7WLTimer))
		AStart = Start >> 3;
	else
		AStart = Start;

	if ((ReqItemPar->TransportSize == S7WLCounter) || (ReqItemPar->TransportSize == S7WLTimer))
	{
		Start = Start >> 1;   // 1 Timer or Counter = 2 bytes
	}
	else
	{
		BitIndex  =Start & 0x07; // start bit
		Start     =Start >> 3;   // start byte
	}
	
	EV.EvStart=Start;

	// Checks bounds
	if (!FServer->ResourceLess)
	{
		ASize = P->Size; // Area size
		if (Start + Size > ASize)
			return RA_OutOfRange(ResItemData, EV);
		Source = P->PData + Start;
	}

	// Read Event (before copy data)
    DoReadEvent(evcDataRead,0,EV.EvArea,EV.EvIndex,EV.EvStart,EV.EvSize);	

	if (FServer->ResourceLess)
	{
		memset(&ResItemData->Data, 0, Size);
		if (!FServer->DoReadArea(ClientHandle, EV.EvArea, EV.EvIndex, AStart, Elements, ReqItemPar->TransportSize, &ResItemData->Data))
			return RA_NotFound(ResItemData, EV);
	}
	else
	{
		// Lock the area
		pcs = P->cs;
		pcs->Enter();
		// Get Data
		memcpy(&ResItemData->Data, Source, Size);
		// Unlock the area
		pcs->Leave();
	}

    ResItemData->ReturnCode=0xFF;
    // Set Result transport size and, for bit, performs the mask
    switch (ReqItemPar->TransportSize)
    {
      case S7WLBit:
        {
          ByteVal=ResItemData->Data[0];

          if ((ByteVal & BitMask[BitIndex])!=0)
              ResItemData->Data[0]=0x01;
          else
              ResItemData->Data[0]=0x00;

          ResItemData->TransportSize=TS_ResBit;
          ResItemData->DataLength=SwapWord(Size);
        };break;
      case S7WLByte:
	  case S7WLWord:
	  case S7WLDWord:
        {
          ResItemData->TransportSize=TS_ResByte;
          ResItemData->DataLength=SwapWord(Size*8);
        };break;
	  case S7WLInt:
	  case S7WLDInt:
        {
          ResItemData->TransportSize=TS_ResInt;
          ResItemData->DataLength=SwapWord(Size*8);
        };break;
      case S7WLReal:
        {
          ResItemData->TransportSize=TS_ResReal;
          ResItemData->DataLength=SwapWord(Size);
        };break;
      case S7WLChar:
      case S7WLTimer:
      case S7WLCounter:
        {
          ResItemData->TransportSize=TS_ResOctet;
		  ResItemData->DataLength=SwapWord(Size);
        };break;
      default : 
        {
          ResItemData->TransportSize=TS_ResByte;
          ResItemData->DataLength=SwapWord(Size*8);
        };break;
    }
    EV.EvRetCode=evrNoError;
    return Size;
}
//------------------------------------------------------------------------------
bool TS7Worker::PerformFunctionRead()
{
    PReqFunReadParams ReqParams;
    PResFunReadParams ResParams;
    TResFunReadData   ResData;
    TS7Answer23       Answer;
    uintptr_t         Offset;
    word ItemSize;
    int ItemsCount, c,
    TotalSize,
    PDURemainder;
    TEv EV;

	PDURemainder=FPDULength;
    // Stage 1 : Setup pointers and initial check
	ReqParams=PReqFunReadParams(pbyte(PDUH_in)+sizeof(TS7ReqHeader));
    ResParams=PResFunReadParams(pbyte(&Answer)+ResHeaderSize23);        // Params after the header

    // trunk to 20 max items.
    if (ReqParams->ItemsCount>MaxVars)
        ReqParams->ItemsCount=MaxVars;

    ItemsCount=ReqParams->ItemsCount;

    // Stage 2 : gather data
    Offset=sizeof(TResFunReadParams);      // = 2

    for (c = 0; c < ItemsCount; c++)
	{
		ResData[c]=PResFunReadItem(pbyte(ResParams)+Offset);
		ItemSize=ReadArea(ResData[c],&ReqParams->Items[c],PDURemainder, EV);

        // S7 doesn't xfer odd byte amount
        if ((c<ItemsCount-1) && (ItemSize % 2 != 0))
	      ItemSize++;
		
        Offset+=(ItemSize+4);
        // For multiple items we have to create multiple events
        if (ItemsCount>1)
            DoEvent(evcDataRead,EV.EvRetCode,EV.EvArea,EV.EvIndex,EV.EvStart,EV.EvSize);
    }
    // Stage 3 : finalize the answer and send the packet
    Answer.Header.P=0x32;
    Answer.Header.PDUType=0x03;
    Answer.Header.AB_EX=0x0000;
    Answer.Header.Sequence=PDUH_in->Sequence;
    Answer.Header.ParLen=SwapWord(sizeof(TResFunReadParams));
    Answer.Header.Error=0x0000; // this is zero, we will find the error in ResData.ReturnCode
    Answer.Header.DataLen=SwapWord(word(Offset)-2);

    ResParams->FunRead  =ReqParams->FunRead;
    ResParams->ItemCount=ReqParams->ItemsCount;

    TotalSize=ResHeaderSize23+int(Offset);
    isoSendBuffer(&Answer, TotalSize);

    // For single item (most likely case) it's better to work with the event after
    // we sent the answer
    if (ItemsCount==1)
        DoEvent(evcDataRead,EV.EvRetCode,EV.EvArea,EV.EvIndex,EV.EvStart,EV.EvSize);

    return true;
}
//==============================================================================
// FUNCTION WRITE
//==============================================================================
byte TS7Worker::WA_NotFound(TEv &EV)
{
	EV.EvRetCode=evrErrAreaNotFound;
    return Code7ResItemNotAvailable;
}
//------------------------------------------------------------------------------
byte TS7Worker::WA_InvalidTransportSize(TEv &EV)
{
    EV.EvRetCode=evrErrTransportSize;
    return Code7InvalidTransportSize;
}
//------------------------------------------------------------------------------
byte TS7Worker::WA_OutOfRange(TEv &EV)
{
    EV.EvRetCode=evrErrOutOfRange;
    return Code7AddressOutOfRange;
}
//------------------------------------------------------------------------------
byte TS7Worker::WA_DataSizeMismatch(TEv &EV)
{
    EV.EvRetCode=evrDataSizeMismatch;
    return Code7WriteDataSizeMismatch;
}
//------------------------------------------------------------------------------
byte TS7Worker::WriteArea(PReqFunWriteDataItem ReqItemData, PReqFunWriteItem ReqItemPar,
     TEv &EV)
{
	int Multiplier;
    PS7Area P = NULL;
	word DBNum = 0;
	word Elements;
    longword *PAdd;
	PSnapCriticalSection pcs;
	longword Start, Size, ASize, DataLen, AStart;
	pbyte Target = NULL;
	byte BitIndex;

    EV.EvStart   =0;
	EV.EvSize    =0;
    EV.EvRetCode =evrNoError;
    EV.EvIndex   =0;

    EV.EvArea=ReqItemPar->Area;
    // Get Pointer to selected Area
	if (ReqItemPar->Area==S7AreaDB)
	{
		DBNum=SwapWord(ReqItemPar->DBNumber);
		EV.EvIndex=DBNum;
	};
	
	if (!FServer->ResourceLess)
	{
		P=GetArea(ReqItemPar->Area, DBNum);
		if (P==NULL)
			return WA_NotFound(EV);
	}

    // Calcs the amount
	Multiplier = DataSizeByte(ReqItemPar->TransportSize);
	if (Multiplier==0)
		return WA_InvalidTransportSize(EV);

    // Checks timers/counters coherence
    if ((ReqItemPar->Area==S7AreaTM) ^ (ReqItemPar->TransportSize==S7WLTimer))
        return WA_OutOfRange(EV);

	if ((ReqItemPar->Area==S7AreaCT) ^ (ReqItemPar->TransportSize==S7WLCounter))
        return WA_OutOfRange(EV);

	// Calcs size
	Elements = SwapWord(ReqItemPar->Length);
	Size = Multiplier*Elements;
    EV.EvSize=Size;

    // More) 1 bit is not supported by S7 CPU
    if ((ReqItemPar->TransportSize==S7WLBit) && (Size>1))
        return WA_OutOfRange(EV);

    // Calcs the start point ??
    PAdd=(longword*)&ReqItemPar->Area;   // points to area since we need 4 bytes for a pointer
    Start=SwapDWord(*PAdd & 0xFFFFFF00);

    // Checks if the address is not multiple of 8 when transport size is neither bit nor timer nor counter
    if (
		(ReqItemPar->TransportSize!=S7WLBit) && 
		(ReqItemPar->TransportSize!=S7WLTimer) && 
		(ReqItemPar->TransportSize!=S7WLCounter) && 
		((Start % 8) !=0)
	   )
		return WA_OutOfRange(EV);

	// AStart is only for callback
	if ((ReqItemPar->TransportSize != S7WLBit) && (ReqItemPar->TransportSize != S7WLCounter) && (ReqItemPar->TransportSize != S7WLTimer))
		AStart = Start >> 3;
	else
		AStart = Start;

	if ((ReqItemPar->TransportSize == S7WLCounter) || (ReqItemPar->TransportSize == S7WLTimer))
	{
		Start = Start >> 1;   // 1 Timer or Counter = 2 bytes
	}
	else
	{
		BitIndex = Start & 0x07; // start bit
		Start = Start >> 3;   // start byte
	}
	EV.EvStart =Start;
	
	if (!FServer->ResourceLess)
	{
		// Checks bounds
		ASize = P->Size; // Area size
		if (Start + Size > ASize)
			return WA_OutOfRange(EV);
		Target = pbyte(P->PData + Start);
	}
    // Checks data size coherence
    DataLen=SwapWord(ReqItemData->DataLength);

	if ((ReqItemData->TransportSize!=TS_ResOctet) && (ReqItemData->TransportSize!=TS_ResReal) && (ReqItemData->TransportSize!=TS_ResBit))
		DataLen=DataLen / 8;

	if (DataLen!=Size)
        return WA_DataSizeMismatch(EV);

	if (FServer->ResourceLess)
	{
		if (!FServer->DoWriteArea(ClientHandle, EV.EvArea, EV.EvIndex, AStart, Elements, ReqItemPar->TransportSize, &ReqItemData->Data[0]))
			return WA_NotFound(EV);
	}
	else
	{
		if (ReqItemPar->TransportSize==S7WLBit)
		{
		  if ((ReqItemData->Data[0] & 0x01) != 0)   // bit set
			  *Target=*Target | BitMask[BitIndex];
		  else                                      // bit reset
			  *Target=*Target & (~BitMask[BitIndex]);
		}
		else {
		  // Lock the area
			pcs = P->cs;
			pcs->Enter();
			// Write Data
			memcpy(Target, &ReqItemData->Data[0], Size);
			pcs->Leave();
		};
	}
	
	return 0xFF;
}
//------------------------------------------------------------------------------
bool TS7Worker::PerformFunctionWrite()
{
    PReqFunWriteParams ReqParams;
    TReqFunWriteData ReqData;
    PResFunWrite ResData;
    TS7Answer23 Answer;
	int L;

	uintptr_t StartData;
	int c, ItemsCount;
	int ResDSize;
	TEv EV;

	// Stage 1 : Setup pointers and initial check
	ReqParams=PReqFunWriteParams(pbyte(PDUH_in)+sizeof(TS7ReqHeader));
	ResData  =PResFunWrite(pbyte(&Answer)+ResHeaderSize23);

	StartData=sizeof(TS7ReqHeader)+SwapWord(PDUH_in->ParLen);

	ItemsCount=ReqParams->ItemsCount;
	ResDSize  =ResHeaderSize23+2+ItemsCount;
	for (c = 0; c < ItemsCount; c++)
	{
		ReqData[c]=PReqFunWriteDataItem(pbyte(PDUH_in)+StartData);
		
		if ((ReqParams->Items[c].TransportSize == S7WLTimer) || (ReqParams->Items[c].TransportSize == S7WLCounter) || (ReqParams->Items[c].TransportSize == S7WLBit))
			L = SwapWord(ReqData[c]->DataLength);
		else
			L = (SwapWord(ReqData[c]->DataLength) / 8);

		StartData+=L+4;
		// the datalength is always even
		if ( L % 2 != 0) StartData++;
	}

	ResData->FunWrite =pduFuncWrite;
	ResData->ItemCount=ReqParams->ItemsCount;

	// Stage 2 : Write data
	for (c = 0; c < ItemsCount; c++)
	{
	  ResData->Data[c]=WriteArea(ReqData[c],&ReqParams->Items[c], EV);
      // For multiple items we have to create multiple events
      if (ItemsCount>1)
           DoEvent(evcDataWrite,EV.EvRetCode,EV.EvArea,EV.EvIndex,EV.EvStart,EV.EvSize);
    }

    // Stage 3 : finalize the answer
    Answer.Header.P=0x32;
    Answer.Header.PDUType=0x03;
    Answer.Header.AB_EX=0x0000;
    Answer.Header.Sequence=PDUH_in->Sequence;
    Answer.Header.ParLen=SwapWord(0x02);
    Answer.Header.Error=0x0000; // this is zero, we will find the error in ResData.ReturnCode if any
    Answer.Header.DataLen=SwapWord(ItemsCount);

    isoSendBuffer(&Answer,ResDSize);
    // For single item (most likely case) it's better to fire the event after
    // we sent the answer
    if (ItemsCount==1)
        DoEvent(evcDataWrite,EV.EvRetCode,EV.EvArea,EV.EvIndex,EV.EvStart,EV.EvSize);
    return true;
}
//==============================================================================
// FUNCTION NEGOTIATE
//==============================================================================
bool TS7Worker::PerformFunctionNegotiate()
{
	PReqFunNegotiateParams ReqParams;
	PResFunNegotiateParams ResParams;
	word ReqLen;
	TS7Answer23 Answer;
	int Size;

	// Setup pointers
	ReqParams=PReqFunNegotiateParams(pbyte(PDUH_in)+sizeof(TS7ReqHeader));
	ResParams=PResFunNegotiateParams(pbyte(&Answer)+sizeof(TS7ResHeader23));
	// Prepares the answer
	Answer.Header.P=0x32;
	Answer.Header.PDUType=0x03;
	Answer.Header.AB_EX=0x0000;
	Answer.Header.Sequence=PDUH_in->Sequence;
	Answer.Header.ParLen=SwapWord(sizeof(TResFunNegotiateParams));
	Answer.Header.DataLen=0x0000;
	Answer.Header.Error=0x0000;
	// Params point at the end of the header
	ResParams->FunNegotiate=pduNegotiate;
	ResParams->Unknown=0x0;
	// We offer the same
	ResParams->ParallelJobs_1=ReqParams->ParallelJobs_1;
	ResParams->ParallelJobs_2=ReqParams->ParallelJobs_2;

	if (FServer->ForcePDU == 0)
	{
		ReqLen = SwapWord(ReqParams->PDULength);
		if (ReqLen<MinPduSize)
			ResParams->PDULength = SwapWord(MinPduSize);
		else
			if (ReqLen>IsoPayload_Size)
				ResParams->PDULength = SwapWord(IsoPayload_Size);
			else
				ResParams->PDULength = ReqParams->PDULength;
	}
	else
		ResParams->PDULength = SwapWord(FServer->ForcePDU);

	FPDULength=SwapWord(ResParams->PDULength); // Stores the value
	// Sends the answer
	Size=sizeof(TS7ResHeader23) + sizeof(TResFunNegotiateParams);
	isoSendBuffer(&Answer, Size);
	// Store the event
	DoEvent(evcNegotiatePDU, evrNoError, FPDULength, 0, 0, 0);
	return true;
}
//==============================================================================
// FUNCTION CONTROL
//==============================================================================
bool TS7Worker::PerformFunctionControl(byte PduFun)
{
    TS7Answer23 Answer;
    PResFunCtrl ResParams;
    word ParLen;
    word CtrlCode;

    // Setup pointer
    ResParams=PResFunCtrl(pbyte(&Answer)+sizeof(TS7ResHeader23));
    // Prepares the answer
    Answer.Header.P=0x32;
    Answer.Header.PDUType=0x03;
    Answer.Header.AB_EX=0x0000;
    Answer.Header.Sequence=PDUH_in->Sequence;
    Answer.Header.ParLen=SwapWord(0x0001); // We send only Res fun without para
    Answer.Header.DataLen=0x0000;
    Answer.Header.Error=0x0000;
    ResParams->ResFun=PduFun;
    ResParams->para  =0x00;

    ParLen=SwapWord(PDUH_in->ParLen);
    if (PduFun==pduStop)
      CtrlCode=CodeControlStop;
    else
    {
      switch (ParLen)
      {
        case 16 : CtrlCode=CodeControlCompress;  break;
        case 18 : CtrlCode=CodeControlCpyRamRom; break;
        case 20 : CtrlCode=CodeControlWarmStart; break;
        case 22 : CtrlCode=CodeControlColdStart; break;
        case 26 : CtrlCode=CodeControlInsDel;    break;
        default : CtrlCode=CodeControlUnknown;
      }
    }
    // Sends the answer
    isoSendBuffer(&Answer,sizeof(TS7ResHeader23)+1);
    // Stores the event
    DoEvent(evcControl,0,CtrlCode,0,0,0);

    if ((CtrlCode==CodeControlWarmStart) || (CtrlCode==CodeControlColdStart))
        FServer->CpuStatus=S7CpuStatusRun;

    if (CtrlCode==CodeControlStop)
        FServer->CpuStatus=S7CpuStatusStop;

    return true;
}
//==============================================================================
// FUNCTION UPLOAD
//==============================================================================
bool TS7Worker::PerformFunctionUpload()
{
    TS7Answer23 Answer;

    // Upload is not implemented, however to avoid that S7 manager hangs
    // we simulate a cpu read/write protected.
    // We can see the directory but can't upload/download anything

    // Prepares the answer
    Answer.Header.P=0x32;
    Answer.Header.PDUType =pduResponse;
    Answer.Header.AB_EX=0x0000;
    Answer.Header.Sequence=PDUH_in->Sequence;
    Answer.Header.ParLen=0;
    Answer.Header.DataLen=0;
    Answer.Header.Error=SwapWord(Code7NeedPassword);
    // Sends the answer
    isoSendBuffer(&Answer,sizeof(TS7ResHeader23));

    DoEvent(evcUpload,evrCannotUpload,evsStartUpload,0,0,0);
    return true;
}
//==============================================================================
// FUNCTION DOWNLOAD
//==============================================================================
bool TS7Worker::PerformFunctionDownload()
{
    TS7Answer23 Answer;

    // Download is not implemented, however to avoid that S7 manager hangs
    // we simulate a cpu read/write protected.
    // We can see the directory but can't upload/download anything

    // Prepares the answer
    Answer.Header.P=0x32;
    Answer.Header.PDUType =pduResponse;
    Answer.Header.AB_EX=0x0000;
    Answer.Header.Sequence=PDUH_in->Sequence;
    Answer.Header.ParLen=0;
    Answer.Header.DataLen=0;
    Answer.Header.Error=SwapWord(Code7NeedPassword);
    // Sends the answer
    isoSendBuffer(&Answer,sizeof(TS7ResHeader23));

    DoEvent(evcUpload,evrCannotDownload, evsStartDownload,0,0,0);
    return true;
}
//==============================================================================
// FUNCTIONS PROGRAMMER AND CYCLIC DATA (NOT IMPLEMENTED...yet)
//==============================================================================
bool TS7Worker::PerformGroupProgrammer()
{
    DoEvent(evcPDUincoming,evrNotImplemented,grProgrammer,0,0,0);
    return true;
}
//------------------------------------------------------------------------------
bool TS7Worker::PerformGroupCyclicData()
{
    DoEvent(evcPDUincoming,evrNotImplemented,grCyclicData,0,0,0);
    return true;
}
//==============================================================================
// BLOCK(S) INFO FUNCTIONS
//==============================================================================
void TS7Worker::BLK_ListAll(TCB &CB)
{
    PDataFunListAll Data;
    int TotalSize;

    TotalSize = ResHeaderSize17+sizeof(TResFunGetBlockInfo)+sizeof(TDataFunListAll);
    // Prepares the answer
    CB.Answer.Header.P=0x32;
    CB.Answer.Header.PDUType=PduType_userdata;
    CB.Answer.Header.AB_EX=0x0000;
    CB.Answer.Header.Sequence=PDUH_in->Sequence;
    CB.Answer.Header.ParLen =SwapWord(sizeof(TResFunGetBlockInfo));
    CB.Answer.Header.DataLen=SwapWord(sizeof(TDataFunListAll));

    CB.ResParams->Head[0]=CB.ReqParams->Head[0];
    CB.ResParams->Head[1]=CB.ReqParams->Head[1];
    CB.ResParams->Head[2]=CB.ReqParams->Head[2];
    CB.ResParams->Plen  =0x08;
    CB.ResParams->Uk    =0x12;
    CB.ResParams->Tg    =0x83; // Type response, group functions info
    CB.ResParams->SubFun=SFun_ListAll;

    CB.ResParams->Seq   =CB.ReqParams->Seq;
    CB.ResParams->Rsvd  =0x0000;
    CB.ResParams->ErrNo =0x0000;

    Data=PDataFunListAll(pbyte(&CB.Answer)+ResHeaderSize17+sizeof(TResFunGetBlockInfo));
    Data->RetVal=0xFF;
    Data->TRSize=TS_ResOctet;
    Data->Length=SwapWord(28); // 28 = Size of TDataFunListAll.Blocks
    // Fill elements, only DB will have a valid number
    Data->Blocks[0].Zero=0x30;
    Data->Blocks[0].BType=Block_OB;
    Data->Blocks[0].BCount=0x0000;    // We don't have OBs
    Data->Blocks[1].Zero=0x30;
    Data->Blocks[1].BType=Block_FB;
    Data->Blocks[1].BCount=0x0000;    // We don't have FBs
    Data->Blocks[2].Zero=0x30;
    Data->Blocks[2].BType=Block_FC;
    Data->Blocks[2].BCount=0x0000;    // We don't have FCs
    Data->Blocks[3].Zero=0x30;
    Data->Blocks[3].BType=Block_DB;
    Data->Blocks[3].BCount=SwapWord(FServer->DBCount); // Most likely we HAVE DBs
    Data->Blocks[4].Zero=0x30;
    Data->Blocks[4].BType=Block_SDB;
    Data->Blocks[4].BCount=0x0000;    // We don't have SDBs
    Data->Blocks[5].Zero=0x30;
    Data->Blocks[5].BType=Block_SFC;
    Data->Blocks[5].BCount=0x0000;    // We don't have SFCs
    Data->Blocks[6].Zero=0x30;
    Data->Blocks[6].BType=Block_SFB;
    Data->Blocks[6].BCount=0x0000;    // We don't have SFBs
    // Sends
    isoSendBuffer(&CB.Answer,TotalSize);
    DoEvent(evcDirectory, 0, evsGetBlockList, 0, 0, 0);
}
//------------------------------------------------------------------------------
void TS7Worker::BLK_NoResource_ListBoT(PDataFunGetBot Data, TCB &CB)
{
    CB.DataLength    =4;
    DBCnt            =0; // Reset counter
    CB.Answer.Header.DataLen=SwapWord(CB.DataLength);
    CB.ResParams->ErrNo =0x0ED2; // function in error
    Data->RetVal     =0x0A;   // No resource available
    Data->TSize      =0x00;   // No transport size;
    Data->DataLen    =0x0000; // No data;
    CB.evError       =evrResNotFound;
}
//------------------------------------------------------------------------------
void TS7Worker::BLK_ListBoT(byte BlockType, bool Start, TCB &CB)
{
    PDataFunGetBot Data;
    int MaxItems, TotalSize, cnt;
    int HiBound = FServer->DBLimit+1;

    CB.evError=0;
    MaxItems=(FPDULength - 32) / 4;
    // Prepares the answer
    CB.Answer.Header.P=0x32;
    CB.Answer.Header.PDUType=PduType_userdata;
    CB.Answer.Header.AB_EX=0x0000;
    CB.Answer.Header.Sequence=PDUH_in->Sequence;
    CB.Answer.Header.ParLen =SwapWord(sizeof(TResFunGetBlockInfo));

    CB.ResParams->Head[0]=CB.ReqParams->Head[0];
    CB.ResParams->Head[1]=CB.ReqParams->Head[1];
    CB.ResParams->Head[2]=CB.ReqParams->Head[2];
    CB.ResParams->Plen  =0x08;
    CB.ResParams->Uk    =0x12;
    CB.ResParams->Tg    =0x83; // Type response, group functions info
    CB.ResParams->SubFun=SFun_ListBoT;
    CB.ResParams->Seq   =CB.ReqParams->Seq;
    CB.ResParams->Rsvd  =0x0000;
    Data=PDataFunGetBot(pbyte(&CB.Answer)+ResHeaderSize17+sizeof(TResFunGetBlockInfo));

    if (BlockType==Block_DB)
    {
      cnt =0;  // Local couter
      if (Start)
	      DBCnt=-1; // Global counter

      if (FServer->DBCount>0)
      {
	      while ((cnt<MaxItems) && (DBCnt<HiBound))
	      {
		      DBCnt++;
		      if (FServer->DB[DBCnt]!=NULL)
		      {
			      Data->Items[cnt].BlockNum=SwapWord(FServer->DB[DBCnt]->Number);
			      Data->Items[cnt].Unknown  =0x22;
			      Data->Items[cnt].BlockLang=0x05;
			      cnt++;
		      };
	      };

	      if ((cnt<MaxItems) || (DBCnt==HiBound))
	      {
		      DBCnt=0; // Finished
		      CB.ResParams->Rsvd=0x0023;
	      }
	      else
		      CB.ResParams->Rsvd=0x0123;

	      if (cnt>0)
	      {
		      CB.ResParams->ErrNo =0x0000;
		      Data->TSize =TS_ResOctet;
		      Data->RetVal=0xFF;
		      CB.DataLength=4+(cnt*word(sizeof(TDataFunGetBotItem)));
		      CB.Answer.Header.DataLen=SwapWord(CB.DataLength);
		      Data->DataLen=SwapWord(CB.DataLength-4);
	      }
	      else
		      BLK_NoResource_ListBoT(Data, CB);
      }
      else
	      BLK_NoResource_ListBoT(Data, CB);
    }
    else // we store only DBs
	    BLK_NoResource_ListBoT(Data, CB);

    TotalSize = ResHeaderSize17+sizeof(TResFunGetBlockInfo)+CB.DataLength;
    isoSendBuffer(&CB.Answer,TotalSize);
    if (Start)
        DoEvent(evcDirectory, CB.evError, evsStartListBoT, BlockType, 0, 0);
    else
        DoEvent(evcDirectory, CB.evError, evsListBoT, BlockType, 0, 0);
}
//------------------------------------------------------------------------------
void TS7Worker::BLK_NoResource_GetBlkInfo(PResDataBlockInfo Data, TCB &CB)
{
    CB.DataLength  =4;
    CB.Answer.Header.DataLen=SwapWord(CB.DataLength);
    CB.ResParams->ErrNo =0x09D2; // function in error
    Data->RetVal        =0x0A;   // No resource available
    Data->TSize         =0x00;   // No transport size;
    Data->Length        =0x0000; // No data;
    CB.evError          =evrResNotFound;
}
//------------------------------------------------------------------------------
void TS7Worker::BLK_GetBlockNum_GetBlkInfo(int &BlkNum, PReqDataBlockInfo ReqData)
{
    BlkNum = (ReqData->AsciiBlk[4] - 0x30) +
             (ReqData->AsciiBlk[3] - 0x30) * 10 +
             (ReqData->AsciiBlk[2] - 0x30) * 100 +
             (ReqData->AsciiBlk[1] - 0x30) * 1000 +
             (ReqData->AsciiBlk[0] - 0x30) * 10000;

    if (BlkNum>65535)
      BlkNum=-1;
}
//------------------------------------------------------------------------------
void TS7Worker::BLK_DoBlockInfo_GetBlkInfo(PS7Area DB, PResDataBlockInfo Data, TCB &CB)
{
    // Prepares the answer
    CB.Answer.Header.P=0x32;
    CB.Answer.Header.PDUType=PduType_userdata;
    CB.Answer.Header.AB_EX=0x0000;
    CB.Answer.Header.Sequence=PDUH_in->Sequence;
    CB.Answer.Header.ParLen =SwapWord(sizeof(TResFunGetBlockInfo));

    CB.ResParams->Head[0]=CB.ReqParams->Head[0];
    CB.ResParams->Head[1]=CB.ReqParams->Head[1];
    CB.ResParams->Head[2]=CB.ReqParams->Head[2];
    CB.ResParams->Plen  =0x08;
    CB.ResParams->Uk    =0x12;
    CB.ResParams->Tg    =0x83; // Type response, group functions info
    CB.ResParams->SubFun=SFun_BlkInfo;
    CB.ResParams->Seq   =CB.ReqParams->Seq;
    CB.ResParams->Rsvd  =0x0000;
    CB.ResParams->ErrNo =0x0000;

    CB.DataLength =sizeof(TResDataBlockInfo);
    CB.Answer.Header.DataLen=SwapWord(CB.DataLength);
    CB.ResParams->ErrNo =0x0000;

    Data->RetVal       =0xFF;
    Data->TSize        =TS_ResOctet;
    Data->Length       =SwapWord(78); // this struct - RetValData->Tsize and length
    Data->Cst_b        =0x01;
    Data->BlkType      =0x00;
    Data->Cst_w1       =0x4A00;
    Data->Cst_w2       =0x0022;
    Data->Cst_pp       =0x7070;
    Data->Unknown_1    =0x01;
    Data->BlkFlags     =0x01;
    Data->BlkLang      =BlockLangDB;
    Data->SubBlkType   =0x0A;
    Data->CodeTime_dy  =SwapWord(5800);// Nov/18/1999 my princess's birthdate
    Data->IntfTime_dy  =Data->CodeTime_dy;
    Data->LocDataLen   =0x0000;
    Data->BlkNumber    =SwapWord(DB->Number);
    Data->SbbLen       =0x1400;
    Data->AddLen       =0x0000;
    Data->MC7Len       =SwapWord(DB->Size);
    Data->LenLoadMem   =SwapDWord(DB->Size+92);
    Data->Version      =0x01;
    Data->Unknown_2    =0x00;
    Data->BlkChksum    =0x0000;
}
//------------------------------------------------------------------------------
void TS7Worker::BLK_GetBlkInfo(TCB &CB)
{
    PReqDataBlockInfo ReqData;
    PResDataBlockInfo Data;
    int BlkNum;
    PS7Area BlkDB;
    byte BlkTypeInfo;
    int TotalSize;

    CB.evError=0;
    Data   =PResDataBlockInfo(pbyte(&CB.Answer)+ResHeaderSize17+sizeof(TResFunGetBlockInfo));
    ReqData=PReqDataBlockInfo(pbyte(PDUH_in)+ReqHeaderSize+sizeof(TReqFunGetBlockInfo));
    memset(Data,0,sizeof(TResDataBlockInfo)); // many fields are 0

    BLK_GetBlockNum_GetBlkInfo(BlkNum, ReqData);
    BlkTypeInfo=ReqData->BlkType;
    if (BlkTypeInfo==Block_DB)
    {
      if (BlkNum>=0)
      {
	    BlkDB=FServer->FindDB(BlkNum);
	    if (BlkDB!=NULL)
                BLK_DoBlockInfo_GetBlkInfo(BlkDB, Data, CB);
	    else
                BLK_NoResource_GetBlkInfo(Data, CB);
      }
      else
          BLK_NoResource_GetBlkInfo(Data, CB);
    }
    else
        BLK_NoResource_GetBlkInfo(Data, CB);

    TotalSize = ResHeaderSize17+sizeof(TResFunGetBlockInfo)+sizeof(TResDataBlockInfo);
    isoSendBuffer(&CB.Answer, TotalSize);
    DoEvent(evcDirectory,CB.evError,evsGetBlockInfo,BlkTypeInfo,BlkNum,0);
}
//------------------------------------------------------------------------------
bool TS7Worker::PerformGroupBlockInfo()
{
    TCB CB;
    pbyte BlockType;

    // Setup pointers
    CB.ReqParams=PReqFunGetBlockInfo(pbyte(PDUH_in)+ReqHeaderSize);
    CB.ResParams=PResFunGetBlockInfo(pbyte(&CB.Answer)+ResHeaderSize17);
    BlockType   =pbyte(PDUH_in)+23;

    switch (CB.ReqParams->SubFun)
    {
      case SFun_ListAll : BLK_ListAll(CB); break;            // List all blocks
      case SFun_ListBoT :
      {
          if (CB.ReqParams->Plen==4)
          {
              LastBlk=*BlockType;
              BLK_ListBoT(*BlockType, true, CB); // start sequence from beginning
          }
          else
              BLK_ListBoT(LastBlk, false, CB);  // Continue sequence
      }; break;
      case SFun_BlkInfo : BLK_GetBlkInfo(CB); // Get Block info
    }
    return true;
}
//==============================================================================
// FUNCTION SZL
//==============================================================================
void TS7Worker::SZLNotAvailable()
{
    SZL.Answer.Header.DataLen=SwapWord(sizeof(SZLNotAvail));
	SZL.ResParams->Err = 0x02D4;
    memcpy(SZL.ResData, &SZLNotAvail, sizeof(SZLNotAvail));
    isoSendBuffer(&SZL.Answer,26);
    SZL.SZLDone=false;
}
void TS7Worker::SZLSystemState()
{
    SZL.Answer.Header.DataLen=SwapWord(sizeof(SZLSysState));
    SZL.ResParams->Err =0x0000;
    memcpy(SZL.ResData,&SZLSysState,sizeof(SZLSysState));
    isoSendBuffer(&SZL.Answer,28);
	SZL.SZLDone=true;
}
void TS7Worker::SZLData(void *P, int len)
{
	int MaxSzl=FPDULength-22;

	if (len>MaxSzl) {
		len=MaxSzl;
	}

	SZL.Answer.Header.DataLen=SwapWord(word(len));
	SZL.ResParams->Err  =0x0000;
	SZL.ResParams->resvd=0x0000; // this is the end, no more packets
	memcpy(SZL.ResData, P, len);

	SZL.ResData[2]=((len-4)>>8) & 0xFF;
	SZL.ResData[3]=(len-4) & 0xFF;

	isoSendBuffer(&SZL.Answer,22+len);
	SZL.SZLDone=true;
}
// this block is dynamic (contains date/time and cpu status)
void TS7Worker::SZL_ID424()
{
	PS7Time PTime;
	pbyte PStatus;

	SZL.Answer.Header.DataLen=SwapWord(sizeof(SZL_ID_0424_IDX_XXXX));
	SZL.ResParams->Err  =0x0000;
	PTime=PS7Time(pbyte(SZL.ResData)+24);
	PStatus =pbyte(SZL.ResData)+15;
	memcpy(SZL.ResData,&SZL_ID_0424_IDX_XXXX,sizeof(SZL_ID_0424_IDX_XXXX));
	FillTime(PTime);
	*PStatus=FServer->CpuStatus;
	SZL.SZLDone=true;
	isoSendBuffer(&SZL.Answer,22+sizeof(SZL_ID_0424_IDX_XXXX));
}

void TS7Worker::SZL_ID131_IDX003()
{
	word len = sizeof(SZL_ID_0131_IDX_0003);
	SZL.Answer.Header.DataLen=SwapWord(len);
	SZL.ResParams->Err  =0x0000;
	SZL.ResParams->resvd=0x0000; // this is the end, no more packets
	memcpy(SZL.ResData, &SZL_ID_0131_IDX_0003, len);
    // Set the max consistent data window to PDU size
	SZL.ResData[18]=((FPDULength)>>8) & 0xFF;
	SZL.ResData[19]=(FPDULength) & 0xFF;

	isoSendBuffer(&SZL.Answer,22+len);
	SZL.SZLDone=true;
}

bool TS7Worker::PerformGroupSZL()
{
  SZL.SZLDone=false;
  // Setup pointers
  SZL.ReqParams=PReqFunReadSZLFirst(pbyte(PDUH_in)+ReqHeaderSize);
  SZL.ResParams=PS7ResParams7(pbyte(&SZL.Answer)+ResHeaderSize17);
  SZL.ResData  =pbyte(&SZL.Answer)+ResHeaderSize17+sizeof(TS7Params7);
  // Prepare Answer header
  SZL.Answer.Header.P=0x32;
  SZL.Answer.Header.PDUType=PduType_userdata;
  SZL.Answer.Header.AB_EX=0x0000;
  SZL.Answer.Header.Sequence=PDUH_in->Sequence;
  SZL.Answer.Header.ParLen =SwapWord(sizeof(TS7Params7));

  SZL.ResParams->Head[0]=SZL.ReqParams->Head[0];
  SZL.ResParams->Head[1]=SZL.ReqParams->Head[1];
  SZL.ResParams->Head[2]=SZL.ReqParams->Head[2];
  SZL.ResParams->Plen  =0x08;
  SZL.ResParams->Uk    =0x12;
  SZL.ResParams->Tg    =0x84; // Type response + group szl
  SZL.ResParams->SubFun=SZL.ReqParams->SubFun;
  SZL.ResParams->Seq   =SZL.ReqParams->Seq;
  SZL.ResParams->resvd=0x0000; // this is the end, no more packets

  // only two subfunction are defined : 0x01 read, 0x02 system state
  if (SZL.ResParams->SubFun==0x02)   // 0x02 = subfunction system state
  {
      SZLSystemState();
      return true;
  };
  if (SZL.ResParams->SubFun!=0x01)
  {
      SZLNotAvailable();
      return true;
  };
  // From here we assume subfunction = 0x01
  SZL.ReqData=PS7ReqSZLData(pbyte(PDUH_in)+ReqHeaderSize+sizeof(TReqFunReadSZLFirst));// Data after params

  SZL.ID=SwapWord(SZL.ReqData->ID);
  SZL.Index=SwapWord(SZL.ReqData->Index);

  // Switch prebuilt Data Bank (they come from a physical CPU)
  switch (SZL.ID)
  {
    case 0x0000 : SZLData(&SZL_ID_0000_IDX_XXXX,sizeof(SZL_ID_0000_IDX_XXXX));break;
    case 0x0F00 : SZLData(&SZL_ID_0F00_IDX_XXXX,sizeof(SZL_ID_0F00_IDX_XXXX));break;
    case 0x0002 : SZLData(&SZL_ID_0002_IDX_XXXX,sizeof(SZL_ID_0002_IDX_XXXX));break;
    case 0x0011 : SZLData(&SZL_ID_0011_IDX_XXXX,sizeof(SZL_ID_0011_IDX_XXXX));break;
    case 0x0012 : SZLData(&SZL_ID_0012_IDX_XXXX,sizeof(SZL_ID_0012_IDX_XXXX));break;
    case 0x0013 : SZLData(&SZL_ID_0013_IDX_XXXX,sizeof(SZL_ID_0013_IDX_XXXX));break;
    case 0x0014 : SZLData(&SZL_ID_0014_IDX_XXXX,sizeof(SZL_ID_0014_IDX_XXXX));break;
    case 0x0015 : SZLData(&SZL_ID_0015_IDX_XXXX,sizeof(SZL_ID_0015_IDX_XXXX));break;
    case 0x0F14 : SZLData(&SZL_ID_0F14_IDX_XXXX,sizeof(SZL_ID_0F14_IDX_XXXX));break;
    case 0x0019 : SZLData(&SZL_ID_0019_IDX_XXXX,sizeof(SZL_ID_0019_IDX_XXXX));break;
    case 0x0F19 : SZLData(&SZL_ID_0F19_IDX_XXXX,sizeof(SZL_ID_0F19_IDX_XXXX));break;
    case 0x001C : SZLData(&SZL_ID_001C_IDX_XXXX,sizeof(SZL_ID_001C_IDX_XXXX));break;
    case 0x0F1C : SZLData(&SZL_ID_0F1C_IDX_XXXX,sizeof(SZL_ID_0F1C_IDX_XXXX));break;
    case 0x0036 : SZLData(&SZL_ID_0036_IDX_XXXX,sizeof(SZL_ID_0036_IDX_XXXX));break;
    case 0x0F36 : SZLData(&SZL_ID_0F36_IDX_XXXX,sizeof(SZL_ID_0F36_IDX_XXXX));break;
    case 0x0025 : SZLData(&SZL_ID_0025_IDX_XXXX,sizeof(SZL_ID_0025_IDX_XXXX));break;
    case 0x0F25 : SZLData(&SZL_ID_0F25_IDX_XXXX,sizeof(SZL_ID_0F25_IDX_XXXX));break;
    case 0x0037 : SZLData(&SZL_ID_0037_IDX_XXXX,sizeof(SZL_ID_0037_IDX_XXXX));break;
    case 0x0F37 : SZLData(&SZL_ID_0F37_IDX_XXXX,sizeof(SZL_ID_0F37_IDX_XXXX));break;
    case 0x0074 : SZLData(&SZL_ID_0074_IDX_XXXX,sizeof(SZL_ID_0074_IDX_XXXX));break;
    case 0x0F74 : SZLData(&SZL_ID_0F74_IDX_XXXX,sizeof(SZL_ID_0F74_IDX_XXXX));break;
    case 0x0591 : SZLData(&SZL_ID_0591_IDX_XXXX,sizeof(SZL_ID_0591_IDX_XXXX));break;
    case 0x0A91 : SZLData(&SZL_ID_0A91_IDX_XXXX,sizeof(SZL_ID_0A91_IDX_XXXX));break;
    case 0x0F92 : SZLData(&SZL_ID_0F92_IDX_XXXX,sizeof(SZL_ID_0F92_IDX_XXXX));break;
    case 0x0294 : SZLData(&SZL_ID_0294_IDX_XXXX,sizeof(SZL_ID_0294_IDX_XXXX));break;
    case 0x0794 : SZLData(&SZL_ID_0794_IDX_XXXX,sizeof(SZL_ID_0794_IDX_XXXX));break;
    case 0x0F94 : SZLData(&SZL_ID_0F94_IDX_XXXX,sizeof(SZL_ID_0F94_IDX_XXXX));break;
    case 0x0095 : SZLData(&SZL_ID_0095_IDX_XXXX,sizeof(SZL_ID_0095_IDX_XXXX));break;
    case 0x0F95 : SZLData(&SZL_ID_0F95_IDX_XXXX,sizeof(SZL_ID_0F95_IDX_XXXX));break;
    case 0x00A0 : SZLData(&SZL_ID_00A0_IDX_XXXX,sizeof(SZL_ID_00A0_IDX_XXXX));break;
    case 0x0FA0 : SZLData(&SZL_ID_0FA0_IDX_XXXX,sizeof(SZL_ID_0FA0_IDX_XXXX));break;
	case 0x0017 : SZLData(&SZL_ID_0017_IDX_XXXX,sizeof(SZL_ID_0017_IDX_XXXX));break;
    case 0x0F17 : SZLData(&SZL_ID_0F17_IDX_XXXX,sizeof(SZL_ID_0F17_IDX_XXXX));break;
    case 0x0018 : SZLData(&SZL_ID_0018_IDX_XXXX,sizeof(SZL_ID_0018_IDX_XXXX));break;
    case 0x0F18 : SZLData(&SZL_ID_0F18_IDX_XXXX,sizeof(SZL_ID_0F18_IDX_XXXX));break;
    case 0x001A : SZLData(&SZL_ID_001A_IDX_XXXX,sizeof(SZL_ID_001A_IDX_XXXX));break;
    case 0x0F1A : SZLData(&SZL_ID_0F1A_IDX_XXXX,sizeof(SZL_ID_0F1A_IDX_XXXX));break;
    case 0x001B : SZLData(&SZL_ID_001B_IDX_XXXX,sizeof(SZL_ID_001B_IDX_XXXX));break;
    case 0x0F1B : SZLData(&SZL_ID_0F1B_IDX_XXXX,sizeof(SZL_ID_0F1B_IDX_XXXX));break;
    case 0x0021 : SZLData(&SZL_ID_0021_IDX_XXXX,sizeof(SZL_ID_0021_IDX_XXXX));break;
    case 0x0A21 : SZLData(&SZL_ID_0A21_IDX_XXXX,sizeof(SZL_ID_0A21_IDX_XXXX));break;
    case 0x0F21 : SZLData(&SZL_ID_0F21_IDX_XXXX,sizeof(SZL_ID_0F21_IDX_XXXX));break;
    case 0x0023 : SZLData(&SZL_ID_0023_IDX_XXXX,sizeof(SZL_ID_0023_IDX_XXXX));break;
    case 0x0F23 : SZLData(&SZL_ID_0F23_IDX_XXXX,sizeof(SZL_ID_0F23_IDX_XXXX));break;
    case 0x0024 : SZLData(&SZL_ID_0024_IDX_XXXX,sizeof(SZL_ID_0024_IDX_XXXX));break;
    case 0x0124 : SZLData(&SZL_ID_0124_IDX_XXXX,sizeof(SZL_ID_0124_IDX_XXXX));break;
    case 0x0424 : SZL_ID424();break;
    case 0x0038 : SZLData(&SZL_ID_0038_IDX_XXXX,sizeof(SZL_ID_0038_IDX_XXXX));break;
    case 0x0F38 : SZLData(&SZL_ID_0F38_IDX_XXXX,sizeof(SZL_ID_0F38_IDX_XXXX));break;
    case 0x003A : SZLData(&SZL_ID_003A_IDX_XXXX,sizeof(SZL_ID_003A_IDX_XXXX));break;
    case 0x0F3A : SZLData(&SZL_ID_0F3A_IDX_XXXX,sizeof(SZL_ID_0F3A_IDX_XXXX));break;
    case 0x0F9A : SZLData(&SZL_ID_0F9A_IDX_XXXX,sizeof(SZL_ID_0F9A_IDX_XXXX));break;
    case 0x0D91 : switch(SZL.Index){
                    case 0x0000 : SZLData(&SZL_ID_0D91_IDX_0000,sizeof(SZL_ID_0D91_IDX_0000));break;
                    default: SZLNotAvailable();break;
                  };
                  break;
    case 0x0092 : switch(SZL.Index){
                    case 0x0000 : SZLData(&SZL_ID_0092_IDX_0000,sizeof(SZL_ID_0092_IDX_0000));break;
                    default     : SZLNotAvailable();break;
                  };break;
    case 0x0292 : switch(SZL.Index){
                    case 0x0000 : SZLData(&SZL_ID_0292_IDX_0000,sizeof(SZL_ID_0292_IDX_0000));break;
                    default     : SZLNotAvailable();break;
                  };break;
    case 0x0692 : switch(SZL.Index){
                    case 0x0000 : SZLData(&SZL_ID_0692_IDX_0000,sizeof(SZL_ID_0692_IDX_0000));break;
                    default     : SZLNotAvailable();break;
                  };break;
	case 0x0094 : switch(SZL.Index){
                    case 0x0000 : SZLData(&SZL_ID_0094_IDX_0000,sizeof(SZL_ID_0094_IDX_0000));break;
                    default     : SZLNotAvailable();break;
                  };break;
    case 0x0D97 : switch(SZL.Index){
                    case 0x0000 : SZLData(&SZL_ID_0D97_IDX_0000,sizeof(SZL_ID_0D97_IDX_0000));break;
                    default     : SZLNotAvailable();break;
                  };break;
    case 0x0111 : switch(SZL.Index){
                    case 0x0001 : SZLData(&SZL_ID_0111_IDX_0001,sizeof(SZL_ID_0111_IDX_0001));break;
                    case 0x0006 : SZLData(&SZL_ID_0111_IDX_0006,sizeof(SZL_ID_0111_IDX_0006));break;
                    case 0x0007 : SZLData(&SZL_ID_0111_IDX_0007,sizeof(SZL_ID_0111_IDX_0007));break;
                    default     : SZLNotAvailable();break;
                  };break;
    case 0x0F11 : switch(SZL.Index){
                    case 0x0001 : SZLData(&SZL_ID_0F11_IDX_0001,sizeof(SZL_ID_0F11_IDX_0001));break;
                    case 0x0006 : SZLData(&SZL_ID_0F11_IDX_0006,sizeof(SZL_ID_0F11_IDX_0006));break;
                    case 0x0007 : SZLData(&SZL_ID_0F11_IDX_0007,sizeof(SZL_ID_0F11_IDX_0007));break;
                    default     : SZLNotAvailable();break;
                  };break;
    case 0x0112 : switch(SZL.Index){
                    case 0x0000 : SZLData(&SZL_ID_0112_IDX_0000,sizeof(SZL_ID_0112_IDX_0000));break;
                    case 0x0100 : SZLData(&SZL_ID_0112_IDX_0100,sizeof(SZL_ID_0112_IDX_0100));break;
                    case 0x0200 : SZLData(&SZL_ID_0112_IDX_0200,sizeof(SZL_ID_0112_IDX_0200));break;
                    case 0x0400 : SZLData(&SZL_ID_0112_IDX_0400,sizeof(SZL_ID_0112_IDX_0400));break;
                    default     : SZLNotAvailable();break;
                  };break;
    case 0x0F12 : switch(SZL.Index){
                   case 0x0000 : SZLData(&SZL_ID_0F12_IDX_0000,sizeof(SZL_ID_0F12_IDX_0000));break;
                   case 0x0100 : SZLData(&SZL_ID_0F12_IDX_0100,sizeof(SZL_ID_0F12_IDX_0100));break;
                   case 0x0200 : SZLData(&SZL_ID_0F12_IDX_0200,sizeof(SZL_ID_0F12_IDX_0200));break;
                   case 0x0400 : SZLData(&SZL_ID_0F12_IDX_0400,sizeof(SZL_ID_0F12_IDX_0400));break;
                   default     : SZLNotAvailable();break;
                  };break;
    case 0x0113 : switch(SZL.Index){
                    case 0x0001 : SZLData(&SZL_ID_0113_IDX_0001,sizeof(SZL_ID_0113_IDX_0001));break;
                    default     : SZLNotAvailable();break;
                  };break;
	case 0x0115 : switch(SZL.Index){
                    case 0x0800 : SZLData(&SZL_ID_0115_IDX_0800,sizeof(SZL_ID_0115_IDX_0800));break;
                    default     : SZLNotAvailable();break;
                  };break;
    case 0x011C : switch(SZL.Index){
                    case 0x0001 : SZLData(&SZL_ID_011C_IDX_0001,sizeof(SZL_ID_011C_IDX_0001));break;
                    case 0x0002 : SZLData(&SZL_ID_011C_IDX_0002,sizeof(SZL_ID_011C_IDX_0002));break;
                    case 0x0003 : SZLData(&SZL_ID_011C_IDX_0003,sizeof(SZL_ID_011C_IDX_0003));break;
                    case 0x0004 : SZLData(&SZL_ID_011C_IDX_0004,sizeof(SZL_ID_011C_IDX_0004));break;
                    case 0x0005 : SZLData(&SZL_ID_011C_IDX_0005,sizeof(SZL_ID_011C_IDX_0005));break;
                    case 0x0007 : SZLData(&SZL_ID_011C_IDX_0007,sizeof(SZL_ID_011C_IDX_0007));break;
                    case 0x0008 : SZLData(&SZL_ID_011C_IDX_0008,sizeof(SZL_ID_011C_IDX_0008));break;
                    case 0x0009 : SZLData(&SZL_ID_011C_IDX_0009,sizeof(SZL_ID_011C_IDX_0009));break;
                    case 0x000A : SZLData(&SZL_ID_011C_IDX_000A,sizeof(SZL_ID_011C_IDX_000A));break;
                    case 0x000B : SZLData(&SZL_ID_011C_IDX_000B,sizeof(SZL_ID_011C_IDX_000B));break;
                    default     : SZLNotAvailable();break;
                  };break;
    case 0x0222 : switch(SZL.Index){
                    case 0x0001 : SZLData(&SZL_ID_0222_IDX_0001,sizeof(SZL_ID_0222_IDX_0001));break;
                    case 0x000A : SZLData(&SZL_ID_0222_IDX_000A,sizeof(SZL_ID_0222_IDX_000A));break;
                    case 0x0014 : SZLData(&SZL_ID_0222_IDX_0014,sizeof(SZL_ID_0222_IDX_0014));break;
                    case 0x0028 : SZLData(&SZL_ID_0222_IDX_0028,sizeof(SZL_ID_0222_IDX_0028));break;
                    case 0x0050 : SZLData(&SZL_ID_0222_IDX_0050,sizeof(SZL_ID_0222_IDX_0050));break;
                    case 0x0064 : SZLData(&SZL_ID_0222_IDX_0064,sizeof(SZL_ID_0222_IDX_0064));break;
                    default     : SZLNotAvailable();break;
                  };break;
    case 0x0125 : switch(SZL.Index){
                    case 0x0000 : SZLData(&SZL_ID_0125_IDX_0000,sizeof(SZL_ID_0125_IDX_0000));break;
                    case 0x0001 : SZLData(&SZL_ID_0125_IDX_0001,sizeof(SZL_ID_0125_IDX_0001));break;
                    default     : SZLNotAvailable();break;
                  };break;
    case 0x0225 : switch(SZL.Index){
                    case 0x0001 : SZLData(&SZL_ID_0225_IDX_0001,sizeof(SZL_ID_0225_IDX_0001));break;
                    default     : SZLNotAvailable();break;
                  };break;
    case 0x0131 : switch(SZL.Index){
					case 0x0001 : SZLData(&SZL_ID_0131_IDX_0001,sizeof(SZL_ID_0131_IDX_0001));break;
					case 0x0002 : SZLData(&SZL_ID_0131_IDX_0002,sizeof(SZL_ID_0131_IDX_0002));break;
					case 0x0003 : SZL_ID131_IDX003();break;
                    case 0x0004 : SZLData(&SZL_ID_0131_IDX_0004,sizeof(SZL_ID_0131_IDX_0004));break;
                    case 0x0005 : SZLData(&SZL_ID_0131_IDX_0005,sizeof(SZL_ID_0131_IDX_0005));break;
                    case 0x0006 : SZLData(&SZL_ID_0131_IDX_0006,sizeof(SZL_ID_0131_IDX_0006));break;
                    case 0x0007 : SZLData(&SZL_ID_0131_IDX_0007,sizeof(SZL_ID_0131_IDX_0007));break;
                    case 0x0008 : SZLData(&SZL_ID_0131_IDX_0008,sizeof(SZL_ID_0131_IDX_0008));break;
                    case 0x0009 : SZLData(&SZL_ID_0131_IDX_0009,sizeof(SZL_ID_0131_IDX_0009));break;
                    default     : SZLNotAvailable();break;
                  };break;
    case 0x0117 : switch(SZL.Index){
                     case 0x0000 : SZLData(&SZL_ID_0117_IDX_0000,sizeof(SZL_ID_0117_IDX_0000));break;
                     case 0x0001 : SZLData(&SZL_ID_0117_IDX_0001,sizeof(SZL_ID_0117_IDX_0001));break;
                     case 0x0002 : SZLData(&SZL_ID_0117_IDX_0002,sizeof(SZL_ID_0117_IDX_0002));break;
                     case 0x0003 : SZLData(&SZL_ID_0117_IDX_0003,sizeof(SZL_ID_0117_IDX_0003));break;
                     case 0x0004 : SZLData(&SZL_ID_0117_IDX_0004,sizeof(SZL_ID_0117_IDX_0004));break;
                     default     : SZLNotAvailable();break;
                   };break;
    case 0x0118 : switch(SZL.Index){
                     case 0x0000 : SZLData(&SZL_ID_0118_IDX_0000,sizeof(SZL_ID_0118_IDX_0000));break;
                     case 0x0001 : SZLData(&SZL_ID_0118_IDX_0001,sizeof(SZL_ID_0118_IDX_0001));break;
                     case 0x0002 : SZLData(&SZL_ID_0118_IDX_0002,sizeof(SZL_ID_0118_IDX_0002));break;
                     case 0x0003 : SZLData(&SZL_ID_0118_IDX_0003,sizeof(SZL_ID_0118_IDX_0003));break;
                     default     : SZLNotAvailable();break;
                   };break;
    case 0x0132 : switch(SZL.Index){
                     case 0x0001 : SZLData(&SZL_ID_0132_IDX_0001,sizeof(SZL_ID_0132_IDX_0001));break;
                     case 0x0002 : SZLData(&SZL_ID_0132_IDX_0002,sizeof(SZL_ID_0132_IDX_0002));break;
                     case 0x0003 : SZLData(&SZL_ID_0132_IDX_0003,sizeof(SZL_ID_0132_IDX_0003));break;
                     case 0x0004 : SZLData(&SZL_ID_0132_IDX_0004,sizeof(SZL_ID_0132_IDX_0004));break;
                     case 0x0005 : SZLData(&SZL_ID_0132_IDX_0005,sizeof(SZL_ID_0132_IDX_0005));break;
                     case 0x0006 : SZLData(&SZL_ID_0132_IDX_0006,sizeof(SZL_ID_0132_IDX_0006));break;
                     case 0x0007 : SZLData(&SZL_ID_0132_IDX_0007,sizeof(SZL_ID_0132_IDX_0007));break;
                     case 0x0008 : SZLData(&SZL_ID_0132_IDX_0008,sizeof(SZL_ID_0132_IDX_0008));break;
                     case 0x0009 : SZLData(&SZL_ID_0132_IDX_0009,sizeof(SZL_ID_0132_IDX_0009));break;
                     case 0x000A : SZLData(&SZL_ID_0132_IDX_000A,sizeof(SZL_ID_0132_IDX_000A));break;
                     case 0x000B : SZLData(&SZL_ID_0132_IDX_000B,sizeof(SZL_ID_0132_IDX_000B));break;
                     case 0x000C : SZLData(&SZL_ID_0132_IDX_000C,sizeof(SZL_ID_0132_IDX_000C));break;
                     default     : SZLNotAvailable();break;
                   };break;
    case 0x0137 : switch(SZL.Index){
                    case 0x07FE : SZLData(&SZL_ID_0137_IDX_07FE,sizeof(SZL_ID_0137_IDX_07FE));break;
                    default     : SZLNotAvailable();break;
                  };break;
    case 0x01A0 : switch(SZL.Index){
                     case 0x0000 : SZLData(&SZL_ID_01A0_IDX_0000,sizeof(SZL_ID_01A0_IDX_0000));break;
                     case 0x0001 : SZLData(&SZL_ID_01A0_IDX_0001,sizeof(SZL_ID_01A0_IDX_0001));break;
                     case 0x0002 : SZLData(&SZL_ID_01A0_IDX_0002,sizeof(SZL_ID_01A0_IDX_0002));break;
                     case 0x0003 : SZLData(&SZL_ID_01A0_IDX_0003,sizeof(SZL_ID_01A0_IDX_0003));break;
                     case 0x0004 : SZLData(&SZL_ID_01A0_IDX_0004,sizeof(SZL_ID_01A0_IDX_0004));break;
                     case 0x0005 : SZLData(&SZL_ID_01A0_IDX_0005,sizeof(SZL_ID_01A0_IDX_0005));break;
                     case 0x0006 : SZLData(&SZL_ID_01A0_IDX_0006,sizeof(SZL_ID_01A0_IDX_0006));break;
                     case 0x0007 : SZLData(&SZL_ID_01A0_IDX_0007,sizeof(SZL_ID_01A0_IDX_0007));break;
                     case 0x0008 : SZLData(&SZL_ID_01A0_IDX_0008,sizeof(SZL_ID_01A0_IDX_0008));break;
                     case 0x0009 : SZLData(&SZL_ID_01A0_IDX_0009,sizeof(SZL_ID_01A0_IDX_0009));break;
                     case 0x000A : SZLData(&SZL_ID_01A0_IDX_000A,sizeof(SZL_ID_01A0_IDX_000A));break;
                     case 0x000B : SZLData(&SZL_ID_01A0_IDX_000B,sizeof(SZL_ID_01A0_IDX_000B));break;
                     case 0x000C : SZLData(&SZL_ID_01A0_IDX_000C,sizeof(SZL_ID_01A0_IDX_000C));break;
                     case 0x000D : SZLData(&SZL_ID_01A0_IDX_000D,sizeof(SZL_ID_01A0_IDX_000D));break;
                     case 0x000E : SZLData(&SZL_ID_01A0_IDX_000E,sizeof(SZL_ID_01A0_IDX_000E));break;
                     case 0x000F : SZLData(&SZL_ID_01A0_IDX_000F,sizeof(SZL_ID_01A0_IDX_000F));break;
                     case 0x0010 : SZLData(&SZL_ID_01A0_IDX_0010,sizeof(SZL_ID_01A0_IDX_0010));break;
                     case 0x0011 : SZLData(&SZL_ID_01A0_IDX_0011,sizeof(SZL_ID_01A0_IDX_0011));break;
                     case 0x0012 : SZLData(&SZL_ID_01A0_IDX_0012,sizeof(SZL_ID_01A0_IDX_0012));break;
                     case 0x0013 : SZLData(&SZL_ID_01A0_IDX_0013,sizeof(SZL_ID_01A0_IDX_0013));break;
                     case 0x0014 : SZLData(&SZL_ID_01A0_IDX_0014,sizeof(SZL_ID_01A0_IDX_0014));break;
                     case 0x0015 : SZLData(&SZL_ID_01A0_IDX_0015,sizeof(SZL_ID_01A0_IDX_0015));break;
                     default     : SZLNotAvailable();break;
                   };break;
    case 0x0174 : switch(SZL.Index){
                    case 0x0001 : SZLData(&SZL_ID_0174_IDX_0001,sizeof(SZL_ID_0174_IDX_0001));break;
                    case 0x0004 : SZLData(&SZL_ID_0174_IDX_0004,sizeof(SZL_ID_0174_IDX_0004));break;
                    case 0x0005 : SZLData(&SZL_ID_0174_IDX_0005,sizeof(SZL_ID_0174_IDX_0005));break;
                    case 0x0006 : SZLData(&SZL_ID_0174_IDX_0006,sizeof(SZL_ID_0174_IDX_0006));break;
                    case 0x000B : SZLData(&SZL_ID_0174_IDX_000B,sizeof(SZL_ID_0174_IDX_000B));break;
                    case 0x000C : SZLData(&SZL_ID_0174_IDX_000C,sizeof(SZL_ID_0174_IDX_000C));break;
                    default     : SZLNotAvailable();break;
                  };break;
    case 0x0194 : switch(SZL.Index){
                    case 0x0064 : SZLData(&SZL_ID_0194_IDX_0064,sizeof(SZL_ID_0194_IDX_0064));break;
                    default     : SZLNotAvailable();break;
                  };break;
    case 0x0694 : switch(SZL.Index){
                    case 0x0064 : SZLData(&SZL_ID_0694_IDX_0064,sizeof(SZL_ID_0694_IDX_0064));break;
                    default     : SZLNotAvailable();break;
                  };break;
    case 0x0232 : switch(SZL.Index){
                     case 0x0001 : SZLData(&SZL_ID_0232_IDX_0001,sizeof(SZL_ID_0232_IDX_0001));break;
                     case 0x0004 : SZLData(&SZL_ID_0232_IDX_0004,sizeof(SZL_ID_0232_IDX_0004));break;
                     default     : SZLNotAvailable();break;
                   };break;
    case 0x0C91 : switch(SZL.Index){
                    case 0x07FE : SZLData(&SZL_ID_0C91_IDX_07FE,sizeof(SZL_ID_0C91_IDX_07FE));break;
                    default     : SZLNotAvailable();break;
                  };break;
    default : SZLNotAvailable();break;
  }
  // Event
  if (SZL.SZLDone)
      DoEvent(evcReadSZL,evrNoError,SZL.ID,SZL.Index,0,0);
  else
      DoEvent(evcReadSZL,evrInvalidSZL,SZL.ID,SZL.Index,0,0);
  return true;
}
//------------------------------------------------------------------------------
bool TS7Worker::PerformGroupSecurity()
{
    PReqFunSecurity ReqParams;
    PResParamsSecurity ResParams;
    PResDataSecurity ResData;
    TS7Answer17 Answer;
    int TotalSize;

    ReqParams=PReqFunSecurity(pbyte(PDUH_in)+ReqHeaderSize);
    ResParams=PResParamsSecurity(pbyte(&Answer)+ResHeaderSize17);
    ResData  =PResDataSecurity(pbyte(ResParams)+sizeof(TResParamsSecurity));

    // Prepares the answer
    Answer.Header.P=0x32;
    Answer.Header.PDUType=PduType_userdata;
    Answer.Header.AB_EX=0x0000;
    Answer.Header.Sequence=PDUH_in->Sequence;
    Answer.Header.ParLen =SwapWord(sizeof(TResParamsSecurity));
    Answer.Header.DataLen=SwapWord(0x0004);
    // Params
    ResParams->Head[0]=ReqParams->Head[0];
    ResParams->Head[1]=ReqParams->Head[1];
    ResParams->Head[2]=ReqParams->Head[2];
    ResParams->Plen  =0x08;
    ResParams->Uk    =0x12;
    ResParams->Tg    =0x85; // Type response, group functions info
    ResParams->SubFun=ReqParams->SubFun;
    ResParams->Seq   =ReqParams->Seq;
    ResParams->resvd =0x0000;
    ResParams->Err   =0x0000;
    // Data
    ResData->Ret =0x0A;
    ResData->TS  =0x00;
    ResData->DLen=0x0000;

    TotalSize=26;
    isoSendBuffer(&Answer,TotalSize);

    switch (ReqParams->SubFun)
    {
      case SFun_EnterPwd  : DoEvent(evcSecurity,evrNoError,evsSetPassword,0,0,0); break;
      case SFun_CancelPwd : DoEvent(evcSecurity,evrNoError,evsClrPassword,0,0,0); break;
      default             : DoEvent(evcSecurity,evrNoError,evsUnknown,0,0,0);
    };

    return true;
}
//------------------------------------------------------------------------------
bool TS7Worker::PerformGetClock()
{
    PS7ReqParams7 ReqParams;
    PS7ResParams7 ResParams;
    TS7Answer17 Answer;
    PResDataGetTime Data;
    PS7Time PTime;
    int TotalSize;

    ReqParams=PS7ReqParams7(pbyte(PDUH_in)+ReqHeaderSize);
    ResParams=PS7ResParams7(pbyte(&Answer)+ResHeaderSize17);
    Data     =PResDataGetTime(pbyte(&Answer)+ResHeaderSize17+sizeof(TS7Params7));
    PTime    =PS7Time(pbyte(Data)+6);

    // Prepares the answer
    Answer.Header.P=0x32;
    Answer.Header.PDUType=PduType_userdata;
    Answer.Header.AB_EX=0x0000;
    Answer.Header.Sequence=PDUH_in->Sequence;
    Answer.Header.ParLen =SwapWord(sizeof(TS7Params7));
    Answer.Header.DataLen=SwapWord(sizeof(TResDataGetTime));

    ResParams->Head[0]=ReqParams->Head[0];
    ResParams->Head[1]=ReqParams->Head[1];
    ResParams->Head[2]=ReqParams->Head[2];
    ResParams->Plen  =0x08;
    ResParams->Uk    =0x12;
    ResParams->Tg    =0x87; // Type response, group functions info
    ResParams->SubFun=ReqParams->SubFun;
    ResParams->Seq   =ReqParams->Seq;
    ResParams->resvd =0x0000;
    ResParams->Err   =0x0000;

    Data->RetVal     =0xFF;
    Data->TSize      =TS_ResOctet;
    Data->Length     =SwapWord(10);
    Data->Rsvd       =0x00;
    Data->HiYear     =0x20; // Year 2000 +
    FillTime(PTime);

    TotalSize=36;
    isoSendBuffer(&Answer,TotalSize);
    DoEvent(evcClock,evrNoError,evsGetClock,0,0,0);
    return true;
}
//------------------------------------------------------------------------------
bool TS7Worker::PerformSetClock()
{
    PS7ReqParams7 ReqParams;
    PS7ResParams7 ResParams;
    PResDataSetTime Data;
    TS7Answer17 Answer;
    int TotalSize;

    ReqParams=PS7ReqParams7(pbyte(PDUH_in)+ReqHeaderSize);
    ResParams=PS7ResParams7(pbyte(&Answer)+ResHeaderSize17);
    Data     =PResDataSetTime(pbyte(&Answer)+ResHeaderSize17+sizeof(TS7Params7));

    // Prepares the answer
    Answer.Header.P=0x32;
    Answer.Header.PDUType=PduType_userdata;
    Answer.Header.AB_EX=0x0000;
    Answer.Header.Sequence=PDUH_in->Sequence;
    Answer.Header.ParLen =SwapWord(sizeof(TS7Params7));
    Answer.Header.DataLen=SwapWord(sizeof(TResDataSetTime));

    ResParams->Head[0]=ReqParams->Head[0];
    ResParams->Head[1]=ReqParams->Head[1];
    ResParams->Head[2]=ReqParams->Head[2];
    ResParams->Plen  =0x08;
    ResParams->Uk    =0x12;
    ResParams->Tg    =0x87; // Type response, group functions info
    ResParams->SubFun=ReqParams->SubFun;
    ResParams->Seq   =ReqParams->Seq;
    ResParams->resvd =0x0000;
    ResParams->Err   =0x0000;

    Data->RetVal     =0x0A;
    Data->TSize      =0x00;
    Data->Length     =0x0000;

    TotalSize=26;
    isoSendBuffer(&Answer,TotalSize);
    DoEvent(evcClock,evrNoError,evsSetClock,0,0,0);
    return true;
}
//------------------------------------------------------------------------------
// S7 SERVER CLASS
//------------------------------------------------------------------------------
TSnap7Server::TSnap7Server()
{
	CSRWHook = new TSnapCriticalSection();
	OnReadEvent=NULL;
	memset(&DB,0,sizeof(DB));
    memset(&HA,0,sizeof(HA));
    DBCount=0;
    DBLimit=0;
	ForcePDU = 0;
	ResourceLess = false;
    LocalPort=isoTcpPort;
    CpuStatus=S7CpuStatusRun;
    WorkInterval=100;
}
//------------------------------------------------------------------------------
TSnap7Server::~TSnap7Server()
{
    DisposeAll();
	delete CSRWHook;
}
//------------------------------------------------------------------------------
PWorkerSocket TSnap7Server::CreateWorkerSocket(socket_t Sock)
{
    PWorkerSocket Result;
    Result = new TS7Worker();
    Result->SetSocket(Sock);
    PS7Worker(Result)->FServer=this;
    return Result;
}
//------------------------------------------------------------------------------
PS7Area TSnap7Server::FindDB(word DBNumber)
{
    int c;
    int max=DBLimit+1;

    for (c=0; c<max; c++)
    {
        if (DB[c]!=NULL)
        {
	        if (DB[c]->Number==DBNumber)
	        {
                return DB[c];
	        }
	    }
    }
    return NULL;
}
//------------------------------------------------------------------------------
int TSnap7Server::IndexOfDB(word DBNumber)
{
    int c;
    int max=DBLimit+1;

    for (c=0; c<max; c++)
    {
		if (DB[c]!=NULL)
		{
			if (DB[c]->Number==DBNumber)
			{
				return c;
			}
		}
    }
    return -1;
}
//------------------------------------------------------------------------------
int TSnap7Server::FindFirstFreeDB()
{
    int c;
    for (c=0; c < MaxDB; c++)
    {
        if (DB[c]==NULL)
	        return c;
    }
    return -1;
}
//------------------------------------------------------------------------------
int TSnap7Server::RegisterDB(word Number, void *pUsrData, word Size)
{
    PS7Area TheArea;
    int index;

    if (pUsrData==0)
        return errSrvDBNullPointer;

    if (FindDB(Number)!=NULL)
        return errSrvAreaAlreadyExists;

    index=FindFirstFreeDB();
    if (index==-1)
        return errSrvTooManyDB;

    TheArea =new TS7Area;
    TheArea->Number=Number;
    TheArea->cs=new TSnapCriticalSection();
    TheArea->PData=pbyte(pUsrData);
    TheArea->Size=Size;
    DB[index]=TheArea;
    DBCount++;
    if (DBLimit<index)
        DBLimit=index;
    return 0;
}
//------------------------------------------------------------------------------
void TSnap7Server::DisposeAll()
{
    PS7Area TheDB;
    int c;
    // Unregister DBs
    for (c = 0; c < MaxDB; c++)
    {
		if (DB[c]!=NULL)
		{
			// Unregister should be done with the server in stop mode
			// however we can minimize the risk...
			TheDB=DB[c];
			DB[c]=NULL;
			if (TheDB->cs!=0)
					delete TheDB->cs;
			delete TheDB;
		}
    }
    DBCount=0;
    // Unregister other
    for (c = srvAreaPE; c < srvAreaDB; c++)
        UnregisterSys(c);
}
//------------------------------------------------------------------------------
int TSnap7Server::RegisterSys(int AreaCode, void *pUsrData, word Size)
{
    PS7Area TheArea;

    if (pUsrData==0)
        return errSrvDBNullPointer;

    if ((AreaCode<srvAreaPE) || (AreaCode>srvAreaTM))
        return errSrvUnknownArea;

    if (HA[AreaCode]==0)
    {
	TheArea=new TS7Area;
	TheArea->cs=new TSnapCriticalSection();
	TheArea->PData=pbyte(pUsrData);
	TheArea->Size=Size;
	HA[AreaCode]=TheArea;
	return 0;
    }
    else
        return errSrvAreaAlreadyExists;
}
//------------------------------------------------------------------------------
int TSnap7Server::UnregisterDB(word DBNumber)
{
    PS7Area TheDB;
    int index = IndexOfDB(DBNumber);
    if (index==-1)
        return errSrvInvalidParams;

    // Unregister should be done with the server in stop mode
    // however we can minimize the risk...
    TheDB=DB[index];
    DB[index]=NULL;
    if (TheDB->cs!=NULL)
        delete TheDB->cs;
    delete TheDB;
    DBCount--;

    return 0;
}
//------------------------------------------------------------------------------
int TSnap7Server::UnregisterSys(int AreaCode)
{
    PS7Area TheArea;
    if (HA[AreaCode]!=NULL)
    {
		// Unregister should be done with the server in stop mode
		// however we can minimize the risk...
		TheArea=HA[AreaCode];
		HA[AreaCode]=NULL;
		if (TheArea->cs!=NULL)
			 delete TheArea->cs;
		delete TheArea;
    }
    return 0;
}
//------------------------------------------------------------------------------
int TSnap7Server::StartTo(const char *Address)
{
    return TCustomMsgServer::StartTo(Address, LocalPort);
}
//------------------------------------------------------------------------------
int TSnap7Server::GetParam(int ParamNumber, void *pValue)
{
    switch (ParamNumber)
    {
	case p_u16_LocalPort:
            *Puint16_t(pValue)=LocalPort;
	    break;
	case p_i32_WorkInterval:
	    *Pint32_t(pValue)=WorkInterval;
	    break;
	case p_i32_MaxClients:
	    *Pint32_t(pValue)=MaxClients;
	    break;
	case p_i32_PDURequest:
		*Pint32_t(pValue) = ForcePDU;
		break;
	default: return errSrvInvalidParamNumber;
    }
    return 0;
}
//------------------------------------------------------------------------------
int TSnap7Server::SetParam(int ParamNumber, void *pValue)
{
	switch (ParamNumber)
	{
	case p_u16_LocalPort:
		if (Status == SrvStopped)
			LocalPort = *Puint16_t(pValue);
		else
			return errSrvCannotChangeParam;
		break;
	case p_i32_PDURequest:
		if (Status == SrvStopped)
		{
			int PDU = *Pint32_t(pValue);
			if (PDU == 0)
				ForcePDU = 0; // ForcePDU=0 --> The server accepts the client's proposal
			else
				{
					if ((PDU < MinPduSize) || (PDU>IsoPayload_Size))
						return errSrvInvalidParams; // Wrong value
					else
						ForcePDU = PDU; // The server imposes ForcePDU as PDU size
				}
	    }
		else
            return errSrvCannotChangeParam;
		break;
	case p_i32_WorkInterval:
         WorkInterval=*Pint32_t(pValue);
	     break;
	case p_i32_MaxClients:
	     if (ClientsCount==0 && Status==SrvStopped)
	         MaxClients=*Pint32_t(pValue);
         else
	         return errSrvCannotChangeParam;
         break;
	default: return errSrvInvalidParamNumber;
    }
    return 0;
}
//------------------------------------------------------------------------------
int TSnap7Server::RegisterArea(int AreaCode, word Index, void *pUsrData, word Size)
{
    if (AreaCode==srvAreaDB)
        return RegisterDB(Index, pUsrData, Size);
    else
        return RegisterSys(AreaCode,pUsrData, Size);
}
//------------------------------------------------------------------------------
int TSnap7Server::UnregisterArea(int AreaCode, word Index)
{
    if (AreaCode==srvAreaDB)
        return UnregisterDB(Index);
    else
        if ((AreaCode>=srvAreaPE) && (AreaCode<=srvAreaTM))
            return UnregisterSys(AreaCode);
	else
        return errSrvInvalidParams;
}
//------------------------------------------------------------------------------
int TSnap7Server::LockArea(int AreaCode, word DBNumber)
{
  int index;

  if ((AreaCode>=srvAreaPE) && (AreaCode<=srvAreaTM))
  {
      if (HA[AreaCode]!=0)
      {
		  HA[AreaCode]->cs->Enter();
		  return 0;
      }
      else
		  return errSrvInvalidParams;
  }
  else
      if (AreaCode==srvAreaDB)
      {
		  index=IndexOfDB(DBNumber);
		  if (index!=-1)
	  {
	      DB[index]->cs->Enter();
	      return 0;
	  }
	  else
	      return errSrvInvalidParams;
      }
      else
		  return errSrvInvalidParams;
}
//------------------------------------------------------------------------------
int TSnap7Server::UnlockArea(int AreaCode, word DBNumber)
{
  int index;

  if ((AreaCode>=srvAreaPE) && (AreaCode<=srvAreaTM))
  {
      if (HA[AreaCode]!=0)
      {
		  HA[AreaCode]->cs->Leave();
		  return 0;
      }
      else
		  return errSrvInvalidParams;
  }
  else
      if (AreaCode==srvAreaDB)
      {
		  index=IndexOfDB(DBNumber);
		  if (index!=-1)
	  {
	      DB[index]->cs->Leave();
	      return 0;
	  }
	  else
	      return errSrvInvalidParams;
      }
      else
		  return errSrvInvalidParams;
}
//------------------------------------------------------------------------------
int TSnap7Server::SetReadEventsCallBack(pfn_SrvCallBack PCallBack, void *UsrPtr) 
{
    OnReadEvent = PCallBack;
    FReadUsrPtr = UsrPtr;
    return 0;
}
//---------------------------------------------------------------------------
int TSnap7Server::SetRWAreaCallBack(pfn_RWAreaCallBack PCallBack, void *UsrPtr)
{
	OnRWArea = PCallBack;
	FRWAreaUsrPtr = UsrPtr;
	ResourceLess = OnRWArea != NULL;
	return 0;
}
//---------------------------------------------------------------------------
void TSnap7Server::DoReadEvent(int Sender, longword Code, word RetCode, word Param1,
  word Param2, word Param3, word Param4)
{
    TSrvEvent SrvReadEvent;
    if (!Destroying && (OnReadEvent != NULL))
    {
        CSEvent->Enter();

        time(&SrvReadEvent.EvtTime);
        SrvReadEvent.EvtSender = Sender;
        SrvReadEvent.EvtCode = Code;
        SrvReadEvent.EvtRetCode = RetCode;
        SrvReadEvent.EvtParam1 = Param1;
        SrvReadEvent.EvtParam2 = Param2;
        SrvReadEvent.EvtParam3 = Param3;
        SrvReadEvent.EvtParam4 = Param4;

        try
        { // callback is outside here, we have to shield it
            OnReadEvent(FReadUsrPtr, &SrvReadEvent, sizeof (TSrvEvent));
        } catch (...)
        {
        };
        CSEvent->Leave();
    };
}
//---------------------------------------------------------------------------
bool TSnap7Server::DoReadArea(int Sender, int Area, int DBNumber, int Start, int Size, int WordLen, void *pUsrData)
{
	TS7Tag Tag;
	bool Result = false;
	if (!Destroying && (OnRWArea != NULL))
	{
		CSRWHook->Enter();
		try
		{ 
			Tag.Area = Area;
			Tag.DBNumber = DBNumber;
			Tag.Start = Start;
			Tag.Size = Size;
			Tag.WordLen = WordLen;
			// callback is outside here, we have to shield it
			Result = OnRWArea(FRWAreaUsrPtr, Sender, OperationRead, &Tag, pUsrData) == 0;
		}
		catch (...)
		{
			Result = false;
		};
		CSRWHook->Leave();
	}
	return Result;
}
//---------------------------------------------------------------------------
bool TSnap7Server::DoWriteArea(int Sender, int Area, int DBNumber, int Start, int Size, int WordLen, void *pUsrData)
{
	TS7Tag Tag;
	bool Result = false;
	if (!Destroying && (OnRWArea != NULL))
	{
		CSRWHook->Enter();
		try
		{ 
			Tag.Area = Area;
			Tag.DBNumber = DBNumber;
			Tag.Start = Start;
			Tag.Size = Size;
			Tag.WordLen = WordLen;
			// callback is outside here, we have to shield it
			Result = OnRWArea(FRWAreaUsrPtr, Sender, OperationWrite, &Tag, pUsrData) == 0;
		}
		catch (...)
		{
			Result = false;
		};
		CSRWHook->Leave();
	}
	return Result;
}


