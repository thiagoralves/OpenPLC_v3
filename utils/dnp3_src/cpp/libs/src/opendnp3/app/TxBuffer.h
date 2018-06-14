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
#ifndef OPENDNP3_TXBUFFER_H
#define OPENDNP3_TXBUFFER_H

#include <openpal/container/Buffer.h>
#include <opendnp3/app/APDUResponse.h>

namespace opendnp3
{

class TxBuffer
{
public:

	TxBuffer(uint32_t maxTxSize) : buffer(maxTxSize)
	{}

	APDUResponse Start()
	{
		APDUResponse response(buffer.GetWSlice());
		return response;
	}

	void Record(const AppControlField& control, const openpal::RSlice& view)
	{
		this->control = control;
		this->lastResponse = view;
	}

	const openpal::RSlice& GetLastResponse() const
	{
		return lastResponse;
	}

	const AppControlField& GetLastControl() const
	{
		return control;
	}

private:

	openpal::RSlice lastResponse;
	AppControlField control;

	openpal::Buffer buffer;
};

}



#endif

