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
#ifndef __FANOUT_DATA_OBSERVER_H_
#define __FANOUT_DATA_OBSERVER_H_

#include <opendnp3/outstation/IMeasurementLoader.h>

#include <vector>

namespace opendnp3
{

class FanoutDataObserver : public IMeasurementLoader
{
public:

	void AddObserver(IMeasurementLoader* apObserver)
	{
		mObservers.push_back(apObserver);
	}

	void Start()
	{
		for (auto pObs : mObservers) openpal::Transaction::Start(pObs);
	}

	void End()
	{
		for (auto pObs : mObservers) openpal::Transaction::End(pObs);
	}

	void Update(const Binary& arPoint, uint16_t aIndex) override final
	{
		for(auto pObs : mObservers) pObs->Update(arPoint, aIndex);
	}

	void Update(const DoubleBitBinary& arPoint, uint16_t aIndex) override final
	{
		for (auto pObs : mObservers) pObs->Update(arPoint, aIndex);
	}

	void Update(const Analog& arPoint, uint16_t aIndex) override final
	{
		for(auto pObs : mObservers) pObs->Update(arPoint, aIndex);
	}

	void Update(const Counter& arPoint, uint16_t aIndex) override final
	{
		for(auto pObs : mObservers) pObs->Update(arPoint, aIndex);
	}

	void Update(const FrozenCounter& arPoint, uint16_t aIndex) override final
	{
		for(auto pObs : mObservers) pObs->Update(arPoint, aIndex);
	}

	void Update(const BinaryOutputStatus& arPoint, uint16_t aIndex) override final
	{
		for(auto pObs : mObservers) pObs->Update(arPoint, aIndex);
	}
	void Update(const AnalogOutputStatus& arPoint, uint16_t aIndex) override final
	{
		for(auto pObs : mObservers) pObs->Update(arPoint, aIndex);
	}

private:
	std::vector<IMeasurementLoader*> mObservers;
};

}

#endif

