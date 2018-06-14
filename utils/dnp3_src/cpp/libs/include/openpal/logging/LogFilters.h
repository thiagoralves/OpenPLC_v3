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
#ifndef OPENPAL_LOGFILTERS_H
#define OPENPAL_LOGFILTERS_H

#include <cstdint>

namespace openpal
{

/**
* Strongly typed wrapper for flags bitfield
*/
class LogFilters
{

public:

	LogFilters() : filters(0)
	{}

	LogFilters(int32_t filters_) : filters(filters_)
	{}

	inline bool IsSet(int32_t levels) const
	{
		return (levels & filters) != 0;
	}

	bool operator &(const LogFilters& rhs) const
	{
		return IsSet(rhs.filters);
	}

	int32_t GetBitfield() const
	{
		return filters;
	}

private:

	int32_t	filters;
};

}

#endif
