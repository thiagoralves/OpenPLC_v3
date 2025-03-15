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
#include "s7_micro_client.h"
//---------------------------------------------------------------------------

TSnap7MicroClient::TSnap7MicroClient()
{
	SrcRef =0x0100; // RFC0983 states that SrcRef and DetRef should be 0
			// and, in any case, they are ignored.
			// S7 instead requires a number != 0
			// Libnodave uses 0x0100
			// S7Manager uses 0x0D00
                        // TIA Portal V12 uses 0x1D00
			// WinCC     uses 0x0300
			// Seems that every non zero value is good enough...
	DstRef  =0x0000;
	SrcTSap =0x0100;
	DstTSap =0x0000; // It's filled by connection functions
    ConnectionType = CONNTYPE_PG; // Default connection type
	memset(&Job,0,sizeof(TSnap7Job));
}
//---------------------------------------------------------------------------
TSnap7MicroClient::~TSnap7MicroClient()
{
    Destroying = true;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::opReadArea()
{
     PReqFunReadParams ReqParams;
     PResFunReadParams ResParams;
     PS7ResHeader23    Answer;
     PResFunReadItem   ResData;
     word              RPSize; // ReqParams size
     int WordSize;
     uintptr_t Offset;
     pbyte Target;
     int Address;
     int IsoSize;
     int Start;
     int MaxElements;  // Max elements that we can transfer in a PDU
     word NumElements; // Num of elements that we are asking for this telegram
     int TotElements;  // Total elements requested
     int Size;
     int Result;

     WordSize=DataSizeByte(Job.WordLen); // The size in bytes of an element that we are asking for
     if (WordSize==0)
        return errCliInvalidWordLen;
     // First check : params bounds
     if ((Job.Number<0) || (Job.Number>65535) || (Job.Start<0) || (Job.Amount<1))
        return errCliInvalidParams;
     // Second check : transport size
	 if ((Job.WordLen==S7WLBit) && (Job.Amount>1))
        return errCliInvalidTransportSize;
     // Request Params size
     RPSize    =sizeof(TReqFunReadItem)+2; // 1 item + FunRead + ItemsCount
     // Setup pointers (note : PDUH_out and PDU.Payload are the same pointer)
     ReqParams =PReqFunReadParams(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
     Answer    =PS7ResHeader23(&PDU.Payload);
     ResParams =PResFunReadParams(pbyte(Answer)+ResHeaderSize23);
     ResData   =PResFunReadItem(pbyte(ResParams)+sizeof(TResFunReadParams));
     // Each packet cannot exceed the PDU length (in bytes) negotiated, and moreover
     // we must ensure to transfer a "finite" number of item per PDU
     MaxElements=(PDULength-sizeof(TS7ResHeader23)-sizeof(TResFunReadParams)-4) / WordSize;
     TotElements=Job.Amount;
     Start      =Job.Start;
     Offset     =0;
     Result     =0;
     while ((TotElements>0) && (Result==0))
     {
          NumElements=TotElements;
          if (NumElements>MaxElements)
             NumElements=MaxElements;

          Target=pbyte(Job.pData)+Offset;
          //----------------------------------------------- Read next slice-----
          PDUH_out->P = 0x32;                    // Always 0x32
          PDUH_out->PDUType = PduType_request;   // 0x01
          PDUH_out->AB_EX = 0x0000;              // Always 0x0000
          PDUH_out->Sequence = GetNextWord();    // AutoInc
          PDUH_out->ParLen = SwapWord(RPSize);   // 14 bytes params
          PDUH_out->DataLen = 0x0000;            // No data

          ReqParams->FunRead = pduFuncRead;      // 0x04
          ReqParams->ItemsCount = 1;
          ReqParams->Items[0].ItemHead[0] = 0x12;
          ReqParams->Items[0].ItemHead[1] = 0x0A;
          ReqParams->Items[0].ItemHead[2] = 0x10;
          ReqParams->Items[0].TransportSize = Job.WordLen;
          ReqParams->Items[0].Length = SwapWord(NumElements);
          ReqParams->Items[0].Area = Job.Area;
          if (Job.Area==S7AreaDB)
               ReqParams->Items[0].DBNumber = SwapWord(Job.Number);
          else
               ReqParams->Items[0].DBNumber = 0x0000;
          // Adjusts the offset
          if ((Job.WordLen==S7WLBit) || (Job.WordLen==S7WLCounter) || (Job.WordLen==S7WLTimer))
               Address = Start;
          else
               Address = Start*8;

          ReqParams->Items[0].Address[2] = Address & 0x000000FF;
          Address = Address >> 8;
          ReqParams->Items[0].Address[1] = Address & 0x000000FF;
          Address = Address >> 8;
          ReqParams->Items[0].Address[0] = Address & 0x000000FF;

          IsoSize = sizeof(TS7ReqHeader)+RPSize;
          Result = isoExchangeBuffer(0,IsoSize);
          // Get Data
          if (Result==0)  // 1St level Iso
          {
               Size = 0;
               // Item level error
                if (ResData->ReturnCode==0xFF) // <-- 0xFF means Result OK
                {
                   // Calcs data size in bytes
                   Size = SwapWord(ResData->DataLength);
                   // Adjust Size in accord of TransportSize
                   if ((ResData->TransportSize != TS_ResOctet) && (ResData->TransportSize != TS_ResReal) && (ResData->TransportSize != TS_ResBit))
                       Size = Size >> 3;
                   memcpy(Target, &ResData->Data[0], Size);
                }
                else
                     Result = CpuError(ResData->ReturnCode);
           Offset+=Size;
          };
          //--------------------------------------------------------------------
          TotElements -= NumElements;
          Start += NumElements*WordSize;
     }
     return Result;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::opWriteArea()
{
     PReqFunWriteParams   ReqParams;
     PReqFunWriteDataItem ReqData;  // only 1 item for WriteArea Function
     PResFunWrite         ResParams;
     PS7ResHeader23       Answer;
     word RPSize;  // ReqParams size
     word RHSize;  // Request headers size
     bool First = true;
     pbyte Source;
     pbyte Target;
     int Address;
     int IsoSize;
     int WordSize;
     word Size;
     uintptr_t Offset = 0;
     int Start;        // where we are starting from for this telegram
     int MaxElements;  // Max elements that we can transfer in a PDU
     word NumElements; // Num of elements that we are asking for this telegram
     int TotElements;  // Total elements requested
     int Result = 0;

     WordSize=DataSizeByte(Job.WordLen); // The size in bytes of an element that we are pushing
     if (WordSize==0)
        return errCliInvalidWordLen;
     // First check : params bounds
     if ((Job.Number<0) || (Job.Number>65535) || (Job.Start<0) || (Job.Amount<1))
        return errCliInvalidParams;
     // Second check : transport size
	 if ((Job.WordLen==S7WLBit) && (Job.Amount>1))
        return errCliInvalidTransportSize;

     RHSize =sizeof(TS7ReqHeader)+    // Request header
             2+                       // FunWrite+ItemCount (of TReqFunWriteParams)
             sizeof(TReqFunWriteItem)+// 1 item reference
             4;                       // ReturnCode+TransportSize+DataLength
     RPSize =sizeof(TReqFunWriteItem)+2;

     // Setup pointers (note : PDUH_out and PDU.Payload are the same pointer)
     ReqParams=PReqFunWriteParams(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
     ReqData  =PReqFunWriteDataItem(pbyte(ReqParams)+sizeof(TReqFunWriteItem)+2); // 2 = FunWrite+ItemsCount
     Target   =pbyte(ReqData)+4; // 4 = ReturnCode+TransportSize+DataLength
     Answer   =PS7ResHeader23(&PDU.Payload);
     ResParams=PResFunWrite(pbyte(Answer)+ResHeaderSize23);

     // Each packet cannot exceed the PDU length (in bytes) negotiated, and moreover
     // we must ensure to transfer a "finite" number of item per PDU
     MaxElements=(PDULength-RHSize) / WordSize;
     TotElements=Job.Amount;
     Start      =Job.Start;
     while ((TotElements>0) && (Result==0))
     {
           NumElements=TotElements;
           if (NumElements>MaxElements)
               NumElements=MaxElements;
           Source=pbyte(Job.pData)+Offset;

           Size=NumElements * WordSize;
           PDUH_out->P=0x32;                    // Always 0x32
           PDUH_out->PDUType=PduType_request;   // 0x01
           PDUH_out->AB_EX=0x0000;              // Always 0x0000
           PDUH_out->Sequence=GetNextWord();    // AutoInc
           PDUH_out->ParLen  =SwapWord(RPSize); // 14 bytes params
           PDUH_out->DataLen =SwapWord(Size+4);

           ReqParams->FunWrite=pduFuncWrite;    // 0x05
           ReqParams->ItemsCount=1;
           ReqParams->Items[0].ItemHead[0]=0x12;
           ReqParams->Items[0].ItemHead[1]=0x0A;
           ReqParams->Items[0].ItemHead[2]=0x10;
           ReqParams->Items[0].TransportSize=Job.WordLen;
           ReqParams->Items[0].Length=SwapWord(NumElements);
           ReqParams->Items[0].Area=Job.Area;
           if (Job.Area==S7AreaDB)
               ReqParams->Items[0].DBNumber=SwapWord(Job.Number);
           else
               ReqParams->Items[0].DBNumber=0x0000;

           // Adjusts the offset
           if ((Job.WordLen==S7WLBit) || (Job.WordLen==S7WLCounter) || (Job.WordLen==S7WLTimer))
               Address=Start;
           else
               Address=Start*8;

           ReqParams->Items[0].Address[2]=Address & 0x000000FF;
           Address=Address >> 8;
           ReqParams->Items[0].Address[1]=Address & 0x000000FF;
           Address=Address >> 8;
           ReqParams->Items[0].Address[0]=Address & 0x000000FF;

           ReqData->ReturnCode=0x00;

           switch(Job.WordLen)
           {
               case S7WLBit:
                   ReqData->TransportSize=TS_ResBit;
                   break;
               case S7WLInt:
               case S7WLDInt:
                   ReqData->TransportSize=TS_ResInt;
                   break;
               case S7WLReal:
                   ReqData->TransportSize=TS_ResReal;
                   break;
               case S7WLChar   :
               case S7WLCounter:
               case S7WLTimer:
                   ReqData->TransportSize=TS_ResOctet;
				   break;
			   default:
                   ReqData->TransportSize=TS_ResByte;
                   break;
           };

           if ((ReqData->TransportSize!=TS_ResOctet) && (ReqData->TransportSize!=TS_ResReal) && (ReqData->TransportSize!=TS_ResBit))
               ReqData->DataLength=SwapWord(Size*8);
           else
               ReqData->DataLength=SwapWord(Size);

           memcpy(Target, Source, Size);
           IsoSize=RHSize + Size;
           Result=isoExchangeBuffer(0,IsoSize);

           if (Result==0) // 1St check : Iso result
           {
               Result=CpuError(SwapWord(Answer->Error)); // 2nd level global error
               if (Result==0)
               {     // 2th check : item error
                   if (ResParams->Data[0] == 0xFF) // <-- 0xFF means Result OK
                       Result=0;
                   else
                       // Now we check the error : if it's the first part we report the cpu error
                       // otherwise we warn that the function failed but some data were written
                       if (First)
                           Result=CpuError(ResParams->Data[0]);
                       else
                           Result=errCliPartialDataWritten;
               };
               Offset+=Size;
           };
           First=false;

           TotElements-=NumElements;
           Start+=(NumElements*WordSize);
     }
     return Result;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::opReadMultiVars()
{
    PS7DataItem       Item;
    PReqFunReadParams ReqParams;
    PS7ResHeader23    Answer;
    PResFunReadParams ResParams;
    TResFunReadData   ResData;

    word       RPSize; // ReqParams size
    uintptr_t  Offset =0 ;
    word       Slice;
    longword   Address;
    int        IsoSize;
    pbyte      P;
    int        ItemsCount, c, Result;

    Item       = PS7DataItem(Job.pData);
    ItemsCount = Job.Amount;

    // Some useful initial check to detail the errors (Since S7 CPU always answers
    // with $05 if (something is wrong in params)
    if (ItemsCount>MaxVars)
    	return errCliTooManyItems;

    // Adjusts Word Length in case of timers and counters and clears results
    for (c = 0; c < ItemsCount; c++)
    {
    	Item->Result=0;
        if (Item->Area==S7AreaCT)
          Item->WordLen=S7WLCounter;
        if (Item->Area==S7AreaTM)
          Item->WordLen=S7WLTimer;
        Item++;
    };

    // Let's build the PDU
    RPSize    = word(2 + ItemsCount * sizeof(TReqFunReadItem));
    ReqParams = PReqFunReadParams(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
    Answer    = PS7ResHeader23(&PDU.Payload);
    ResParams = PResFunReadParams(pbyte(Answer)+ResHeaderSize23);
    // Fill Header
    PDUH_out->P=0x32;                    // Always 0x32
    PDUH_out->PDUType=PduType_request;   // 0x01
    PDUH_out->AB_EX=0x0000;              // Always 0x0000
    PDUH_out->Sequence=GetNextWord();    // AutoInc
    PDUH_out->ParLen=SwapWord(RPSize);   // Request params size
    PDUH_out->DataLen=0x0000;            // No data in output

    // Fill Params
    ReqParams->FunRead=pduFuncRead;      // 0x04
    ReqParams->ItemsCount=ItemsCount;

    Item = PS7DataItem(Job.pData);
    for (c = 0; c < ItemsCount; c++)
    {
        ReqParams->Items[c].ItemHead[0]=0x12;
        ReqParams->Items[c].ItemHead[1]=0x0A;
        ReqParams->Items[c].ItemHead[2]=0x10;

        ReqParams->Items[c].TransportSize=Item->WordLen;
        ReqParams->Items[c].Length=SwapWord(Item->Amount);
        ReqParams->Items[c].Area=Item->Area;
        // Automatically drops DBNumber if (Area is not DB
        if (Item->Area==S7AreaDB)
	        ReqParams->Items[c].DBNumber=SwapWord(Item->DBNumber);
        else
    	    ReqParams->Items[c].DBNumber=0x0000;
        // Adjusts the offset
        if ((Item->WordLen==S7WLBit) || (Item->WordLen==S7WLCounter) || (Item->WordLen==S7WLTimer))
        	Address=Item->Start;
        else
        	Address=Item->Start*8;
        // Builds the offset
        ReqParams->Items[c].Address[2]=Address & 0x000000FF;
        Address=Address >> 8;
        ReqParams->Items[c].Address[1]=Address & 0x000000FF;
        Address=Address >> 8;
        ReqParams->Items[c].Address[0]=Address & 0x000000FF;
        Item++;
    };

    IsoSize=RPSize+sizeof(TS7ReqHeader);
	if (IsoSize>PDULength) 
		return errCliSizeOverPDU;
	Result=isoExchangeBuffer(0,IsoSize);

	if (Result!=0)
        return Result;

    // Function level error
    if (Answer->Error!=0)
    	return CpuError(SwapWord(Answer->Error));

    if (ResParams->ItemCount!=ItemsCount)
    	return errCliInvalidPlcAnswer;

    P=pbyte(ResParams)+sizeof(TResFunReadParams);
    Item = PS7DataItem(Job.pData);
    for (c = 0; c < ItemsCount; c++)
    {
        ResData[c] =PResFunReadItem(pbyte(P)+Offset);
        Slice=0;
        // Item level error
        if (ResData[c]->ReturnCode==0xFF) // <-- 0xFF means Result OK
        {
          // Calcs data size in bytes
          Slice=SwapWord(ResData[c]->DataLength);
          // Adjust Size in accord of TransportSize
          if ((ResData[c]->TransportSize != TS_ResOctet) && (ResData[c]->TransportSize != TS_ResReal) && (ResData[c]->TransportSize != TS_ResBit))
            Slice=Slice >> 3;

		  memcpy(Item->pdata, ResData[c]->Data, Slice);
          Item->Result=0;
        }
        else
          Item->Result=CpuError(ResData[c]->ReturnCode);

        if ((Slice % 2)!=0)
        	Slice++; // Skip fill byte for Odd frame

        Offset+=(4+Slice);
        Item++;
    };
    return Result;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::opWriteMultiVars()
{
    PS7DataItem        Item;
    PReqFunWriteParams ReqParams;
    PResFunWrite       ResParams;
    TReqFunWriteData   ReqData;
    PS7ResHeader23     Answer;
    pbyte              P;
    uintptr_t          Offset;
    longword           Address;
    int                ItemsCount, c, IsoSize;
    word               RPSize; // ReqParams size
    word               Size;   // Write data size
    int                WordSize, Result;

    Item       = PS7DataItem(Job.pData);
    ItemsCount = Job.Amount;

    // Some useful initial check to detail the errors (Since S7 CPU always answers
    // with $05 if (something is wrong in params)
    if (ItemsCount>MaxVars)
    	return errCliTooManyItems;

    // Adjusts Word Length in case of timers and counters and clears results
    for (c = 0; c < ItemsCount; c++)
    {
    	Item->Result=0;
        if (Item->Area==S7AreaCT)
          Item->WordLen=S7WLCounter;
        if (Item->Area==S7AreaTM)
          Item->WordLen=S7WLTimer;
        Item++;
    };

    // Let's build the PDU : setup pointers
    RPSize    = word(2 + ItemsCount * sizeof(TReqFunWriteItem));
    ReqParams = PReqFunWriteParams(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
    Answer    = PS7ResHeader23(&PDU.Payload);
    ResParams = PResFunWrite(pbyte(Answer)+ResHeaderSize23);
    P=pbyte(ReqParams)+RPSize;

    // Fill Header
    PDUH_out->P=0x32;                    // Always 0x32
    PDUH_out->PDUType=PduType_request;   // 0x01
    PDUH_out->AB_EX=0x0000;              // Always 0x0000
    PDUH_out->Sequence=GetNextWord();    // AutoInc
    PDUH_out->ParLen=SwapWord(RPSize);   // Request params size

    // Fill Params
    ReqParams->FunWrite=pduFuncWrite;      // 0x05
    ReqParams->ItemsCount=ItemsCount;

    Offset=0;
    Item  = PS7DataItem(Job.pData);
    for (c = 0; c < ItemsCount; c++)
    {
        // Items Params
        ReqParams->Items[c].ItemHead[0]=0x12;
        ReqParams->Items[c].ItemHead[1]=0x0A;
        ReqParams->Items[c].ItemHead[2]=0x10;

        ReqParams->Items[c].TransportSize=Item->WordLen;
        ReqParams->Items[c].Length=SwapWord(Item->Amount);
        ReqParams->Items[c].Area=Item->Area;

        if (Item->Area==S7AreaDB)
            ReqParams->Items[c].DBNumber=SwapWord(Item->DBNumber);
        else
            ReqParams->Items[c].DBNumber=0x0000;

        // Adjusts the offset
        if ((Item->WordLen==S7WLBit) || (Item->WordLen==S7WLCounter) || (Item->WordLen==S7WLTimer))
        	Address=Item->Start;
        else
        	Address=Item->Start*8;
        // Builds the offset
        ReqParams->Items[c].Address[2]=Address & 0x000000FF;
        Address=Address >> 8;
        ReqParams->Items[c].Address[1]=Address & 0x000000FF;
        Address=Address >> 8;
        ReqParams->Items[c].Address[0]=Address & 0x000000FF;

        // Items Data
        ReqData[c]=PReqFunWriteDataItem(pbyte(P)+Offset);
        ReqData[c]->ReturnCode=0x00;

        switch (Item->WordLen)
        {
          case S7WLBit     :
               ReqData[c]->TransportSize=TS_ResBit;
               break;
		  case S7WLInt     :
          case S7WLDInt    :
               ReqData[c]->TransportSize=TS_ResInt;
               break;
          case S7WLReal    :
               ReqData[c]->TransportSize=TS_ResReal;
               break;
          case S7WLChar    :
          case S7WLCounter :
          case S7WLTimer   : ReqData[c]->TransportSize=TS_ResOctet;
			   break;
		  default :
			   ReqData[c]->TransportSize=TS_ResByte; // byte/word/dword etc.
			   break;
        };

        WordSize=DataSizeByte(Item->WordLen);
        Size=Item->Amount * WordSize;

		if ((ReqData[c]->TransportSize!=TS_ResOctet) && (ReqData[c]->TransportSize!=TS_ResReal) && (ReqData[c]->TransportSize!=TS_ResBit))
           ReqData[c]->DataLength=SwapWord(Size*8);
        else
           ReqData[c]->DataLength=SwapWord(Size);

        memcpy(ReqData[c]->Data, Item->pdata, Size);

		if ((Size % 2) != 0 && (ItemsCount - c != 1))
			Size++; // Skip fill byte for Odd frame (except for the last one)

        Offset+=(4+Size); // next item
        Item++;
    };

    PDUH_out->DataLen=SwapWord(word(Offset));

    IsoSize=RPSize+sizeof(TS7ReqHeader)+int(Offset);
	if (IsoSize>PDULength) 
		return errCliSizeOverPDU;
    Result=isoExchangeBuffer(0,IsoSize);

	if (Result!=0)
		return Result;

	// Function level error
	if (Answer->Error!=0)
		return CpuError(SwapWord(Answer->Error));

    if (ResParams->ItemCount!=ItemsCount)
        return errCliInvalidPlcAnswer;

    Item  = PS7DataItem(Job.pData);
    for (c = 0; c < ItemsCount; c++)
    {
        // Item level error
        if (ResParams->Data[c]==0xFF) // <-- 0xFF means Result OK
           Item->Result=0;
        else
           Item->Result=CpuError(ResParams->Data[c]);
        Item++;
    };
    return Result;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::opListBlocks()
{
    PReqFunGetBlockInfo ReqParams;
    PReqDataFunBlocks   ReqData;
    PResFunGetBlockInfo ResParams;
    PDataFunListAll     ResData;
    PS7ResHeader17      Answer;
    PS7BlocksList       List;
    int IsoSize, Result;

    // Setup pointers (note : PDUH_out and PDU.Payload are the same pointer)
    ReqParams=PReqFunGetBlockInfo(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
    ReqData  =PReqDataFunBlocks(pbyte(ReqParams)+sizeof(TReqFunGetBlockInfo));
    Answer   =PS7ResHeader17(&PDU.Payload);
    ResParams=PResFunGetBlockInfo(pbyte(Answer)+ResHeaderSize17);
    ResData  =PDataFunListAll(pbyte(ResParams)+sizeof(TResFunGetBlockInfo));
    List     =PS7BlocksList(Job.pData);
    // Fill Header
    PDUH_out->P=0x32;                    // Always 0x32
    PDUH_out->PDUType=PduType_userdata;  // 0x07
    PDUH_out->AB_EX=0x0000;              // Always 0x0000
    PDUH_out->Sequence=GetNextWord();    // AutoInc
    PDUH_out->ParLen=SwapWord(sizeof(TReqFunGetBlockInfo));   // 8 bytes params
    PDUH_out->DataLen=SwapWord(sizeof(TReqDataFunBlocks));   // 4 bytes data
    // Fill params (mostly constants)
    ReqParams->Head[0]=0x00;
    ReqParams->Head[1]=0x01;
    ReqParams->Head[2]=0x12;
    ReqParams->Plen   =0x04;
    ReqParams->Uk     =0x11;
    ReqParams->Tg     =grBlocksInfo;
    ReqParams->SubFun =SFun_ListAll;
    ReqParams->Seq    =0x00;
    // Fill data
    ReqData[0] =0x0A;
    ReqData[1] =0x00;
    ReqData[2] =0x00;
    ReqData[3] =0x00;

    IsoSize=sizeof(TS7ReqHeader)+sizeof(TReqFunGetBlockInfo)+sizeof(TReqDataFunBlocks);
    Result=isoExchangeBuffer(0,IsoSize);
    // Get Data
    if (Result==0)
    {
        if (ResParams->ErrNo==0)
        {
            if (SwapWord(ResData->Length)!=28)
              return errCliInvalidPlcAnswer;

            for (int c = 0; c < 7; c++)
            {
                switch (ResData->Blocks[c].BType)
                {
                  case Block_OB:
                      List->OBCount=SwapWord(ResData->Blocks[c].BCount);
                      break;
                  case Block_DB:
                      List->DBCount=SwapWord(ResData->Blocks[c].BCount);
                      break;
                  case Block_SDB:
                      List->SDBCount=SwapWord(ResData->Blocks[c].BCount);
                      break;
                  case Block_FC:
                      List->FCCount=SwapWord(ResData->Blocks[c].BCount);
                      break;
                  case Block_SFC:
                      List->SFCCount=SwapWord(ResData->Blocks[c].BCount);
                      break;
                  case Block_FB:
                      List->FBCount=SwapWord(ResData->Blocks[c].BCount);
                      break;
                  case Block_SFB:
                      List->SFBCount=SwapWord(ResData->Blocks[c].BCount);
                      break;
                }
            }
        }
        else
          Result=CpuError(SwapWord(ResParams->ErrNo));
    }
    return Result;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::opListBlocksOfType()
{
    PReqFunGetBlockInfo ReqParams;
    PReqDataBlockOfType ReqData;

    PS7ResHeader17      Answer;
    PResFunGetBlockInfo ResParams;
    PDataFunGetBot      ResData;
    longword            *PadData;
    word                *List;
    bool                First;
	bool                Done = false;
    byte                BlockType, In_Seq;
    int                 Count, Last, IsoSize, Result;
    int                 c, CThis;
    word                DataLength;
	bool                RoomError = false;

    BlockType=Job.Area;
    List=(word*)(&opData);
    // Setup pointers (note : PDUH_out and PDU.Payload are the same pointer)
    ReqParams=PReqFunGetBlockInfo(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
    Answer   =PS7ResHeader17(&PDU.Payload);
    ResParams=PResFunGetBlockInfo(pbyte(Answer)+ResHeaderSize17);
    ResData  =PDataFunGetBot(pbyte(ResParams)+sizeof(TResFunGetBlockInfo));
    // Get Data
    First =true;
    In_Seq=0x00; // first group sequence, next will come from PLC
    Count =0;
    Last  =0;
    do
    {
    //<--------------------------------------------------------- Get next slice
        // Fill Header
        PDUH_out->P=0x32;                    // Always 0x32
        PDUH_out->PDUType=PduType_userdata;  // 0x07
        PDUH_out->AB_EX=0x0000;              // Always 0x0000
        PDUH_out->Sequence=GetNextWord();    // AutoInc
        if (First)
        {
           PDUH_out->ParLen=SwapWord(8);     // 8 bytes params
           PDUH_out->DataLen=SwapWord(6);    // 6 bytes data
           DataLength=14;
        }
        else
        {
           PDUH_out->ParLen=SwapWord(12);    // 12 bytes params
           PDUH_out->DataLen=SwapWord(4);    // 4 bytes data
           DataLength=16;
        }

        // Fill params (mostly constants)
        ReqParams->Head[0]=0x00;
        ReqParams->Head[1]=0x01;
        ReqParams->Head[2]=0x12;

        if (First)
            ReqParams->Plen   =0x04;
		else
            ReqParams->Plen   =0x08;

		if (First)
			ReqParams->Uk = 0x11;
		else
			ReqParams->Uk = 0x12;

        ReqParams->Tg     =grBlocksInfo;
        ReqParams->SubFun =SFun_ListBoT;
        ReqParams->Seq    =In_Seq;

        // Fill data
        if (First)
        {
            // overlap resvd and error to avoid another struct...
            ReqData  =PReqDataBlockOfType(pbyte(ReqParams)+sizeof(TReqFunGetBlockInfo));
            ReqData->RetVal   =0xFF;
            ReqData->TSize    =TS_ResOctet;
            ReqData->Length   =SwapWord(0x0002);
            ReqData->Zero     =0x30; // zero ascii '0'
            ReqData->BlkType  =BlockType;
        }
        else
        {
            PadData  =(longword*)(pbyte(ReqParams)+sizeof(TReqFunGetBlockInfo));
            ReqData  =PReqDataBlockOfType(pbyte(ReqParams)+sizeof(TReqFunGetBlockInfo)+4);
            *PadData          =0x00000000;
            ReqData->RetVal   =0x0A;
            ReqData->TSize    =0x00;
            ReqData->Length   =0x0000;
            ReqData->Zero     =0x00;
            ReqData->BlkType  =0x00;
        };

        IsoSize=sizeof(TS7ReqHeader)+DataLength;
        Result=isoExchangeBuffer(0,IsoSize);

        if (Result==0)
        {
            if (ResParams->ErrNo==0)
            {
                if (ResData->RetVal==0xFF)
                {
                     Done=((ResParams->Rsvd & 0xFF00) == 0); // Low order byte = 0x00 => the sequence is done
                     In_Seq=ResParams->Seq;  // every next telegram must have this number
					 CThis=((SwapWord(ResData->DataLen) - 4 ) / 4) + 1; // Partial counter
                     for (c=0; c < CThis+1; c++)
                     {
                         *List=SwapWord(ResData->Items[c].BlockNum);
                         Last++;
                         List++;
                         if (Last==0x8000)
                         {
                             Done=true;
                             break;
                         };
                     };
                     Count+=CThis; // Total counter
                     List--;
                }
                else
                    Result=errCliItemNotAvailable;
            }
            else
                Result=errCliItemNotAvailable;
        };
        First=false;
    //---------------------------------------------------------> Get next slice
    }
    while ((!Done) && (Result==0));

	*Job.pAmount=0;
	if (Result==0)
	{
		if (Count>Job.Amount)
		{
			Count=Job.Amount;
			RoomError=true;
		}
		memcpy(Job.pData, &opData, Count*2);
		*Job.pAmount=Count;

		if (RoomError) // Result==0 -> override if romerror
			Result=errCliPartialDataRead;
	};

	return Result;
}
//---------------------------------------------------------------------------
void TSnap7MicroClient::FillTime(word SiemensTime, char *PTime)
{
	// SiemensTime -> number of seconds after 1/1/1984
	// This is not S7 date and time but is used only internally for block info
    time_t TheDate = (SiemensTime * 86400)+ DeltaSecs;
    struct tm * timeinfo = localtime (&TheDate);
    if (timeinfo!=NULL) {
        strftime(PTime,11,"%Y/%m/%d",timeinfo);
    }
    else
        *PTime='\0';
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::opAgBlockInfo()
{
    PS7BlockInfo BlockInfo;
    PReqFunGetBlockInfo ReqParams;
    PReqDataBlockInfo ReqData;
    PS7ResHeader17 Answer;
    PResFunGetBlockInfo ResParams;
    PResDataBlockInfo ResData;
    byte BlockType;
    int BlockNum, IsoSize, Result;

    BlockType=Job.Area;
    BlockNum =Job.Number;
    BlockInfo=PS7BlockInfo(Job.pData);
    memset(BlockInfo,0,sizeof(TS7BlockInfo));
    // Setup pointers (note : PDUH_out and PDU.Payload are the same pointer)
    ReqParams=PReqFunGetBlockInfo(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
    ReqData  =PReqDataBlockInfo(pbyte(ReqParams)+sizeof(TReqFunGetBlockInfo));
    Answer   =PS7ResHeader17(&PDU.Payload);
    ResParams=PResFunGetBlockInfo(pbyte(Answer)+ResHeaderSize17);
    ResData  =PResDataBlockInfo(pbyte(ResParams)+sizeof(TResFunGetBlockInfo));
    // Fill Header
    PDUH_out->P=0x32;                    // Always 0x32
    PDUH_out->PDUType=PduType_userdata;  // 0x07
    PDUH_out->AB_EX=0x0000;              // Always 0x0000
	PDUH_out->Sequence=GetNextWord();    // AutoInc
    PDUH_out->ParLen=SwapWord(sizeof(TReqFunGetBlockInfo));   // 8 bytes params
    PDUH_out->DataLen=SwapWord(sizeof(TReqDataBlockInfo));   // 4 bytes data
    // Fill params (mostly constants)
    ReqParams->Head[0]=0x00;
    ReqParams->Head[1]=0x01;
    ReqParams->Head[2]=0x12;
    ReqParams->Plen   =0x04;
    ReqParams->Uk     =0x11;
    ReqParams->Tg     =grBlocksInfo;
    ReqParams->SubFun =SFun_BlkInfo;
    ReqParams->Seq    =0x00;
    // Fill data
    ReqData->RetVal   =0xFF;
    ReqData->TSize    =TS_ResOctet;
    ReqData->DataLen  =SwapWord(0x0008);
    ReqData->BlkPrfx  =0x30;
    ReqData->BlkType  =BlockType;
    ReqData->A        =0x41;

    ReqData->AsciiBlk[0]=(BlockNum / 10000)+0x30;
    BlockNum=BlockNum % 10000;
    ReqData->AsciiBlk[1]=(BlockNum / 1000)+0x30;
    BlockNum=BlockNum % 1000;
    ReqData->AsciiBlk[2]=(BlockNum / 100)+0x30;
    BlockNum=BlockNum % 100;
    ReqData->AsciiBlk[3]=(BlockNum / 10)+0x30;
    BlockNum=BlockNum % 10;
    ReqData->AsciiBlk[4]=(BlockNum / 1)+0x30;

    IsoSize=sizeof(TS7ReqHeader)+sizeof(TReqFunGetBlockInfo)+sizeof(TReqDataBlockInfo);
    Result=isoExchangeBuffer(0,IsoSize);

    // Get Data
    if (Result==0)
    {
        if (ResParams->ErrNo==0)
        {
            if (SwapWord(ResData->Length)<40) // 78
                return errCliInvalidPlcAnswer;
            if (ResData->RetVal==0xFF) // <-- 0xFF means Result OK
            {
               //<----------------------------------------------Fill block info
                 BlockInfo->BlkType=ResData->SubBlkType;
                 BlockInfo->BlkNumber=SwapWord(ResData->BlkNumber);
                 BlockInfo->BlkLang=ResData->BlkLang;
                 BlockInfo->BlkFlags=ResData->BlkFlags;
                 BlockInfo->MC7Size=SwapWord(ResData->MC7Len);
                 BlockInfo->LoadSize=SwapDWord(ResData->LenLoadMem);
                 BlockInfo->LocalData=SwapWord(ResData->LocDataLen);
                 BlockInfo->SBBLength=SwapWord(ResData->SbbLen);
                 BlockInfo->CheckSum=SwapWord(ResData->BlkChksum);
                 BlockInfo->Version=ResData->Version;
                 memcpy(BlockInfo->Author, ResData->Author, 8);
                 memcpy(BlockInfo->Family,ResData->Family,8);
                 memcpy(BlockInfo->Header,ResData->Header,8);
                 FillTime(SwapWord(ResData->CodeTime_dy),BlockInfo->CodeDate);
                 FillTime(SwapWord(ResData->IntfTime_dy),BlockInfo->IntfDate);
               //---------------------------------------------->Fill block info
            }
            else
                Result=CpuError(ResData->RetVal);
        }
        else
            Result=CpuError(SwapWord(ResParams->ErrNo));
    };
    return Result;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::opDBGet()
{
    TS7BlockInfo BI;
    void * usrPData;
    int * usrPSize;
    int Result, Room;
    bool RoomError = false;

    // Stores user pointer
    usrPData=Job.pData;
    usrPSize=Job.pAmount;
    Room    =Job.Amount;

    // 1 Pass : Get block info
    Job.Area=Block_DB;
    Job.pData=&BI;
    Result=opAgBlockInfo();

    // 2 Pass : Read the whole (MC7Size bytes) DB.
    if (Result==0)
    {
        // Check user space
        if (BI.MC7Size>Room)
        {
           Job.Amount=Room;
           RoomError=true;
        }
        else
            Job.Amount =BI.MC7Size;
        // The data is read even if the buffer is small (the error is reported).
        // Imagine that we want to read only a small amount of data at the
        // beginning of a DB regardless it's size....
        Job.Area   =S7AreaDB;
        Job.WordLen=S7WLByte;
        Job.Start  =0;
        Job.pData  =usrPData;
        Result     =opReadArea();
        if (Result==0)
           *usrPSize=Job.Amount;
    }

    if ((Result==0) && RoomError)
        return errCliBufferTooSmall;
    else
        return Result;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::opDBFill()
{
    TS7BlockInfo BI;
    int Result;
    // new op : get block info
    Job.Op   =s7opAgBlockInfo;
    Job.Area =Block_DB;
    Job.pData=&BI;
    Result   =opAgBlockInfo();
    // Restore original op
    Job.Op   =s7opDBFill;
    // Fill internal buffer then write it
    if (Result==0)
    {
        Job.Amount =BI.MC7Size;
        Job.Area   =S7AreaDB;
        Job.WordLen=S7WLByte;
        Job.Start  =0;
        memset(&opData, byte(Job.IParam), Job.Amount);
        Job.pData  =&opData;
        Result     =opWriteArea();
    }
    return Result;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::opUpload()
{
    PS7ResHeader23 Answer;
    int  IsoSize;
    byte Upload_ID = 0; // not strictly needed, only to avoid warning
	byte BlockType;
    int  BlockNum, BlockLength, Result;
    bool Done, Full; // if full==true, the data will be compatible to full download function
    uintptr_t Offset;
    bool RoomError = false;

    BlockType=Job.Area;
    BlockNum =Job.Number;
    Full     =Job.IParam==1;
    // Setup Answer (is the same for all Upload pdus)
    Answer=  PS7ResHeader23(&PDU.Payload);
    // Init sequence
    Done  =false;
    Offset=0;
    //<-------------------------------------------------------------StartUpload
    PReqFunStartUploadParams ReqParams;
    PResFunStartUploadParams ResParams;
    // Setup pointers (note : PDUH_out and PDU.Payload are the same pointer)
    ReqParams=PReqFunStartUploadParams(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
    ResParams=PResFunStartUploadParams(pbyte(Answer)+ResHeaderSize23);
    // Init Header
    PDUH_out->P=0x32;                   // Always 0x32
    PDUH_out->PDUType=PduType_request;  // 0x01
    PDUH_out->AB_EX=0x0000;             // Always 0x0000
    PDUH_out->Sequence=GetNextWord();   // AutoInc
    PDUH_out->ParLen=SwapWord(sizeof(TReqFunStartUploadParams));// params size
    PDUH_out->DataLen=0x0000;           // No data
    // Init Params
    ReqParams->FunSUpld=pduStartUpload;
    ReqParams->Uk6[0]=0x00;
    ReqParams->Uk6[1]=0x00;
    ReqParams->Uk6[2]=0x00;
    ReqParams->Uk6[3]=0x00;
    ReqParams->Uk6[4]=0x00;
    ReqParams->Uk6[5]=0x00;
    ReqParams->Upload_ID=Upload_ID;  // At begining is 0) we will put upload id incoming from plc
    ReqParams->Len_1 =0x09; // 9 bytes from here
    ReqParams->Prefix=0x5F;
    ReqParams->BlkPrfx=0x30; // '0'
    ReqParams->BlkType=BlockType;
    // Block number
    ReqParams->AsciiBlk[0]=(BlockNum / 10000)+0x30;
    BlockNum=BlockNum % 10000;
    ReqParams->AsciiBlk[1]=(BlockNum / 1000)+0x30;
    BlockNum=BlockNum % 1000;
    ReqParams->AsciiBlk[2]=(BlockNum / 100)+0x30;
    BlockNum=BlockNum % 100;
    ReqParams->AsciiBlk[3]=(BlockNum / 10)+0x30;
    BlockNum=BlockNum % 10;
    ReqParams->AsciiBlk[4]=(BlockNum / 1)+0x30;
    ReqParams->A=0x41; // 'A'

    IsoSize=sizeof(TS7ReqHeader)+sizeof(TReqFunStartUploadParams);
    Result=isoExchangeBuffer(0,IsoSize);
    // Get Upload Infos (only ID now)
    if (Result==0)
    {
      if (Answer->Error==0)
        Upload_ID=ResParams->Upload_ID;
      else
        Result=CpuError(SwapWord(Answer->Error));
    };
    //------------------------------------------------------------->StartUpload
    if (Result==0)
    {
         //<--------------------------------------------------------FirstUpload
        PReqFunUploadParams ReqParams;
        PResFunUploadParams ResParams;
        PResFunUploadDataHeaderFirst ResDataHeader;
        pbyte Source;
        pbyte Target;
        int Size;

        // Setup pointers (note : PDUH_out and PDU.Payload are the same pointer)
        ReqParams=PReqFunUploadParams(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
        // First upload pdu consists of params, block info header, data.
        ResParams=PResFunUploadParams(pbyte(Answer)+ResHeaderSize23);
        ResDataHeader=PResFunUploadDataHeaderFirst(pbyte(ResParams)+sizeof(TResFunUploadParams));
        if (Full)
          Source=pbyte(ResDataHeader)+4; // skip only the mini header
        else
          Source=pbyte(ResDataHeader)+sizeof(TResFunUploadDataHeaderFirst);  // not full : skip the data header
        // Init Header
        PDUH_out->P=0x32;                   // Always 0x32
        PDUH_out->PDUType=PduType_request;  // 0x01
        PDUH_out->AB_EX=0x0000;             // Always 0x0000
        PDUH_out->Sequence=GetNextWord();   // AutoInc
        PDUH_out->ParLen=SwapWord(sizeof(TReqFunUploadParams));// params size
        PDUH_out->DataLen=0x0000;           // No data
        // Init Params
        ReqParams->FunUpld=pduUpload;
        ReqParams->Uk6[0]=0x00;
        ReqParams->Uk6[1]=0x00;
        ReqParams->Uk6[2]=0x00;
        ReqParams->Uk6[3]=0x00;
        ReqParams->Uk6[4]=0x00;
        ReqParams->Uk6[5]=0x00;
        ReqParams->Upload_ID=Upload_ID;  // At begining is 0) we will put upload id incoming from plc

        IsoSize=sizeof(TS7ReqHeader)+sizeof(TReqFunUploadParams);
        Result=isoExchangeBuffer(0,IsoSize);
        // Get Upload Infos (only ID now)
        if (Result==0)
        {
          if (Answer->Error==0)
          {
            Done=ResParams->EoU==0;
            if (Full)
                Size=SwapWord(Answer->DataLen)-4; // Full data Size
            else
                Size=SwapWord(Answer->DataLen)-sizeof(TResFunUploadDataHeaderFirst); // Size of this data slice

            BlockLength=SwapWord(ResDataHeader->MC7Len); // Full block size in byte
            Target=pbyte(&opData)+Offset;
            memcpy(Target, Source, Size);
            Offset+=Size;
          }
          else
              Result=errCliUploadSequenceFailed;
        };
         //-------------------------------------------------------->FirstUpload
         while (!Done && (Result==0))
         {
             //<----------------------------------------------------NextUpload
            PReqFunUploadParams ReqParams;
            PResFunUploadParams ResParams;
            PResFunUploadDataHeaderNext ResDataHeader;
            pbyte Source;
            pbyte Target;
            int Size;

            // Setup pointers (note : PDUH_out and PDU.Payload are the same pointer)
            ReqParams=PReqFunUploadParams(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
            // Next upload pdu consists of params, small info header, data.
            ResParams=PResFunUploadParams(pbyte(Answer)+ResHeaderSize23);
            ResDataHeader=PResFunUploadDataHeaderNext(pbyte(ResParams)+sizeof(TResFunUploadParams));
            Source=pbyte(ResDataHeader)+sizeof(TResFunUploadDataHeaderNext);
            // Init Header
            PDUH_out->P=0x32;                   // Always 0x32
            PDUH_out->PDUType=PduType_request;  // 0x01
            PDUH_out->AB_EX=0x0000;             // Always 0x0000
            PDUH_out->Sequence=GetNextWord();   // AutoInc
            PDUH_out->ParLen=SwapWord(sizeof(TReqFunUploadParams));// params size
            PDUH_out->DataLen=0x0000;           // No data
            // Init Params
            ReqParams->FunUpld=pduUpload;
            ReqParams->Uk6[0]=0x00;
            ReqParams->Uk6[1]=0x00;
            ReqParams->Uk6[2]=0x00;
            ReqParams->Uk6[3]=0x00;
            ReqParams->Uk6[4]=0x00;
            ReqParams->Uk6[5]=0x00;
            ReqParams->Upload_ID=Upload_ID;  // At begining is 0) we will put upload id incoming from plc

            IsoSize=sizeof(TS7ReqHeader)+sizeof(TReqFunUploadParams);
            Result=isoExchangeBuffer(0,IsoSize);
            // Get Upload Infos (only ID now)
            if (Result==0)
            {
                if (Answer->Error==0)
                {
                    Done=ResParams->EoU==0;
                    Size=SwapWord(Answer->DataLen)-sizeof(TResFunUploadDataHeaderNext); // Size of this data slice
                    Target=pbyte(&opData)+Offset;
                    memcpy(Target, Source, Size);
                    Offset+=Size;
                }
                else
                    Result=errCliUploadSequenceFailed;
            };
             //---------------------------------------------------->NextUpload
         }
         if (Result==0)
         {
            //<----------------------------------------------------EndUpload;
            PReqFunEndUploadParams ReqParams;
            PResFunEndUploadParams ResParams;

            // Setup pointers (note : PDUH_out and PDU.Payload are the same pointer)
            ReqParams=PReqFunEndUploadParams(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
            ResParams=PResFunEndUploadParams(pbyte(Answer)+ResHeaderSize23);
            // Init Header
            PDUH_out->P=0x32;                   // Always 0x32
            PDUH_out->PDUType=PduType_request;  // 0x01
            PDUH_out->AB_EX=0x0000;             // Always 0x0000
            PDUH_out->Sequence=GetNextWord();   // AutoInc
            PDUH_out->ParLen=SwapWord(sizeof(TReqFunEndUploadParams));// params size
            PDUH_out->DataLen=0x0000;           // No data
            // Init Params
            ReqParams->FunEUpld=pduEndUpload;
            ReqParams->Uk6[0]=0x00;
            ReqParams->Uk6[1]=0x00;
            ReqParams->Uk6[2]=0x00;
            ReqParams->Uk6[3]=0x00;
            ReqParams->Uk6[4]=0x00;
            ReqParams->Uk6[5]=0x00;
            ReqParams->Upload_ID=Upload_ID;  // At begining is 0) we will put upload id incoming from plc

            IsoSize=sizeof(TS7ReqHeader)+sizeof(TReqFunEndUploadParams);
            Result=isoExchangeBuffer(0,IsoSize);

            // Get EndUpload Result
            if (Result==0)
            {
               if ((Answer->Error!=0) || (ResParams->FunEUpld!=pduEndUpload))
                   Result=errCliUploadSequenceFailed;
            };
            //---------------------------------------------------->EndUpload;
         }
    };

    *Job.pAmount=0;
    if (Result==0)
    {
        if (Full)
        {
            opSize=int(Offset);
            if (opSize<78)
                Result=errCliInvalidDataSizeRecvd;
        }
        else
        {
            opSize=BlockLength;
            if (opSize<1)
                Result=errCliInvalidDataSizeRecvd;
        };
        if (Result==0)
        {
			 // Checks user space
             if (Job.Amount<opSize) {
                 opSize=Job.Amount;
				 RoomError = true;
			 };
			 memcpy(Job.pData, &opData, opSize);
			 *Job.pAmount=opSize;
			 if (RoomError) // Result==0 -> override if romerror
				Result=errCliPartialDataRead;
		};
    };
    return Result;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::opDownload()
{
    PS7CompactBlockInfo Info;
    PS7BlockFooter Footer;
    int BlockNum, StoreBlockNum, BlockAmount;
    int BlockSize, BlockSizeLd;
    int BlockType, Remainder;
    int Result, IsoSize;
    bool Done  = false;
    uintptr_t Offset;

    BlockAmount=Job.Amount;
    BlockNum   =Job.Number;
    Result=CheckBlock(-1,-1,&opData,BlockAmount);
    if (Result==0)
    {
        Info=PS7CompactBlockInfo(&opData);
        // Gets blocktype
        BlockType=SubBlockToBlock(Info->SubBlkType);

        if (BlockNum>=0)
            Info->BlkNum=SwapWord(BlockNum); // change the number
        else
            BlockNum=SwapWord(Info->BlkNum); // use the header's number

        BlockSizeLd=BlockAmount; // load mem needed for this block
        BlockSize  =SwapWord(Info->MC7Len); // net size
        Footer=PS7BlockFooter(pbyte(&opData)+BlockSizeLd-sizeof(TS7BlockFooter));
        Footer->Chksum=0x0000;

        Offset=0;
        Remainder=BlockAmount;
        //<---------------------------------------------- Start Download request
            PReqStartDownloadParams ReqParams;
            PResStartDownloadParams ResParams;
            PS7ResHeader23          Answer;

            // Setup pointers (note : PDUH_out and PDU.Payload are the same pointer)
            ReqParams=PReqStartDownloadParams(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
            Answer   =PS7ResHeader23(&PDU.Payload);
            ResParams=PResStartDownloadParams(pbyte(Answer)+ResHeaderSize23);
            // Init Header
            PDUH_out->P=0x32;                     // Always 0x32
            PDUH_out->PDUType=PduType_request;    // 0x01
            PDUH_out->AB_EX=0x0000;               // Always 0x0000
            PDUH_out->Sequence=GetNextWord();     // AutoInc
            PDUH_out->ParLen=SwapWord(sizeof(TReqStartDownloadParams));
            PDUH_out->DataLen=0x0000;             // No data
            // Init Params
            ReqParams->FunSDwnld = pduReqDownload;
            ReqParams->Uk6[0]=0x00;
            ReqParams->Uk6[1]=0x01;
            ReqParams->Uk6[2]=0x00;
            ReqParams->Uk6[3]=0x00;
            ReqParams->Uk6[4]=0x00;
            ReqParams->Uk6[5]=0x00;
            ReqParams->Dwnld_ID=0x00;
            ReqParams->Len_1 =0x09;
            ReqParams->Prefix=0x5F;
            ReqParams->BlkPrfx=0x30;
            ReqParams->BlkType=BlockType;
			StoreBlockNum=BlockNum;
            ReqParams->AsciiBlk[0]=(BlockNum / 10000)+0x30;
            BlockNum=BlockNum % 10000;
            ReqParams->AsciiBlk[1]=(BlockNum / 1000)+0x30;
            BlockNum=BlockNum % 1000;
            ReqParams->AsciiBlk[2]=(BlockNum / 100)+0x30;
            BlockNum=BlockNum % 100;
            ReqParams->AsciiBlk[3]=(BlockNum / 10)+0x30;
            BlockNum=BlockNum % 10;
            ReqParams->AsciiBlk[4]=(BlockNum / 1)+0x30;
            ReqParams->P    =0x50;
            ReqParams->Len_2=0x0D;
            ReqParams->Uk1  =0x31; // '1'
			BlockNum=StoreBlockNum;
            // Load memory
            ReqParams->AsciiLoad[0]=(BlockSizeLd / 100000)+0x30;
            BlockSizeLd=BlockSizeLd % 100000;
            ReqParams->AsciiLoad[1]=(BlockSizeLd / 10000)+0x30;
            BlockSizeLd=BlockSizeLd % 10000;
            ReqParams->AsciiLoad[2]=(BlockSizeLd / 1000)+0x30;
            BlockSizeLd=BlockSizeLd % 1000;
            ReqParams->AsciiLoad[3]=(BlockSizeLd / 100)+0x30;
            BlockSizeLd=BlockSizeLd % 100;
            ReqParams->AsciiLoad[4]=(BlockSizeLd / 10)+0x30;
            BlockSizeLd=BlockSizeLd % 10;
            ReqParams->AsciiLoad[5]=(BlockSizeLd / 1)+0x30;
            // MC7 memory
            ReqParams->AsciiMC7[0]=(BlockSize / 100000)+0x30;
            BlockSize=BlockSize % 100000;
            ReqParams->AsciiMC7[1]=(BlockSize / 10000)+0x30;
            BlockSize=BlockSize % 10000;
            ReqParams->AsciiMC7[2]=(BlockSize / 1000)+0x30;
            BlockSize=BlockSize % 1000;
            ReqParams->AsciiMC7[3]=(BlockSize / 100)+0x30;
            BlockSize=BlockSize % 100;
            ReqParams->AsciiMC7[4]=(BlockSize / 10)+0x30;
            BlockSize=BlockSize % 10;
            ReqParams->AsciiMC7[5]=(BlockSize / 1)+0x30;

            IsoSize=sizeof(TS7ReqHeader)+sizeof(TReqStartDownloadParams);
            Result=isoExchangeBuffer(0,IsoSize);

            // Get Result
            if (Result==0)
            {
                 if (SwapWord(Answer->Error)!=Code7NeedPassword)
                 {
                     if ((Answer->Error!=0) || (*ResParams!=pduReqDownload))
                          Result=errCliDownloadSequenceFailed;
                 }
                 else
                     Result=errCliNeedPassword;
            }
        //----------------------------------------------> Start Download request
        if (Result==0)
        {
          do
          {
            //<-------------------------------- Download sequence (PLC requests)
                PReqDownloadParams ReqParams;
                PS7ResHeader23 Answer;
                PResDownloadParams ResParams;
                PResDownloadDataHeader ResData;
                int Slice, Size, MaxSlice;
                word Sequence;
                pbyte Source;
                pbyte Target;

                ReqParams=PReqDownloadParams(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
                Answer   =PS7ResHeader23(&PDU.Payload);
                ResParams=PResDownloadParams(pbyte(Answer)+ResHeaderSize23);
                ResData  =PResDownloadDataHeader(pbyte(ResParams)+sizeof(TResDownloadParams));
                Target   =pbyte(ResData)+sizeof(TResDownloadDataHeader);
                Source   =pbyte(&opData)+Offset;

                Result=isoRecvBuffer(0,Size);
                if (Result==0)
                {
                      if ((u_int(Size)>sizeof(TS7ReqHeader)) && (ReqParams->Fun==pduDownload))
                      {
                            Sequence=PDUH_out->Sequence;
                            // Max data slice that we can fit in this pdu
                            MaxSlice=PDULength-ResHeaderSize23-sizeof(TResDownloadParams)-sizeof(TResDownloadDataHeader);
                            Slice=Remainder;
                            if (Slice>MaxSlice)
                                Slice=MaxSlice;
                            Remainder-=Slice;
                            Offset+=Slice;
                            Done=Remainder<=0;
                            // Init Answer
                            Answer->P=0x32;
                            Answer->PDUType=PduType_response;
                            Answer->AB_EX=0x0000;
                            Answer->Sequence=Sequence;
                            Answer->ParLen =SwapWord(sizeof(TResDownloadParams));
                            Answer->DataLen=SwapWord(word(sizeof(TResDownloadDataHeader))+Slice);
                            Answer->Error  =0x0000;

                            // Init Params
                            ResParams->FunDwnld=pduDownload;
                            if (Remainder>0)
                                ResParams->EoS=0x01;
                            else
                                ResParams->EoS=0x00;

                            // Init Data
                            ResData->DataLen=SwapWord(Slice);
                            ResData->FB_00=0xFB00;
                            memcpy(Target, Source, Slice);

                            // Send the slice
                            IsoSize=ResHeaderSize23+sizeof(TResDownloadParams)+sizeof(TResDownloadDataHeader)+Slice;
                            Result=isoSendBuffer(0,IsoSize);
                      }
                      else
                          Result=errCliDownloadSequenceFailed;
                };
            //--------------------------------> Download sequence (PLC requests)
          }
          while (!Done && (Result==0));

          if (Result==0)
          {
            //<-------------------------------------------Perform Download Ended
                PReqDownloadParams ReqParams;
                PS7ResHeader23 Answer;
                PResEndDownloadParams ResParams;
                int Size;
                word Sequence;

                ReqParams=PReqDownloadParams(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
                Answer   =PS7ResHeader23(&PDU.Payload);
                ResParams=PResEndDownloadParams(pbyte(Answer)+ResHeaderSize23);

                Result=isoRecvBuffer(0,Size);
                if (Result==0)
                {
                  if ((u_int(Size)>sizeof(TS7ReqHeader)) && (ReqParams->Fun==pduDownloadEnded))
                  {
                    Sequence=PDUH_out->Sequence;
                    // Init Answer
                    Answer->P=0x32;
                    Answer->PDUType=PduType_response;
                    Answer->AB_EX=0x0000;
                    Answer->Sequence=Sequence;
                    Answer->ParLen =SwapWord(sizeof(TResEndDownloadParams));
                    Answer->DataLen=0x0000;
                    Answer->Error  =0x0000;

                    // Init Params
                    *ResParams=pduDownloadEnded;
                    IsoSize=ResHeaderSize23+sizeof(TResEndDownloadParams);
                    Result=isoSendBuffer(0,IsoSize);
                  }
                  else
                      Result=errCliDownloadSequenceFailed;
                };
            //------------------------------------------->Perform Download Ended
            if (Result==0)
            {
               //<----------------------------------- Insert block into the unit
                PReqControlBlockParams ReqParams;
                PS7ResHeader23 Answer;
                pbyte ResParams;

                // Setup pointers (note : PDUH_out and PDU.Payload are the same pointer)
                ReqParams=PReqControlBlockParams(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
                Answer=PS7ResHeader23(&PDU.Payload);
                ResParams=pbyte(Answer)+ResHeaderSize23;
                // Init Header
                PDUH_out->P=0x32;                     // Always 0x32
                PDUH_out->PDUType=PduType_request;    // 0x01
                PDUH_out->AB_EX=0x0000;               // Always 0x0000
                PDUH_out->Sequence=GetNextWord();     // AutoInc
                PDUH_out->ParLen=SwapWord(sizeof(TReqControlBlockParams));
                PDUH_out->DataLen=0x0000;             // No data
                // Init Params
                ReqParams->Fun = pduControl;
                ReqParams->Uk7[0]=0x00;
                ReqParams->Uk7[1]=0x00;
                ReqParams->Uk7[2]=0x00;
                ReqParams->Uk7[3]=0x00;
                ReqParams->Uk7[4]=0x00;
                ReqParams->Uk7[5]=0x00;
                ReqParams->Uk7[6]=0xFD;
                ReqParams->Len_1 =SwapWord(0x0A);
                ReqParams->NumOfBlocks=0x01;
                ReqParams->ByteZero   =0x00;
                ReqParams->AsciiZero  =0x30;
                ReqParams->BlkType=BlockType;
                ReqParams->AsciiBlk[0]=(BlockNum / 10000)+0x30;
                BlockNum=BlockNum % 10000;
                ReqParams->AsciiBlk[1]=(BlockNum / 1000)+0x30;
                BlockNum=BlockNum % 1000;
                ReqParams->AsciiBlk[2]=(BlockNum / 100)+0x30;
                BlockNum=BlockNum % 100;
                ReqParams->AsciiBlk[3]=(BlockNum / 10)+0x30;
                BlockNum=BlockNum % 10;
                ReqParams->AsciiBlk[4]=(BlockNum / 1)+0x30;
                ReqParams->SFun =SFun_Insert;
                ReqParams->Len_2=0x05;
                ReqParams->Cmd[0]='_';
                ReqParams->Cmd[1]='I';
                ReqParams->Cmd[2]='N';
                ReqParams->Cmd[3]='S';
                ReqParams->Cmd[4]='E';

                IsoSize=sizeof(TS7ReqHeader)+sizeof(TReqControlBlockParams);
                Result=isoExchangeBuffer(0,IsoSize);

                if (Result==0)
                {
                  if ((Answer->Error!=0) || (*ResParams!=pduControl))
                     Result=errCliInsertRefused;
                };
               //-----------------------------------> Insert block into the unit
            }
          };
        };
    };
    return Result;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::opDelete()
{
    PReqControlBlockParams ReqParams;
    PS7ResHeader23 Answer;
    pbyte ResParams;
    int IsoSize, BlockType, BlockNum, Result;

    BlockType=Job.Area;
    BlockNum =Job.Number;
    // Setup pointers (note : PDUH_out and PDU.Payload are the same pointer)
    ReqParams=PReqControlBlockParams(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
    Answer   =PS7ResHeader23(&PDU.Payload);
    ResParams=pbyte(Answer)+ResHeaderSize23;
    // Init Header
    PDUH_out->P=0x32;                     // Always 0x32
    PDUH_out->PDUType=PduType_request;    // 0x01
    PDUH_out->AB_EX=0x0000;               // Always 0x0000
    PDUH_out->Sequence=GetNextWord();     // AutoInc
    PDUH_out->ParLen=SwapWord(sizeof(TReqControlBlockParams));
    PDUH_out->DataLen=0x0000;             // No data
    // Init Params
    ReqParams->Fun = pduControl;
    ReqParams->Uk7[0]=0x00;
    ReqParams->Uk7[1]=0x00;
    ReqParams->Uk7[2]=0x00;
    ReqParams->Uk7[3]=0x00;
    ReqParams->Uk7[4]=0x00;
    ReqParams->Uk7[5]=0x00;
    ReqParams->Uk7[6]=0xFD;
    ReqParams->Len_1 =SwapWord(0x0A);
    ReqParams->NumOfBlocks=0x01;
    ReqParams->ByteZero   =0x00;
    ReqParams->AsciiZero  =0x30;
    ReqParams->BlkType=BlockType;
    ReqParams->AsciiBlk[0]=(BlockNum / 10000)+0x30;
    BlockNum=BlockNum % 10000;
    ReqParams->AsciiBlk[1]=(BlockNum / 1000)+0x30;
    BlockNum=BlockNum % 1000;
    ReqParams->AsciiBlk[2]=(BlockNum / 100)+0x30;
    BlockNum=BlockNum % 100;
    ReqParams->AsciiBlk[3]=(BlockNum / 10)+0x30;
    BlockNum=BlockNum % 10;
    ReqParams->AsciiBlk[4]=(BlockNum / 1)+0x30;
    ReqParams->SFun =SFun_Delete;
    ReqParams->Len_2=0x05;
    ReqParams->Cmd[0]='_';
    ReqParams->Cmd[1]='D';
    ReqParams->Cmd[2]='E';
    ReqParams->Cmd[3]='L';
    ReqParams->Cmd[4]='E';

    IsoSize=sizeof(TS7ReqHeader)+sizeof(TReqControlBlockParams);
    Result=isoExchangeBuffer(0,IsoSize);

    if (Result==0)
    {
        if (SwapWord(Answer->Error)!=Code7NeedPassword)
        {
            if ((Answer->Error!=0) || (*ResParams!=pduControl))
              Result=errCliDeleteRefused;
        }
        else
            Result=errCliNeedPassword;
    }
    return Result;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::opReadSZL()
{
    PS7Answer17         Answer;
    PReqFunReadSZLFirst ReqParamsFirst;
    PReqFunReadSZLNext  ReqParamsNext;
    PS7ReqSZLData       ReqDataFirst;
    PS7ReqSZLData       ReqDataNext;
    PS7ResParams7       ResParams;
    PS7ResSZLDataFirst  ResDataFirst;
    PS7ResSZLDataNext   ResDataNext;
    PSZL_HEADER         Header;
    PS7SZLList          Target;
    pbyte               PDataFirst;
    pbyte               PDataNext;
    word                ID, Index;
    int                 IsoSize, DataSize, DataSZL, Result;
    bool                First, Done;
    bool                NoRoom = false;
    uintptr_t           Offset =0;
    byte                Seq_in =0x00;

    ID=Job.ID;
    Index=Job.Index;
    opSize=0;
    // Setup pointers (note : PDUH_out and PDU.Payload are the same pointer)
    ReqParamsFirst=PReqFunReadSZLFirst(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
    ReqParamsNext =PReqFunReadSZLNext(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
    ReqDataFirst  =PS7ReqSZLData(pbyte(ReqParamsFirst)+sizeof(TReqFunReadSZLFirst));
    ReqDataNext   =PS7ReqSZLData(pbyte(ReqParamsNext)+sizeof(TReqFunReadSZLNext));

    Answer        =PS7Answer17(&PDU.Payload);
    ResParams     =PS7ResParams7(pbyte(Answer)+ResHeaderSize17);
    ResDataFirst  =PS7ResSZLDataFirst(pbyte(ResParams)+sizeof(TS7Params7));
    ResDataNext   =PS7ResSZLDataNext(pbyte(ResParams)+sizeof(TS7Params7));
    PDataFirst    =pbyte(ResDataFirst)+8; // skip header
    PDataNext     =pbyte(ResDataNext)+4;  // skip header
    Header        =PSZL_HEADER(&opData);
    First=true;
    Done =false;
    do
    {
    //<------------------------------------------------------- read slices
        if (First)
        {
         //<-------------------------------------------------- prepare first
            DataSize=sizeof(TS7ReqSZLData);
            // Fill Header
            PDUH_out->P=0x32;                    // Always 0x32
            PDUH_out->PDUType=PduType_userdata;  // 0x07
            PDUH_out->AB_EX=0x0000;              // Always 0x0000
            PDUH_out->Sequence=GetNextWord();    // AutoInc
            PDUH_out->ParLen=SwapWord(sizeof(TReqFunReadSZLFirst)); // 8 bytes params
            PDUH_out->DataLen=SwapWord(DataSize); // 8/4 bytes data
            // Fill Params
            ReqParamsFirst->Head[0]=0x00;
            ReqParamsFirst->Head[1]=0x01;
            ReqParamsFirst->Head[2]=0x12;
            ReqParamsFirst->Plen   =0x04;
            ReqParamsFirst->Uk     =0x11;
            ReqParamsFirst->Tg     =grSZL;
            ReqParamsFirst->SubFun =SFun_ReadSZL; //0x03
            ReqParamsFirst->Seq    =Seq_in;
            // Fill Data
            ReqDataFirst->Ret      =0xFF;
            ReqDataFirst->TS       =TS_ResOctet;
            ReqDataFirst->DLen     =SwapWord(0x0004);
            ReqDataFirst->ID       =SwapWord(ID);
            ReqDataFirst->Index    =SwapWord(Index);
            IsoSize=sizeof(TS7ReqHeader)+sizeof(TReqFunReadSZLFirst)+DataSize;
         //--------------------------------------------------> prepare first
        }
        else
        {
         //<-------------------------------------------------- prepare next
            DataSize=sizeof(TS7ReqSZLData)-4;
            // Fill Header
            PDUH_out->P=0x32;                    // Always 0x32
            PDUH_out->PDUType=PduType_userdata;  // 0x07
            PDUH_out->AB_EX=0x0000;              // Always 0x0000
            PDUH_out->Sequence=GetNextWord();    // AutoInc
            PDUH_out->ParLen=SwapWord(sizeof(TReqFunReadSZLNext)); // 8 bytes params
            PDUH_out->DataLen=SwapWord(DataSize);// 8/4 bytes data
            // Fill Params
            ReqParamsNext->Head[0]=0x00;
            ReqParamsNext->Head[1]=0x01;
            ReqParamsNext->Head[2]=0x12;
            ReqParamsNext->Plen   =0x08;
            ReqParamsNext->Uk     =0x12;
            ReqParamsNext->Tg     =grSZL;
            ReqParamsNext->SubFun =SFun_ReadSZL;
            ReqParamsNext->Seq    =Seq_in;
            ReqParamsNext->Rsvd   =0x0000;
            ReqParamsNext->ErrNo  =0x0000;
            // Fill Data
            ReqDataNext->Ret      =0x0A;
            ReqDataNext->TS       =0x00;
            ReqDataNext->DLen     =0x0000;
            ReqDataNext->ID       =0x0000;
            ReqDataNext->Index    =0x0000;
            IsoSize=sizeof(TS7ReqHeader)+sizeof(TReqFunReadSZLNext)+DataSize;
         //--------------------------------------------------> prepare next
        }
        Result=isoExchangeBuffer(0,IsoSize);
        // Get Data
        if (Result==0)
        {
            if (First)
            {
            //<------------------------------------------ get data first
                if (ResParams->Err==0)
                {
                    if (ResDataFirst->Ret==0xFF)  // <-- 0xFF means Result OK
                    {
                        // Gets Amount of this slice
                        DataSZL=SwapWord(ResDataFirst->DLen)-4;// Skips extra params (ID, Index ...)
                        // Gets end of Sequence Flag
                        Done=(ResParams->resvd & 0xFF00) == 0; // Low order byte = 0x00 => the sequence is done
                        // Gets Unit's function sequence
                        Seq_in=ResParams->Seq;
                        Target=PS7SZLList(pbyte(&opData)+Offset);
                        memcpy(Target, PDataFirst, DataSZL);
                        Offset+=DataSZL;
                    }
                    else
                        Result=CpuError(ResDataFirst->Ret);
                }
                else
                    Result=CpuError(ResDataFirst->Ret);
            //------------------------------------------> get data first
            }
            else
            {
            //<------------------------------------------ get data next
                if (ResParams->Err==0)
                {
                    if (ResDataNext->Ret==0xFF)  // <-- 0xFF means Result OK
                    {
                        // Gets Amount of this slice
                        DataSZL=SwapWord(ResDataNext->DLen);
                        // Gets end  of Sequence Flag
                        Done=(ResParams->resvd & 0xFF00) == 0; // Low order byte = 0x00 => the sequence is done
                        // Gets Unit's function sequence
                        Seq_in=ResParams->Seq;
                        Target=PS7SZLList(pbyte(&opData)+Offset);
                        memcpy(Target, PDataNext, DataSZL);
                        Offset+=DataSZL;
                    }
                    else
                        Result=CpuError(ResDataNext->Ret);
                }
                else
                    Result=CpuError(ResDataNext->Ret);
            //------------------------------------------> get data next
            }
            First=false;
        }
    //-------------------------------------------------------> read slices
    }
    while ((!Done) && (Result==0));

    // Check errors and adjust header
    if (Result==0)
    {
        // Adjust big endian header
        Header->LENTHDR=SwapWord(Header->LENTHDR);
        Header->N_DR   =SwapWord(Header->N_DR);
        opSize=int(Offset);

        if (Job.IParam==1)  // if 1 data has to be copied into user buffer
        {
              // Check buffer size
              if (opSize>Job.Amount)
              {
                 opSize=Job.Amount;
                 NoRoom=true;
              }
              memcpy(Job.pData, &opData, opSize);
              *Job.pAmount=opSize;
        };
    };
    if ((Result==0)&& NoRoom)
        Result=errCliBufferTooSmall;

    return Result;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::opReadSZLList()
{
    PS7SZLList usrSZLList, opDataList;
    int ItemsCount, ItemsCount_in, c, Result;
    bool NoRoom = false;

    Job.ID       =0x0000;
    Job.Index    =0x0000;
    Job.IParam   =0;
    ItemsCount_in=Job.Amount;     // stores the room
    Job.Amount   =sizeof(opData); // read into the internal buffer

    Result =opReadSZL();
    if (Result==0)
    {
        opDataList=PS7SZLList(&opData); // Source
        usrSZLList=PS7SZLList(Job.pData);  // Target

        ItemsCount=(opSize-sizeof(SZL_HEADER)) / 2;
        // Check input size
        if (ItemsCount>ItemsCount_in)
        {
            ItemsCount=ItemsCount_in; // Trim itemscount
            NoRoom=true;
        }
        for (c = 0; c < ItemsCount; c++)
          usrSZLList->List[c]=SwapWord(opDataList->List[c]);
        *Job.pAmount=ItemsCount;
    }
    else
        *Job.pAmount=0;

    if ((Result==0) && NoRoom)
        Result=errCliBufferTooSmall;

    return Result;
}
//---------------------------------------------------------------------------
byte TSnap7MicroClient::BCDtoByte(byte B)
{
    return ((B >> 4) * 10) + (B & 0x0F);
}
//---------------------------------------------------------------------------
byte TSnap7MicroClient::WordToBCD(word Value)
{
    return ((Value / 10) << 4) | (Value % 10);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::opGetDateTime()
{
    PTimeStruct DateTime;
    PReqFunDateTime ReqParams;
    PReqDataGetDateTime ReqData;
    PS7ResParams7 ResParams;
    PResDataGetTime ResData;
    PS7ResHeader17 Answer;
    int IsoSize, Result;
    word AYear;

    // Setup pointers (note : PDUH_out and PDU.Payload are the same pointer)
    ReqParams=PReqFunDateTime(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
    ReqData  =PReqDataGetDateTime(pbyte(ReqParams)+sizeof(TReqFunDateTime));
    Answer   =PS7ResHeader17(&PDU.Payload);
    ResParams=PS7ResParams7(pbyte(Answer)+ResHeaderSize17);
    ResData  =PResDataGetTime(pbyte(ResParams)+sizeof(TS7Params7));
    DateTime =PTimeStruct(Job.pData);
    // Fill Header
    PDUH_out->P=0x32;                    // Always 0x32
    PDUH_out->PDUType=PduType_userdata;  // 0x07
    PDUH_out->AB_EX=0x0000;              // Always 0x0000
    PDUH_out->Sequence=GetNextWord();    // AutoInc
    PDUH_out->ParLen=SwapWord(sizeof(TReqFunDateTime));   // 8 bytes params
    PDUH_out->DataLen=SwapWord(sizeof(TReqDataGetDateTime)); // 4 bytes data
    // Fill params (mostly constants)
    ReqParams->Head[0]=0x00;
    ReqParams->Head[1]=0x01;
    ReqParams->Head[2]=0x12;
    ReqParams->Plen   =0x04;
    ReqParams->Uk     =0x11;
    ReqParams->Tg     =grClock;
    ReqParams->SubFun =SFun_ReadClock;
    ReqParams->Seq    =0x00;
    // Fill Data
    *ReqData =0x0000000A;

    IsoSize=sizeof(TS7ReqHeader)+sizeof(TReqFunDateTime)+sizeof(TReqDataGetDateTime);
    Result=isoExchangeBuffer(0,IsoSize);

    // Get Data
    if (Result==0)
    {
        if (ResParams->Err==0)
        {
            if (ResData->RetVal==0xFF) // <-- 0xFF means Result OK
            {
                // Decode Plc Date and Time
                AYear=BCDtoByte(ResData->Time[0]);
                if (AYear<90)
                  AYear=AYear+100;
                DateTime->tm_year=AYear;
                DateTime->tm_mon =BCDtoByte(ResData->Time[1])-1;
                DateTime->tm_mday=BCDtoByte(ResData->Time[2]);
                DateTime->tm_hour=BCDtoByte(ResData->Time[3]);
                DateTime->tm_min =BCDtoByte(ResData->Time[4]);
                DateTime->tm_sec =BCDtoByte(ResData->Time[5]);
                DateTime->tm_wday=(ResData->Time[7] & 0x0F)-1;
            }
            else
                Result=CpuError(ResData->RetVal);
        }
        else
            Result=CpuError(ResData->RetVal);
    }
    return Result;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::opSetDateTime()
{
    PTimeStruct DateTime;
    PReqFunDateTime ReqParams;
    PReqDataSetTime ReqData;
    PS7ResParams7 ResParams;
    PS7ResHeader17 Answer;
    word AYear;
    int IsoSize, Result;

    // Setup pointers (note : PDUH_out and PDU.Payload are the same pointer)
    ReqParams=PReqFunDateTime(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
    ReqData  =PReqDataSetTime(pbyte(ReqParams)+sizeof(TReqFunDateTime));
    Answer   =PS7ResHeader17(&PDU.Payload);
    ResParams=PS7ResParams7(pbyte(Answer)+ResHeaderSize17);
    DateTime =PTimeStruct(Job.pData);
    // Fill Header
    PDUH_out->P=0x32;                    // Always 0x32
    PDUH_out->PDUType=PduType_userdata;  // 0x07
    PDUH_out->AB_EX=0x0000;              // Always 0x0000
    PDUH_out->Sequence=GetNextWord();    // AutoInc
    PDUH_out->ParLen=SwapWord(sizeof(TReqFunDateTime));  // 8 bytes params
    PDUH_out->DataLen=SwapWord(sizeof(TReqDataSetTime)); // 4 bytes data
    // Fill params (mostly constants)
    ReqParams->Head[0]=0x00;
    ReqParams->Head[1]=0x01;
    ReqParams->Head[2]=0x12;
    ReqParams->Plen   =0x04;
    ReqParams->Uk     =0x11;
    ReqParams->Tg     =grClock;
    ReqParams->SubFun =SFun_SetClock;
    ReqParams->Seq    =0x00;
    // EncodeSiemensDateTime;
    if (DateTime->tm_year<100)
        AYear=DateTime->tm_year;
    else
        AYear=DateTime->tm_year-100;

    ReqData->RetVal=0xFF;
    ReqData->TSize =TS_ResOctet;
    ReqData->Length=SwapWord(0x000A);
    ReqData->Rsvd  =0x00;
    ReqData->HiYear=0x19; // *must* be 19 tough it's not the Hi part of the year...

    ReqData->Time[0]=WordToBCD(AYear);
    ReqData->Time[1]=WordToBCD(DateTime->tm_mon+1);
    ReqData->Time[2]=WordToBCD(DateTime->tm_mday);
    ReqData->Time[3]=WordToBCD(DateTime->tm_hour);
    ReqData->Time[4]=WordToBCD(DateTime->tm_min);
    ReqData->Time[5]=WordToBCD(DateTime->tm_sec);
    ReqData->Time[6]=0;
    ReqData->Time[7]=DateTime->tm_wday+1;

    IsoSize=sizeof(TS7ReqHeader)+sizeof(TReqFunDateTime)+sizeof(TReqDataSetTime);
    Result=isoExchangeBuffer(0,IsoSize);

    // Get Result
    if (Result==0)
    {
       if (ResParams->Err!=0)
          Result=CpuError(SwapWord(ResParams->Err));
    };
    return Result;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::opGetOrderCode()
{
    PS7OrderCode OC;
    int Result;

    Job.ID     =0x0011;
    Job.Index  =0x0000;
    Job.IParam =0;
    Result     =opReadSZL();
    if (Result==0)
    {
        OC=PS7OrderCode(Job.pData);
        memset(OC,0,sizeof(TS7OrderCode));
        memcpy(OC->Code,&opData[6],20);
        OC->V1=opData[opSize-3];
        OC->V2=opData[opSize-2];
        OC->V3=opData[opSize-1];
    };
    return Result;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::opGetCpuInfo()
{
    PS7CpuInfo Info;
    int Result;

    // Store Pointer
    Info=PS7CpuInfo(Job.pData);
    // Clear data in order to have the end of strings (\0) correctly setted
    memset(Info, 0, sizeof(TS7CpuInfo));

    Job.ID    =0x001C;
    Job.Index =0x0000;
    Job.IParam=0;
    Result    =opReadSZL();
    if (Result==0)
    {
        memcpy(Info->ModuleTypeName,&opData[176],32);
        memcpy(Info->SerialNumber,&opData[142],24);
		memcpy(Info->ASName,&opData[6],24);
        memcpy(Info->Copyright,&opData[108],26);
        memcpy(Info->ModuleName,&opData[40],24);
    }
    return Result;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::opGetCpInfo()
{
    PS7CpInfo Info;
    int Result;
    // Store Pointer
    Info=PS7CpInfo(Job.pData);
    memset(Info,0,sizeof(TS7CpInfo));
    Job.ID    =0x0131;
    Job.Index =0x0001;
    Job.IParam=0;
    Result    =opReadSZL();
    if (Result==0)
    {
        Info->MaxPduLengt=opData[6]*256+opData[7];
        Info->MaxConnections=opData[8]*256+opData[9];
        Info->MaxMpiRate=DWordAt(&opData[10]);
        Info->MaxBusRate=DWordAt(&opData[14]);
    };
    return Result;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::opGetPlcStatus()
{
    int *Status;
    int Result;

    Status =(int*)Job.pData;
    Job.ID     =0x0424;
    Job.Index  =0x0000;
    Job.IParam =0;
    Result     =opReadSZL();
    if (Result==0)
    {
        switch (opData[7])
        {
            case S7CpuStatusUnknown :
            case S7CpuStatusRun     :
            case S7CpuStatusStop    : *Status=opData[7];
            break;
            default :
            // Since RUN status is always $08 for all CPUs and CPs, STOP status
            // sometime can be coded as $03 (especially for old cpu...)
                *Status=S7CpuStatusStop;
        }
    }
    else
        *Status=0;
    return Result;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::opPlcStop()
{
    PReqFunPlcStop ReqParams;
    PResFunCtrl ResParams;
    PS7ResHeader23 Answer;
    int IsoSize, Result;

    char p_program[] = {'P','_','P','R','O','G','R','A','M'};

    // Setup pointers (note : PDUH_out and PDU.Payload are the same pointer)
    ReqParams=PReqFunPlcStop(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
    Answer   =PS7ResHeader23(&PDU.Payload);
    ResParams=PResFunCtrl(pbyte(Answer)+ResHeaderSize23);
    // Fill Header
    PDUH_out->P=0x32;                    // Always 0x32
    PDUH_out->PDUType=PduType_request;   // 0x01
    PDUH_out->AB_EX=0x0000;              // Always 0x0000
    PDUH_out->Sequence=GetNextWord();    // AutoInc
    PDUH_out->ParLen=SwapWord(sizeof(TReqFunPlcStop));
    PDUH_out->DataLen=0x0000;            // No Data
    // Fill Params
    ReqParams->Fun=pduStop;
    memset(ReqParams->Uk_5,0,5);
    ReqParams->Len_2=0x09;
    memcpy(ReqParams->Cmd,&p_program,9);

    IsoSize=sizeof(TS7ReqHeader)+sizeof(TReqFunPlcStop);
    Result=isoExchangeBuffer(0,IsoSize);

    if (Result==0)
    {
        if (Answer->Error!=0)
        {
          if (ResParams->ResFun!=pduStop)
             Result=errCliCannotStopPLC;
          else
             if (ResParams->para ==0x07)
                 Result=errCliAlreadyStop;
             else
                 Result=errCliCannotStopPLC;
        };
    };
    return Result;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::opPlcHotStart()
{
    PReqFunPlcHotStart ReqParams;
    PResFunCtrl ResParams;
    PS7ResHeader23 Answer;
    int IsoSize, Result;

    char p_program[] = {'P','_','P','R','O','G','R','A','M'};

    // Setup pointers (note : PDUH_out and PDU.Payload are the same pointer)
    ReqParams=PReqFunPlcHotStart(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
    Answer   =PS7ResHeader23(&PDU.Payload);
    ResParams=PResFunCtrl(pbyte(Answer)+ResHeaderSize23);
    // Fill Header
    PDUH_out->P=0x32;                    // Always 0x32
    PDUH_out->PDUType=PduType_request;   // 0x01
    PDUH_out->AB_EX=0x0000;              // Always 0x0000
    PDUH_out->Sequence=GetNextWord();    // AutoInc
    PDUH_out->ParLen=SwapWord(sizeof(TReqFunPlcHotStart));   // 16 bytes params
    PDUH_out->DataLen=0x0000;            // No Data
    // Fill Params
    ReqParams->Fun=pduStart;
    ReqParams->Uk_7[0]=0x00;
    ReqParams->Uk_7[1]=0x00;
    ReqParams->Uk_7[2]=0x00;
    ReqParams->Uk_7[3]=0x00;
    ReqParams->Uk_7[4]=0x00;
    ReqParams->Uk_7[5]=0x00;
    ReqParams->Uk_7[6]=0xFD;

    ReqParams->Len_1=0x0000;
    ReqParams->Len_2=0x09;
    memcpy(ReqParams->Cmd,&p_program,9);

    IsoSize=sizeof(TS7ReqHeader)+sizeof(TReqFunPlcHotStart);
    Result=isoExchangeBuffer(0,IsoSize);

    if (Result==0)
    {
        if ((Answer->Error!=0))
        {
          if ((ResParams->ResFun!=pduStart))
              Result=errCliCannotStartPLC;
          else
          {
              if (ResParams->para==0x03)
                  Result=errCliAlreadyRun;
              else
                  if (ResParams->para==0x02)
                      Result=errCliCannotStartPLC;
                  else
                      Result=errCliCannotStartPLC;
          }
        }
    };
    return Result;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::opPlcColdStart()
{
    PReqFunPlcColdStart ReqParams;
    PResFunCtrl ResParams;
    PS7ResHeader23 Answer;
    int IsoSize, Result;
    char p_program[] = {'P','_','P','R','O','G','R','A','M'};

    // Setup pointers (note : PDUH_out and PDU.Payload are the same pointer)
    ReqParams=PReqFunPlcColdStart(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
    Answer   =PS7ResHeader23(&PDU.Payload);
    ResParams=PResFunCtrl(pbyte(Answer)+ResHeaderSize23);
    // Fill Header
    PDUH_out->P=0x32;                     // Always 0x32
    PDUH_out->PDUType=PduType_request;   // 0x01
    PDUH_out->AB_EX=0x0000;               // Always 0x0000
    PDUH_out->Sequence=GetNextWord();      // AutoInc
    PDUH_out->ParLen=SwapWord(sizeof(TReqFunPlcColdStart));   // 22 bytes params
    PDUH_out->DataLen=0x0000;             // No Data
    // Fill Params
    ReqParams->Fun=pduStart;
    ReqParams->Uk_7[0]=0x00;
    ReqParams->Uk_7[1]=0x00;
    ReqParams->Uk_7[2]=0x00;
    ReqParams->Uk_7[3]=0x00;
    ReqParams->Uk_7[4]=0x00;
    ReqParams->Uk_7[5]=0x00;
    ReqParams->Uk_7[6]=0xFD;

    ReqParams->Len_1=SwapWord(0x0002);
    ReqParams->SFun =SwapWord(0x4320); // Cold start
    ReqParams->Len_2=0x09;
    memcpy(ReqParams->Cmd,&p_program,9);

    IsoSize=sizeof(TS7ReqHeader)+sizeof(TReqFunPlcColdStart);
    Result=isoExchangeBuffer(0,IsoSize);

    if (Result==0)
    {
        if ((Answer->Error!=0))
        {
          if ((ResParams->ResFun!=pduStart))
              Result=errCliCannotStartPLC;
          else
          {
              if (ResParams->para==0x03)
                  Result=errCliAlreadyRun;
              else
                  if (ResParams->para==0x02)
                      Result=errCliCannotStartPLC;
                  else
                      Result=errCliCannotStartPLC;
          }
        }
    };
    return Result;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::opCopyRamToRom()
{
    PReqFunCopyRamToRom ReqParams;
    PResFunCtrl ResParams;
    PS7ResHeader23 Answer;
    int IsoSize, CurTimeout, Result;
    char _modu[] = {'_','M','O','D','U'};

    // Setup pointers (note : PDUH_out and PDU.Payload are the same pointer)
    ReqParams=PReqFunCopyRamToRom(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
    Answer   =PS7ResHeader23(&PDU.Payload);
    ResParams=PResFunCtrl(pbyte(Answer)+ResHeaderSize23);
    // Fill Header
    PDUH_out->P=0x32;                    // Always 0x32
    PDUH_out->PDUType=PduType_request;   // 0x01
    PDUH_out->AB_EX=0x0000;              // Always 0x0000
    PDUH_out->Sequence=GetNextWord();    // AutoInc
    PDUH_out->ParLen=SwapWord(sizeof(TReqFunCopyRamToRom));
    PDUH_out->DataLen=0x0000;             // No Data
    // Fill Params
    ReqParams->Fun=pduControl;
    ReqParams->Uk_7[0]=0x00;
    ReqParams->Uk_7[1]=0x00;
    ReqParams->Uk_7[2]=0x00;
    ReqParams->Uk_7[3]=0x00;
    ReqParams->Uk_7[4]=0x00;
    ReqParams->Uk_7[5]=0x00;
    ReqParams->Uk_7[6]=0xFD;

    ReqParams->Len_1=SwapWord(0x0002);
    ReqParams->SFun =SwapWord(0x4550);
    ReqParams->Len_2=0x05;
    memcpy(ReqParams->Cmd,&_modu,5);

    IsoSize=sizeof(TS7ReqHeader)+sizeof(TReqFunCopyRamToRom);
    // Changes the timeout
    CurTimeout=RecvTimeout;
    RecvTimeout=Job.IParam;
    Result=isoExchangeBuffer(0,IsoSize);
    // Restores the timeout
    RecvTimeout=CurTimeout;

    if (Result==0)
    {
        if ((Answer->Error!=0) || (ResParams->ResFun!=pduControl))
             Result=errCliCannotCopyRamToRom;
    }
    return Result;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::opCompress()
{
    PReqFunCompress ReqParams;
    PResFunCtrl ResParams;
    PS7ResHeader23 Answer;
    int IsoSize, CurTimeout, Result;
    char _garb[] = {'_','G','A','R','B'};

    // Setup pointers (note : PDUH_out and PDU.Payload are the same pointer)
    ReqParams=PReqFunCompress(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
    Answer   =PS7ResHeader23(&PDU.Payload);
    ResParams=PResFunCtrl(pbyte(Answer)+ResHeaderSize23);
    // Fill Header
    PDUH_out->P=0x32;                    // Always 0x32
    PDUH_out->PDUType=PduType_request;   // 0x01
    PDUH_out->AB_EX=0x0000;              // Always 0x0000
    PDUH_out->Sequence=GetNextWord();    // AutoInc
    PDUH_out->ParLen=SwapWord(sizeof(TReqFunCompress));
    PDUH_out->DataLen=0x0000;            // No Data
    // Fill Params
    ReqParams->Fun=pduControl;
    ReqParams->Uk_7[0]=0x00;
    ReqParams->Uk_7[1]=0x00;
    ReqParams->Uk_7[2]=0x00;
    ReqParams->Uk_7[3]=0x00;
    ReqParams->Uk_7[4]=0x00;
    ReqParams->Uk_7[5]=0x00;
    ReqParams->Uk_7[6]=0xFD;

    ReqParams->Len_1=0x0000;
    ReqParams->Len_2=0x05;
    memcpy(ReqParams->Cmd,&_garb,5);

    IsoSize=sizeof(TS7ReqHeader)+sizeof(TReqFunCompress);
    // Changes the timeout
    CurTimeout=RecvTimeout;
    RecvTimeout=Job.IParam;
    Result=isoExchangeBuffer(0,IsoSize);
    // Restores the timeout
    RecvTimeout=CurTimeout;

    if (Result==0)
    {
         if (((Answer->Error!=0) || (ResParams->ResFun!=pduControl)))
              Result=errCliCannotCompress;
    }
    return Result;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::opGetProtection()
{
    PS7Protection Info, usrInfo;
    int Result;

    // Store Pointer
    usrInfo=PS7Protection(Job.pData);
    memset(usrInfo, 0, sizeof(TS7Protection));

    Job.ID    =0x0232;
    Job.Index =0x0004;
    Job.IParam=0; // No copy in Usr Data pointed by Job.pData
    Result    =opReadSZL();
    if (Result==0)
    {
        Info=PS7Protection(pbyte(&opData)+6);
        usrInfo->sch_schal=SwapWord(Info->sch_schal);
        usrInfo->sch_par  =SwapWord(Info->sch_par);
        usrInfo->sch_rel  =SwapWord(Info->sch_rel);
        usrInfo->bart_sch =SwapWord(Info->bart_sch);
        usrInfo->anl_sch  =SwapWord(Info->anl_sch);
    }
    return Result;
}
//******************************************************************************
//                                     NOTE
//          PASSWORD HACKING IS VERY FAR FROM THE AIM OF THIS PROJECT
//  NEXT FUNCTION ONLY ENCODES THE ASCII PASSWORD TO BE DOWNLOADED IN THE PLC.
//
//       MOREOVER **YOU NEED TO KNOW** THE CORRECT PASSWORD TO MEET THE CPU
//                                SECURITY LEVEL
//******************************************************************************
int TSnap7MicroClient::opSetPassword()
{
    PReqFunSecurity ReqParams;
    PReqDataSecurity ReqData;
    PResParamsSecurity ResParams;
    PS7ResHeader23 Answer;
    int c, IsoSize, Result;

    // Setup pointers (note : PDUH_out and PDU.Payload are the same pointer)
    ReqParams=PReqFunSecurity(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
    ReqData  =PReqDataSecurity(pbyte(ReqParams)+sizeof(TReqFunSecurity));
    Answer   =PS7ResHeader23(&PDU.Payload);
    ResParams=PResParamsSecurity(pbyte(Answer)+ResHeaderSize17);
    // Fill Header
    PDUH_out->P=0x32;                    // Always 0x32
    PDUH_out->PDUType=PduType_userdata;  // 0x07
    PDUH_out->AB_EX=0x0000;              // Always 0x0000
    PDUH_out->Sequence=GetNextWord();    // AutoInc
    PDUH_out->ParLen =SwapWord(sizeof(TReqFunSecurity));
    PDUH_out->DataLen=SwapWord(sizeof(TReqDataSecurity));
    // Fill params (mostly constants)
    ReqParams->Head[0]=0x00;
    ReqParams->Head[1]=0x01;
    ReqParams->Head[2]=0x12;
    ReqParams->Plen   =0x04;
    ReqParams->Uk     =0x11;
    ReqParams->Tg     =grSecurity;
    ReqParams->SubFun =SFun_EnterPwd;
    ReqParams->Seq    =0x00;
    // Fill Data
    ReqData->Ret      =0xFF;
    ReqData->TS       =TS_ResOctet;
    ReqData->DLen     =SwapWord(0x0008); // 8 bytes data : password
    // Encode the password
    ReqData->Pwd[0]=opData[0] ^ 0x55;
    ReqData->Pwd[1]=opData[1] ^ 0x55;
    for (c = 2; c < 8; c++){
      ReqData->Pwd[c]=opData[c] ^ 0x55 ^ ReqData->Pwd[c-2];
    };

    IsoSize=sizeof(TS7ReqHeader)+sizeof(TReqFunSecurity)+sizeof(TReqDataSecurity);
    Result=isoExchangeBuffer(0,IsoSize);

    // Get Return
    if (Result==0)
    {
        if (ResParams->Err!=0)
              Result=CpuError(SwapWord(ResParams->Err));
    };

    return Result;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::opClearPassword()
{
    PReqFunSecurity ReqParams;
    PReqDataSecurity ReqData;
    PResParamsSecurity ResParams;
    PS7ResHeader23 Answer;
    int IsoSize, Result;

    // Setup pointers (note : PDUH_out and PDU.Payload are the same pointer)
    ReqParams=PReqFunSecurity(pbyte(PDUH_out)+sizeof(TS7ReqHeader));
    ReqData  =PReqDataSecurity(pbyte(ReqParams)+sizeof(TReqFunSecurity));
    Answer   =PS7ResHeader23(&PDU.Payload);
    ResParams=PResParamsSecurity(pbyte(Answer)+ResHeaderSize17);
    // Fill Header
    PDUH_out->P=0x32;                     // Always 0x32
    PDUH_out->PDUType=PduType_userdata;   // 0x07
    PDUH_out->AB_EX=0x0000;               // Always 0x0000
    PDUH_out->Sequence=GetNextWord();     // AutoInc
    PDUH_out->ParLen =SwapWord(sizeof(TReqFunSecurity));
    PDUH_out->DataLen=SwapWord(0x0004);   // We need only 4 bytes
    // Fill params (mostly constants)
    ReqParams->Head[0]=0x00;
    ReqParams->Head[1]=0x01;
    ReqParams->Head[2]=0x12;
    ReqParams->Plen   =0x04;
    ReqParams->Uk     =0x11;
    ReqParams->Tg     =grSecurity;
    ReqParams->SubFun =SFun_CancelPwd;
    ReqParams->Seq    =0x00;
    // Fill Data
    ReqData->Ret      =0x0A;
    ReqData->TS       =0x00;
    ReqData->DLen     =0x0000;

    IsoSize=sizeof(TS7ReqHeader)+sizeof(TReqFunSecurity)+4;
    Result=isoExchangeBuffer(0,IsoSize);

    // Get Return
    if (Result==0)
    {
       if (ResParams->Err!=0)
            Result=CpuError(SwapWord(ResParams->Err));
    };
    return Result;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::CpuError(int Error)
{
  switch(Error)
  {
    case 0                          : return 0;
    case Code7AddressOutOfRange     : return errCliAddressOutOfRange;
    case Code7InvalidTransportSize  : return errCliInvalidTransportSize;
    case Code7WriteDataSizeMismatch : return errCliWriteDataSizeMismatch;
    case Code7ResItemNotAvailable   :
    case Code7ResItemNotAvailable1  : return errCliItemNotAvailable;
    case Code7DataOverPDU           : return errCliSizeOverPDU;
    case Code7InvalidValue          : return errCliInvalidValue;
    case Code7FunNotAvailable       : return errCliFunNotAvailable;
    case Code7NeedPassword          : return errCliNeedPassword;
    case Code7InvalidPassword       : return errCliInvalidPassword;
    case Code7NoPasswordToSet       :
    case Code7NoPasswordToClear     : return errCliNoPasswordToSetOrClear;
  default:
    return errCliFunctionRefused;
  };
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::DataSizeByte(int WordLength)
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
//---------------------------------------------------------------------------
longword TSnap7MicroClient::DWordAt(void * P)
{
     longword DW;
     DW=*(longword*)P;
     return SwapDWord(DW);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::CheckBlock(int BlockType, int BlockNum,  void * pBlock,  int Size)
{
      PS7CompactBlockInfo Info = PS7CompactBlockInfo(pBlock);

      if (BlockType>=0) // if (BlockType<0 the test is skipped
      {
        if ((BlockType!=Block_OB)&&(BlockType!=Block_DB)&&(BlockType!=Block_FB)&&
           (BlockType!=Block_FC)&&(BlockType!=Block_SDB)&&(BlockType!=Block_SFC)&&
           (BlockType!=Block_SFB))
          return errCliInvalidBlockType;
      }

      if (BlockNum>=0)  // if (BlockNum<0 the test is skipped
      {
          if (BlockNum>0xFFFF)
            return errCliInvalidBlockNumber;
      };

      if (SwapDWord(Info->LenLoadMem)!=longword(Size))
          return errCliInvalidBlockSize;

  // Check the presence of the footer
  if (SwapWord(Info->MC7Len)+sizeof(TS7CompactBlockInfo)>=u_int(Size))
    return errCliInvalidBlockSize;

  return 0;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::SubBlockToBlock(int SBB)
{
  switch (SBB)
  {
    case SubBlk_OB  : return Block_OB;
    case SubBlk_DB  : return Block_DB;
    case SubBlk_SDB : return Block_SDB;
    case SubBlk_FC  : return Block_FC;
    case SubBlk_SFC : return Block_SFC;
    case SubBlk_FB  : return Block_FB;
    case SubBlk_SFB : return Block_SFB;
    default : return 0;
  };
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::PerformOperation()
{
    ClrError();
    int Operation=Job.Op;
    switch(Operation)
    {
        case s7opNone:
             Job.Result=errCliInvalidParams;
             break;
        case s7opReadArea:
             Job.Result=opReadArea();
             break;
        case s7opWriteArea:
             Job.Result=opWriteArea();
             break;
        case s7opReadMultiVars:
             Job.Result=opReadMultiVars();
             break;
        case s7opWriteMultiVars:
             Job.Result=opWriteMultiVars();
             break;
        case s7opDBGet:
             Job.Result=opDBGet();
             break;
        case s7opDBFill:
             Job.Result=opDBFill();
             break;
        case s7opUpload:
             Job.Result=opUpload();
             break;
        case s7opDownload:
             Job.Result=opDownload();
             break;
        case s7opDelete:
             Job.Result=opDelete();
             break;
        case s7opListBlocks:
             Job.Result=opListBlocks();
             break;
        case s7opAgBlockInfo:
             Job.Result=opAgBlockInfo();
             break;
        case s7opListBlocksOfType:
             Job.Result=opListBlocksOfType();
             break;
        case s7opReadSzlList:
             Job.Result=opReadSZLList();
             break;
        case s7opReadSZL:
             Job.Result=opReadSZL();
             break;
        case s7opGetDateTime:
             Job.Result=opGetDateTime();
             break;
        case s7opSetDateTime:
             Job.Result=opSetDateTime();
             break;
        case s7opGetOrderCode:
             Job.Result=opGetOrderCode();
             break;
        case s7opGetCpuInfo:
             Job.Result=opGetCpuInfo();
             break;
        case s7opGetCpInfo:
             Job.Result=opGetCpInfo();
             break;
        case s7opGetPlcStatus:
             Job.Result=opGetPlcStatus();
             break;
        case s7opPlcHotStart:
             Job.Result=opPlcHotStart();
             break;
        case s7opPlcColdStart:
             Job.Result=opPlcColdStart();
             break;
        case s7opCopyRamToRom:
             Job.Result=opCopyRamToRom();
             break;
        case s7opCompress:
             Job.Result=opCompress();
             break;
        case s7opPlcStop:
             Job.Result=opPlcStop();
             break;
        case s7opGetProtection:
             Job.Result=opGetProtection();
             break;
        case s7opSetPassword:
             Job.Result=opSetPassword();
             break;
        case s7opClearPassword:
             Job.Result=opClearPassword();
             break;
    }
   Job.Time =SysGetTick()-JobStart;
   Job.Pending=false;
   return SetError(Job.Result);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::Disconnect()
{
     JobStart=SysGetTick();
     PeerDisconnect();
     Job.Time=SysGetTick()-JobStart;
	 Job.Pending=false;
     return 0;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::Reset(bool DoReconnect)
{
    Job.Pending=false;
    if (DoReconnect) {
        Disconnect();
        return Connect();
    }
    else
        return 0;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::Connect()
{
	 int Result;
	 JobStart=SysGetTick();
	 Result  =PeerConnect();
	 Job.Time=SysGetTick()-JobStart;
	 return Result;
}
//---------------------------------------------------------------------------
void TSnap7MicroClient::SetConnectionType(word ConnType)
{
    ConnectionType=ConnType;
}
//---------------------------------------------------------------------------
void TSnap7MicroClient::SetConnectionParams(const char *RemAddress, word LocalTSAP, word RemoteTSAP)
{
     SrcTSap = LocalTSAP;
     DstTSap = RemoteTSAP;
     strncpy(RemoteAddress, RemAddress, 16);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::ConnectTo(const char *RemAddress, int Rack, int Slot)
{
    word RemoteTSAP = (ConnectionType<<8)+(Rack*0x20)+Slot;
    SetConnectionParams(RemAddress, SrcTSap, RemoteTSAP);
    return Connect();
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::GetParam(int ParamNumber, void *pValue)
{
    switch (ParamNumber)
    {
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
	default: return errCliInvalidParamNumber;
    }
    return 0;
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::SetParam(int ParamNumber, void *pValue)
{
    switch (ParamNumber)
    {
	case p_u16_RemotePort:
		if (!Connected)
            RemotePort=*Puint16_t(pValue);
		else
		    return errCliCannotChangeParam;
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
	default: return errCliInvalidParamNumber;
    }
    return 0;
}
//---------------------------------------------------------------------------
// Data I/O functions
int TSnap7MicroClient::ReadArea(int Area, int DBNumber, int Start, int Amount, int WordLen,  void * pUsrData)
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
         return PerformOperation();
     }
     else
         return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::WriteArea(int Area, int DBNumber, int Start, int Amount, int WordLen,  void * pUsrData)
{
     if (!Job.Pending)
     {
          Job.Pending  = true;
          Job.Op       = s7opWriteArea;
          Job.Area     = Area;
          Job.Number   = DBNumber;
          Job.Start    = Start;
          Job.Amount   = Amount;
          Job.WordLen  = WordLen;
          Job.pData    = pUsrData;
          JobStart     = SysGetTick();
          return PerformOperation();
     }
     else
          return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::ReadMultiVars(PS7DataItem Item, int ItemsCount)
{
    if (!Job.Pending)
    {
        Job.Pending  =true;
        Job.Op       =s7opReadMultiVars;
        Job.Amount   =ItemsCount;
        Job.pData    =Item;
        JobStart     =SysGetTick();
        return PerformOperation();
    }
    else
    	return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::WriteMultiVars(PS7DataItem Item, int ItemsCount)
{
    if (!Job.Pending)
    {
        Job.Pending  =true;
        Job.Op       =s7opWriteMultiVars;
        Job.Amount   =ItemsCount;
        Job.pData    =Item;
        JobStart     =SysGetTick();
        return PerformOperation();
    }
    else
    	return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::DBRead(int DBNumber, int Start, int Size,  void * pUsrData)
{
     return ReadArea(S7AreaDB, DBNumber, Start, Size, S7WLByte, pUsrData);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::DBWrite(int DBNumber, int Start, int Size,  void * pUsrData)
{
     return WriteArea(S7AreaDB, DBNumber, Start, Size, S7WLByte, pUsrData);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::MBRead(int Start, int Size,  void * pUsrData)
{
     return ReadArea(S7AreaMK, 0, Start, Size, S7WLByte, pUsrData);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::MBWrite(int Start, int Size,  void * pUsrData)
{
     return WriteArea(S7AreaMK, 0, Start, Size, S7WLByte, pUsrData);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::EBRead(int Start, int Size,  void * pUsrData)
{
    return ReadArea(S7AreaPE, 0, Start, Size, S7WLByte, pUsrData);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::EBWrite(int Start, int Size,  void * pUsrData)
{
    return WriteArea(S7AreaPE, 0, Start, Size, S7WLByte, pUsrData);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::ABRead(int Start, int Size,  void * pUsrData)
{
    return ReadArea(S7AreaPA, 0, Start, Size, S7WLByte, pUsrData);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::ABWrite(int Start, int Size,  void * pUsrData)
{
    return WriteArea(S7AreaPA, 0, Start, Size, S7WLByte, pUsrData);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::TMRead(int Start, int Amount,  void * pUsrData)
{
    return ReadArea(S7AreaTM, 0, Start, Amount, S7WLTimer, pUsrData);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::TMWrite(int Start, int Amount,  void * pUsrData)
{
    return WriteArea(S7AreaTM, 0, Start, Amount, S7WLTimer, pUsrData);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::CTRead(int Start, int Amount,  void * pUsrData)
{
    return ReadArea(S7AreaCT, 0, Start, Amount, S7WLCounter, pUsrData);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::CTWrite(int Start, int Amount,  void * pUsrData)
{
    return WriteArea(S7AreaCT, 0, Start, Amount, S7WLCounter, pUsrData);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::ListBlocks(PS7BlocksList pUsrData)
{
    if (!Job.Pending)
    {
        Job.Pending  =true;
        Job.Op       =s7opListBlocks;
        Job.pData    =pUsrData;
        JobStart     =SysGetTick();
        return PerformOperation();
    }
    else
    	return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::GetAgBlockInfo(int BlockType, int BlockNum, PS7BlockInfo pUsrData)
{
    if (!Job.Pending)
    {
        Job.Pending  =true;
        Job.Op       =s7opAgBlockInfo;
        Job.Area     =BlockType;
        Job.Number   =BlockNum;
        Job.pData    =pUsrData;
        JobStart     =SysGetTick();
        return PerformOperation();
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::GetPgBlockInfo(void * pBlock, PS7BlockInfo pUsrData, int Size)
{
    PS7CompactBlockInfo Info;
    PS7BlockFooter Footer;

    int Result=CheckBlock(-1,-1,pBlock,Size);
    if (Result==0)
    {
        Info=PS7CompactBlockInfo(pBlock);
        pUsrData->BlkType  =Info->SubBlkType;
        pUsrData->BlkNumber=SwapWord(Info->BlkNum);
        pUsrData->BlkLang  =Info->BlkLang;
        pUsrData->BlkFlags =Info->BlkFlags;
        pUsrData->MC7Size  =SwapWord(Info->MC7Len);
        pUsrData->LoadSize =SwapDWord(Info->LenLoadMem);
        pUsrData->LocalData=SwapDWord(Info->LocDataLen);
        pUsrData->SBBLength=SwapDWord(Info->SbbLen);
        pUsrData->CheckSum =0; // this info is not available
        pUsrData->Version  =0; // this info is not available
        FillTime(SwapWord(Info->CodeTime_dy),pUsrData->CodeDate);
        FillTime(SwapWord(Info->IntfTime_dy),pUsrData->IntfDate);

        Footer=PS7BlockFooter(pbyte(Info)+pUsrData->LoadSize-sizeof(TS7BlockFooter));

        memcpy(pUsrData->Author,Footer->Author,8);
        memcpy(pUsrData->Family,Footer->Family,8);
        memcpy(pUsrData->Header,Footer->Header,8);
    };
    return SetError(Result);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::ListBlocksOfType(int BlockType, TS7BlocksOfType *pUsrData, int &ItemsCount)
{
    if (!Job.Pending)
    {
	if (ItemsCount<1)
	    return SetError(errCliInvalidBlockSize);
	Job.Pending  =true;
	Job.Op       =s7opListBlocksOfType;
	Job.Area     =BlockType;
	Job.pData    =pUsrData;
	Job.pAmount  =&ItemsCount;
	Job.Amount   =ItemsCount;
	JobStart     =SysGetTick();
	return PerformOperation();
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::Upload(int BlockType, int BlockNum,  void * pUsrData, int & Size)
{
    if (!Job.Pending)
    {
        if (Size<=0)
	     return SetError(errCliInvalidBlockSize);
        Job.Pending  =true;
        Job.Op       =s7opUpload;
        Job.Area     =BlockType;
        Job.pData    =pUsrData;
        Job.pAmount  =&Size;
        Job.Amount   =Size;
        Job.Number   =BlockNum;
        Job.IParam   =0; // not full upload, only data
        JobStart     =SysGetTick();
        return PerformOperation();
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::FullUpload(int BlockType, int BlockNum, void * pUsrData, int & Size)
{
    if (!Job.Pending)
    {
        if (Size<=0)
            return SetError(errCliInvalidBlockSize);
        Job.Pending  =true;
        Job.Op       =s7opUpload;
        Job.Area     =BlockType;
        Job.pData    =pUsrData;
        Job.pAmount  =&Size;
        Job.Amount   =Size;
        Job.Number   =BlockNum;
        Job.IParam   =1; // header + data + footer
        JobStart     =SysGetTick();
        return PerformOperation();
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::Download(int BlockNum,  void * pUsrData,  int Size)
{
    if (!Job.Pending)
    {
        Job.Pending  =true;
        Job.Op       =s7opDownload;
        memcpy(&opData, pUsrData, Size);
        Job.Number   =BlockNum;
        Job.Amount   =Size;
        JobStart     =SysGetTick();
        return PerformOperation();
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::Delete(int BlockType, int BlockNum)
{
    if (!Job.Pending)
    {
        Job.Pending  =true;
        Job.Op       =s7opDelete;
        Job.Area     =BlockType;
        Job.Number   =BlockNum;
        JobStart     =SysGetTick();
        return PerformOperation();
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::DBGet(int DBNumber, void * pUsrData, int & Size)
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
        return PerformOperation();
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::DBFill(int DBNumber,  int FillChar)
{
    if (!Job.Pending)
    {
        Job.Pending  =true;
        Job.Op       =s7opDBFill;
        Job.Number   =DBNumber;
        Job.IParam   =FillChar;
        JobStart     =SysGetTick();
        return PerformOperation();
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::GetPlcDateTime(tm &DateTime)
{
    if (!Job.Pending)
    {
        Job.Pending  =true;
        Job.Op       =s7opGetDateTime;
        Job.pData    =&DateTime;
        JobStart     =SysGetTick();
        return PerformOperation();
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::SetPlcDateTime(tm * DateTime)
{
    if (!Job.Pending)
    {
        Job.Pending  =true;
        Job.Op       =s7opSetDateTime;
        Job.pData    =DateTime;
        JobStart     =SysGetTick();
        return PerformOperation();
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::SetPlcSystemDateTime()
{
    time_t Now;
    time(&Now);
    struct tm * DateTime = localtime (&Now);
    return SetPlcDateTime(DateTime);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::GetOrderCode(PS7OrderCode pUsrData)
{
    if (!Job.Pending)
    {
        Job.Pending  =true;
        Job.Op       =s7opGetOrderCode;
        Job.pData    =pUsrData;
        JobStart     =SysGetTick();
        return PerformOperation();
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::GetCpuInfo(PS7CpuInfo pUsrData)
{
    if (!Job.Pending)
    {
        Job.Pending  =true;
        Job.Op       =s7opGetCpuInfo;
        Job.pData    =pUsrData;
        JobStart     =SysGetTick();
        return PerformOperation();
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::GetCpInfo(PS7CpInfo pUsrData)
{
    if (!Job.Pending)
    {
        Job.Pending  =true;
        Job.Op       =s7opGetCpInfo;
        Job.pData    =pUsrData;
        JobStart     =SysGetTick();
        return PerformOperation();
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::ReadSZL(int ID, int Index, PS7SZL pUsrData, int &Size)
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
        return PerformOperation();
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::ReadSZLList(PS7SZLList pUsrData, int &ItemsCount)
{
    if (!Job.Pending)
    {
        Job.Pending  =true;
        Job.Op       =s7opReadSzlList;
        Job.pData    =pUsrData;
        Job.pAmount  =&ItemsCount;
        Job.Amount   =ItemsCount;
        JobStart     =SysGetTick();
        return PerformOperation();
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::PlcHotStart()
{
    if (!Job.Pending)
    {
        Job.Pending  =true;
        Job.Op       =s7opPlcHotStart;
        JobStart     =SysGetTick();
        return PerformOperation();
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::PlcColdStart()
{
    if (!Job.Pending)
    {
        Job.Pending  =true;
        Job.Op       =s7opPlcColdStart;
        JobStart     =SysGetTick();
        return PerformOperation();
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::PlcStop()
{
    if (!Job.Pending)
    {
        Job.Pending  =true;
        Job.Op       =s7opPlcStop;
        JobStart     =SysGetTick();
        return PerformOperation();
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::CopyRamToRom(int Timeout)
{
      if (!Job.Pending)
      {
          if (Timeout>0)
          {
               Job.Pending =true;
               Job.Op      =s7opCopyRamToRom;
               Job.IParam  =Timeout;
               JobStart    =SysGetTick();
               return PerformOperation();
          }
          else
              return SetError(errCliInvalidParams);
      }
      else
          return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::Compress(int Timeout)
{
      if (!Job.Pending)
      {
          if (Timeout>0)
          {
               Job.Pending =true;
               Job.Op      =s7opCompress;
               Job.IParam  =Timeout;
               JobStart    =SysGetTick();
               return PerformOperation();
          }
          else
              return SetError(errCliInvalidParams);
      }
      else
          return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::GetPlcStatus(int & Status)
{
    if (!Job.Pending)
    {
        Job.Pending  =true;
        Job.Op       =s7opGetPlcStatus;
        Job.pData    =&Status;
        JobStart     =SysGetTick();
        return PerformOperation();
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::GetProtection(PS7Protection pUsrData)
{
    if (!Job.Pending)
    {
        Job.Pending  =true;
        Job.Op       =s7opGetProtection;
        Job.pData    =pUsrData;
        JobStart     =SysGetTick();
        return PerformOperation();
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::SetSessionPassword(char *Password)
{
    if (!Job.Pending)
    {
        size_t L = strlen(Password);
        // checks the len
        if ((L<1) || (L>8))
           return SetError(errCliInvalidParams);
        Job.Pending  =true;
        // prepares an 8 char string filled with spaces
        memset(&opData,0x20,8);
        // copies
        strncpy((char*)&opData,Password,L);
        Job.Op       =s7opSetPassword;
        JobStart     =SysGetTick();
        return PerformOperation();
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------
int TSnap7MicroClient::ClearSessionPassword()
{
    if (!Job.Pending)
    {
        Job.Pending  =true;
        Job.Op       =s7opClearPassword;
        JobStart     =SysGetTick();
        return PerformOperation();
    }
    else
        return SetError(errCliJobPending);
}
//---------------------------------------------------------------------------

