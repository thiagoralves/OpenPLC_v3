/**
 * Copyright 2013 Automatak, LLC
 *
 * Licensed to Automatak, LLC (www.automatak.com) under one or more
 * contributor license agreements. See the NOTICE file distributed with this
 * work for additional information regarding copyright ownership. Automatak, LLC
 * licenses this file to you under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. You may obtain
 * a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#ifndef OPENDNP3_MASTERAPPLICATIONADAPTER_H
#define OPENDNP3_MASTERAPPLICATIONADAPTER_H

#include <opendnp3/master/IMasterApplication.h>

#include "opendnp3/gen/PointClass.h"

#include "GlobalRef.h"

class MasterApplicationAdapter : public opendnp3::IMasterApplication
{
public:

	MasterApplicationAdapter(jobject proxy) : proxy(proxy) {}

	virtual openpal::UTCTimestamp Now() override;

	virtual void OnReceiveIIN(const opendnp3::IINField& iin) override;
	virtual void OnTaskStart(opendnp3::MasterTaskType type, opendnp3::TaskId id) override;
	virtual void OnTaskComplete(const opendnp3::TaskInfo& info) override;
	virtual void OnOpen() override;
	virtual void OnClose() override;
	virtual bool AssignClassDuringStartup() override;
	virtual void ConfigureAssignClassRequest(const opendnp3::WriteHeaderFunT& fun)  override;

private:

	GlobalRef proxy;
};

#endif
