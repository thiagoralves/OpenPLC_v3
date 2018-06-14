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
#ifndef ASIODNP3_IOUTSTATION_H
#define ASIODNP3_IOUTSTATION_H

#include "asiodnp3/IStack.h"
#include "asiodnp3/Updates.h"

#include <openpal/logging/LogFilters.h>

namespace asiodnp3
{

/**
* Interface representing a running outstation.
*/
class IOutstation : public IStack
{
	friend class MeasUpdate;

public:

	virtual ~IOutstation() {}

	/**
	*  @param filters Adjust the filters to this value
	*/
	virtual void SetLogFilters(const openpal::LogFilters& filters) = 0;

	/**
	* Sets the restart IIN bit. Normally applications should not
	* touch this bit, but it is provided for simulating restarts.
	*/
	virtual void SetRestartIIN() = 0;

	/**
	* Apply a set of measurement updates to the outstation
	*/
	virtual void Apply(const Updates& updates) = 0;

};

}

#endif
