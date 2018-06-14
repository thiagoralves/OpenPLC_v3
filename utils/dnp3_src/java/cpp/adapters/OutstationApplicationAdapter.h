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
#ifndef OPENDNP3_OUTSTATIONAPPLICATIONADAPTER_H
#define OPENDNP3_OUTSTATIONAPPLICATIONADAPTER_H

#include <opendnp3/outstation/IOutstationApplication.h>

#include "GlobalRef.h"

class OutstationApplicationAdapter : public opendnp3::IOutstationApplication
{
public:

	OutstationApplicationAdapter(jobject proxy) : proxy(proxy) {}

	virtual bool SupportsWriteAbsoluteTime() override;

	virtual bool WriteAbsoluteTime(const openpal::UTCTimestamp& timestamp) override;

	virtual bool SupportsAssignClass() override;

	virtual void RecordClassAssignment(opendnp3::AssignClassType type, opendnp3::PointClass clazz, uint16_t start, uint16_t stop) override;

	virtual opendnp3::ApplicationIIN GetApplicationIIN() const override;

private:

	GlobalRef proxy;
};

#endif
