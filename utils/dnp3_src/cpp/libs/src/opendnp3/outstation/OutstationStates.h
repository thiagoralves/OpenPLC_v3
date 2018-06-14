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
#ifndef OPENDNP3_OUTSTATIONSTATES_H
#define OPENDNP3_OUTSTATIONSTATES_H

#include "opendnp3/app/APDUHeader.h"

#include <openpal/util/Uncopyable.h>
#include <openpal/container/RSlice.h>

namespace opendnp3
{

class OContext;

/**
 * Base class for the outstation states
 */
class OutstationState : private openpal::Uncopyable
{
public:

	virtual bool IsIdle()
	{
		return false;
	}

	virtual const char* Name() = 0;

	virtual OutstationState& OnConfirm(OContext&, const APDUHeader& header) = 0;

	virtual OutstationState& OnConfirmTimeout(OContext&) = 0;

	virtual OutstationState& OnNewReadRequest(OContext&, const APDUHeader& header, const openpal::RSlice& objects) = 0;

	virtual OutstationState& OnNewNonReadRequest(OContext&, const APDUHeader& header, const openpal::RSlice& objects) = 0;

	virtual OutstationState& OnRepeatNonReadRequest(OContext&, const APDUHeader& header, const openpal::RSlice& objects) = 0;

	virtual OutstationState& OnRepeatReadRequest(OContext&, const APDUHeader& header, const openpal::RSlice& objects) = 0;



};

class StateIdle final : public OutstationState
{

public:

	virtual bool IsIdle() override
	{
		return true;
	}

	virtual const char* Name() override
	{
		return "Idle";
	}

	inline static OutstationState& Inst()
	{
		return instance;
	}

	virtual OutstationState& OnConfirm(OContext&, const APDUHeader& header) override;

	virtual OutstationState& OnConfirmTimeout(OContext&) override;

	virtual OutstationState& OnNewReadRequest(OContext&, const APDUHeader& header, const openpal::RSlice& objects) override;

	virtual OutstationState& OnNewNonReadRequest(OContext&, const APDUHeader& header, const openpal::RSlice& objects) override;

	virtual OutstationState& OnRepeatNonReadRequest(OContext&, const APDUHeader& header, const openpal::RSlice& objects) override;

	virtual OutstationState& OnRepeatReadRequest(OContext&, const APDUHeader& header, const openpal::RSlice& objects) override;

private:

	static StateIdle instance;

	StateIdle() {}

};


/*
* waiting for a confirm to a solicited read response
*/
class StateSolicitedConfirmWait final : public OutstationState
{

public:

	inline static OutstationState& Inst()
	{
		return instance;
	}

	virtual const char* Name() override
	{
		return "SolicitedConfirmWait";
	}

	virtual OutstationState& OnConfirm(OContext&, const APDUHeader& header) override;

	virtual OutstationState& OnConfirmTimeout(OContext&) override;

	virtual OutstationState& OnNewReadRequest(OContext&, const APDUHeader& header, const openpal::RSlice& objects) override;

	virtual OutstationState& OnNewNonReadRequest(OContext&, const APDUHeader& header, const openpal::RSlice& objects) override;

	virtual OutstationState& OnRepeatNonReadRequest(OContext&, const APDUHeader& header, const openpal::RSlice& objects) override;

	virtual OutstationState& OnRepeatReadRequest(OContext&, const APDUHeader& header, const openpal::RSlice& objects) override;

private:

	static StateSolicitedConfirmWait instance;

	StateSolicitedConfirmWait() {}
};

/*
* waiting for a confirm to an unsolicited read response
*/
class StateUnsolicitedConfirmWait final : public OutstationState
{

public:

	inline static OutstationState& Inst()
	{
		return instance;
	}

	virtual const char* Name() override
	{
		return "UnsolicitedConfirmWait";
	}

	virtual OutstationState& OnConfirm(OContext&, const APDUHeader& header) override;

	virtual OutstationState& OnConfirmTimeout(OContext&) override;

	virtual OutstationState& OnNewReadRequest(OContext&, const APDUHeader& header, const openpal::RSlice& objects) override;

	virtual OutstationState& OnNewNonReadRequest(OContext&, const APDUHeader& header, const openpal::RSlice& objects) override;

	virtual OutstationState& OnRepeatNonReadRequest(OContext&, const APDUHeader& header, const openpal::RSlice& objects) override;

	virtual OutstationState& OnRepeatReadRequest(OContext&, const APDUHeader& header, const openpal::RSlice& objects) override;

private:

	static StateUnsolicitedConfirmWait instance;

	StateUnsolicitedConfirmWait() {}
};

} //ens ns



#endif

