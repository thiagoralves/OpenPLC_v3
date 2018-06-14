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
#ifndef OPENDNP3_IMASTERAPPLICATION_H
#define OPENDNP3_IMASTERAPPLICATION_H

#include <openpal/executor/IUTCTimeSource.h>

#include "opendnp3/master/TaskInfo.h"
#include "opendnp3/master/HeaderTypes.h"

#include "opendnp3/app/IINField.h"
#include "opendnp3/link/ILinkListener.h"

#include "opendnp3/gen/MasterTaskType.h"
#include "opendnp3/gen/TaskCompletion.h"

#include <functional>

namespace opendnp3
{

typedef std::function<void(const Header&)> WriteHeaderFunT;

/**
* Interface for all master application callback info except for measurement values.
*/
class IMasterApplication : public ILinkListener, public openpal::IUTCTimeSource
{
public:

	virtual ~IMasterApplication() {}

	/// Called when a response or unsolicited response is receive from the outstation
	virtual void OnReceiveIIN(const IINField& iin) {}

	/// Task start notification
	virtual void OnTaskStart(MasterTaskType type, TaskId id) {}

	/// Task completion notification
	virtual void OnTaskComplete(const TaskInfo& info) {}

	/// Called when the application layer is opened
	virtual void OnOpen() {}

	/// Called when the application layer is closed
	virtual void OnClose() {}

	/// @return true if the master should do an assign class task during startup handshaking
	virtual bool AssignClassDuringStartup()
	{
		return false;
	}

	/// Configure the request headers for assign class. Only called if
	/// "AssignClassDuringStartup" returns true
	/// The user only needs to call the function for each header type to be written
	virtual void ConfigureAssignClassRequest(const WriteHeaderFunT& fun) {}

};

}

#endif
