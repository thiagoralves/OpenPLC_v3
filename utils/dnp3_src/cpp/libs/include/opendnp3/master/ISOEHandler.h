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
#ifndef OPENDNP3_ISOEHANDLER_H
#define OPENDNP3_ISOEHANDLER_H

#include "opendnp3/app/ITransactable.h"

#include "opendnp3/app/MeasurementTypes.h"
#include "opendnp3/app/BinaryCommandEvent.h"
#include "opendnp3/app/AnalogCommandEvent.h"
#include "opendnp3/app/OctetString.h"
#include "opendnp3/app/SecurityStat.h"

#include "opendnp3/app/Indexed.h"
#include "opendnp3/app/parsing/ICollection.h"

#include "opendnp3/master/HeaderInfo.h"
#include "openpal/executor/UTCTimestamp.h"

namespace opendnp3
{

/**
* An interface for Sequence-Of-Events (SOE) callbacks from a master stack to
* the application layer.
*
* A call is made to the appropriate member method for every measurement value in an ASDU.
* The HeaderInfo class provides information about the object header associated with the value.
*
*/
class ISOEHandler : public ITransactable
{
public:

	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<Binary>>& values) = 0;
	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<DoubleBitBinary>>& values) = 0;
	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<Analog>>& values) = 0;
	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<Counter>>& values) = 0;
	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<FrozenCounter>>& values) = 0;
	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<BinaryOutputStatus>>& values) = 0;
	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<AnalogOutputStatus>>& values) = 0;
	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<OctetString>>& values) = 0;
	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<TimeAndInterval>>& values) = 0;
	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<BinaryCommandEvent>>& values) = 0;
	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<AnalogCommandEvent>>& values) = 0;
	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<SecurityStat>>& values) = 0;
	virtual void Process(const HeaderInfo& info, const ICollection<DNPTime>& values) = 0;

	virtual ~ISOEHandler() {}
};

}

#endif
