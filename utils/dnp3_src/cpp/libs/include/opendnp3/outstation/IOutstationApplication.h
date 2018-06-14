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
#ifndef OPENDNP3_IOUTSTATIONAPPLICATION_H
#define OPENDNP3_IOUTSTATIONAPPLICATION_H

#include <openpal/executor/UTCTimestamp.h>

#include "opendnp3/gen/PointClass.h"
#include "opendnp3/gen/RestartMode.h"
#include "opendnp3/gen/AssignClassType.h"

#include "opendnp3/link/ILinkListener.h"

#include "opendnp3/app/Indexed.h"
#include "opendnp3/app/MeasurementTypes.h"
#include "opendnp3/app/parsing/ICollection.h"

#include "opendnp3/outstation/ApplicationIIN.h"

#include <memory>

namespace opendnp3
{

/**
* Interface for all outstation application callback info except for control requests
*/
class IOutstationApplication : public ILinkListener
{
public:


	/// Queries whether the the outstation supports absolute time writes
	/// If this function returns false, WriteAbsoluteTime will never be called
	/// and the outstation will return IIN 2.1 (FUNC_NOT_SUPPORTED)
	virtual bool SupportsWriteAbsoluteTime()
	{
		return false;
	}

	/// Write the time to outstation, only called if SupportsWriteAbsoluteTime return true
	/// @return boolean value indicating if the time value supplied was accepted. Returning
	/// false will cause the outstation to set IIN 2.3 (PARAM_ERROR) in its response.
	/// The outstation should clear its NEED_TIME field when handling this response
	virtual bool WriteAbsoluteTime(const openpal::UTCTimestamp& timestamp)
	{
		return false;
	}

	/// Queries whether the outstation supports the writing of TimeAndInterval
	/// If this function returns false, WriteTimeAndInterval will never be called
	/// and the outstation will return IIN 2.1 (FUNC_NOT_SUPPORTED) when it receives this request
	virtual bool SupportsWriteTimeAndInterval()
	{
		return false;
	}

	/// Write one or more TimeAndInterval values. Only called if SupportsWriteTimeAndInterval returns true.
	/// The outstation application code is reponsible for updating TimeAndInterval values in the database if this behavior
	/// is desired
	/// @return boolean value indicating if the values supplied were accepted. Returning
	/// false will cause the outstation to set IIN 2.3 (PARAM_ERROR) in its response.
	virtual bool WriteTimeAndInterval(const ICollection<Indexed<TimeAndInterval>>& values)
	{
		return false;
	}

	/// True if the outstation supports the assign class function code
	/// If this function returns false, the assign class callbacks will never be called
	/// and the outstation will return IIN 2.1 (FUNC_NOT_SUPPORTED) when it receives this function code
	virtual bool SupportsAssignClass()
	{
		return false;
	}

	/// Called if SupportsAssignClass returns true
	/// The type and range are pre-validated against the outstation's database
	/// and class assignments are automatically applied internally.
	/// This callback allows user code to persist the changes to non-volatile memory
	virtual void RecordClassAssignment(AssignClassType type, PointClass clazz, uint16_t start, uint16_t stop) {}

	/// Returns the application-controlled IIN field
	virtual ApplicationIIN GetApplicationIIN() const
	{
		return ApplicationIIN();
	}

	/// Query the outstation for the cold restart mode it supports
	virtual RestartMode ColdRestartSupport() const
	{
		return RestartMode::UNSUPPORTED;
	}

	/// Query the outstation for the warm restart mode it supports
	virtual RestartMode WarmRestartSupport() const
	{
		return RestartMode::UNSUPPORTED;
	}

	/// The outstation should perform a complete restart.
	/// See the DNP3 specification for a complete descripton of normal behavior
	/// @return number of seconds or milliseconds until restart is complete. The value
	/// is interpreted based on the Restart Mode returned from ColdRestartSupport()
	virtual uint16_t ColdRestart()
	{
		return 65535;
	}

	/// The outstation should perform a partial restart of only the DNP3 application.
	/// See the DNP3 specification for a complete descripton of normal behavior
	/// @return number of seconds or milliseconds until restart is complete. The value
	/// is interpreted based on the Restart Mode returned from WarmRestartSupport()
	virtual uint16_t WarmRestart()
	{
		return 65535;
	}


	virtual ~IOutstationApplication() {}
};

/**
* A singleton with default setting useful for examples
*/
class DefaultOutstationApplication : public IOutstationApplication
{
public:

	static std::shared_ptr<IOutstationApplication> Create()
	{
		return std::make_shared<DefaultOutstationApplication>();
	}

	DefaultOutstationApplication() = default;

};

}

#endif
