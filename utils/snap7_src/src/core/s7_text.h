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
#ifndef s7_text_h
#define s7_text_h
//---------------------------------------------------------------------------
#include "s7_micro_client.h"
#include "s7_server.h"
#include "s7_partner.h"
//---------------------------------------------------------------------------

const int errLibInvalidParam  = -1;
const int errLibInvalidObject = -2;
// Errors areas definition
const longword ErrTcpMask = 0x0000FFFF;
const longword ErrIsoMask = 0x000F0000;
const longword ErrS7Mask  = 0xFFF00000;

char* ErrCliText(int Error, char* Result, int TextLen);
char* ErrSrvText(int Error, char* Result, int TextLen);
char* ErrParText(int Error, char* Result, int TextLen);
char* EvtSrvText(TSrvEvent &Event, char* Result, int TextLen);


#endif


