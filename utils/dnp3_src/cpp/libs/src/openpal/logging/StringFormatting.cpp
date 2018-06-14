/*
 * Licensed to Green Energy Corp (www.greenenergycorp.com) under one or
 * more contributor license agreements. See the NOTICE file distributed
 * with this work for additional information regarding copyright ownership.
 * Green Energy Corp licenses this file to you under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in
 * compliance with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This project was forked on 01/01/2013 by Automatak, LLC and modifications
 * may have been made to this file. Automatak, LLC licenses these modifications
 * to you under the terms of the License.
 */
#include "openpal/logging/StringFormatting.h"

#include "openpal/logging/LogMacros.h"
#include "openpal/util/ToHex.h"
#include "openpal/container/RSlice.h"
#include "openpal/Configure.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace openpal
{
char* AllocateCopy(char const* src)
{
	auto size = strlen(src) + 1;
	char* tmp = new char[size];
#ifdef WIN32
	strcpy_s(tmp, size, src);
#else
	strcpy(tmp, src);
#endif
	return tmp;
}

void LogHex(Logger& logger, const openpal::LogFilters& filters, const openpal::RSlice& source, uint32_t firstRowSize, uint32_t otherRowSize)
{
	char buffer[MAX_LOG_ENTRY_SIZE];
	RSlice copy(source);
	uint32_t rowCount = 0;
	while (copy.IsNotEmpty())
	{
		uint32_t rowSize = (copy.Size() < MAX_HEX_PER_LINE) ? copy.Size() : MAX_HEX_PER_LINE;
		if (rowCount == 0)
		{
			if (firstRowSize < rowSize)
			{
				rowSize = firstRowSize;
			}
		}
		else
		{
			if (otherRowSize < rowSize)
			{
				rowSize = otherRowSize;
			}
		}
		auto region = copy.Take(rowSize);
		auto pLocation = buffer;
		for (uint32_t pos = 0; pos < rowSize; ++pos)
		{
			pLocation[0] = ToHexChar((region[pos] & 0xf0) >> 4);
			pLocation[1] = ToHexChar(region[pos] & 0xf);
			pLocation[2] = ' ';
			pLocation += 3;
		}
		buffer[3 * rowSize] = '\0';
		copy.Advance(rowSize);

		logger.Log(filters, "", buffer);

		++rowCount;
	}
}

}

