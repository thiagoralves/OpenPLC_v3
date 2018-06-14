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
#ifndef OPENDNP3_TIMESYNCHANDLER_H
#define OPENDNP3_TIMESYNCHANDLER_H

#include "opendnp3/app/parsing/IAPDUHandler.h"

#include "opendnp3/LogLevels.h"

#include <openpal/logging/LogMacros.h>
#include <openpal/util/Uncopyable.h>

namespace opendnp3
{

/**
 * Dedicated class for processing response data in the master.
 */
class TimeSyncHandler : public IAPDUHandler, private openpal::Uncopyable
{

public:

	TimeSyncHandler() :
		m_valid(false),
		m_time(0)
	{}

	bool GetTimeDelay(uint16_t& time)
	{
		if (this->Errors().Any())
		{
			return false;
		}
		else
		{
			if (m_valid)
			{
				time = m_time;
			}
			return m_valid;
		}
	}

	virtual bool IsAllowed(uint32_t headerCount, GroupVariation gv, QualifierCode qc) override final
	{
		return (headerCount == 0) && (gv == GroupVariation::Group52Var2);
	}

private:

	virtual IINField ProcessHeader(const CountHeader& header, const ICollection<Group52Var2>& times) override final
	{
		Group52Var2 value;
		if (times.ReadOnlyValue(value))
		{
			m_valid = true;
			m_time = value.time;
			return IINField::Empty();
		}
		else
		{
			return IINBit::PARAM_ERROR;
		}
	}

	bool m_valid;
	uint16_t m_time;

};

}



#endif

