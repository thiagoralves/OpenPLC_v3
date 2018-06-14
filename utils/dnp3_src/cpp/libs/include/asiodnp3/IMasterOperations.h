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
#ifndef ASIODNP3_IMASTEROPERATIONS_H
#define ASIODNP3_IMASTEROPERATIONS_H

#include <opendnp3/app/ClassField.h>
#include <opendnp3/app/MeasurementTypes.h>
#include <opendnp3/StackStatistics.h>

#include <opendnp3/master/HeaderTypes.h>
#include <opendnp3/master/TaskConfig.h>
#include <opendnp3/master/ICommandProcessor.h>
#include <opendnp3/master/RestartOperationResult.h>

#include <opendnp3/gen/FunctionCode.h>
#include <opendnp3/gen/RestartType.h>

#include <openpal/executor/TimeDuration.h>
#include <openpal/logging/LogFilters.h>

#include <asiodnp3/IMasterScan.h>

#include <vector>
#include <memory>

namespace asiodnp3
{

/**
* All the operations that the user can perform on a running master
*/
class IMasterOperations : public opendnp3::ICommandProcessor
{
public:

	virtual ~IMasterOperations() {}

	/**
	*  @param filters Adjust the filters to this value
	*/
	virtual void SetLogFilters(const openpal::LogFilters& filters) = 0;

	/**
	* Add a recurring user-defined scan from a vector of headers
	* @ return A proxy class used to manipulate the scan
	*/
	virtual std::shared_ptr<IMasterScan> AddScan(openpal::TimeDuration period, const std::vector<opendnp3::Header>& headers, const opendnp3::TaskConfig& config = opendnp3::TaskConfig::Default()) = 0;

	/**
	* Add a scan that requests all objects using qualifier code 0x06
	* @ return A proxy class used to manipulate the scan
	*/
	virtual std::shared_ptr<IMasterScan> AddAllObjectsScan(opendnp3::GroupVariationID gvId, openpal::TimeDuration period, const opendnp3::TaskConfig& config = opendnp3::TaskConfig::Default()) = 0;

	/**
	* Add a class-based scan to the master
	* @return A proxy class used to manipulate the scan
	*/
	virtual std::shared_ptr<IMasterScan> AddClassScan(const opendnp3::ClassField& field, openpal::TimeDuration period, const opendnp3::TaskConfig& config = opendnp3::TaskConfig::Default()) = 0;

	/**
	* Add a start/stop (range) scan to the master
	* @return A proxy class used to manipulate the scan
	*/
	virtual std::shared_ptr<IMasterScan> AddRangeScan(opendnp3::GroupVariationID gvId, uint16_t start, uint16_t stop, openpal::TimeDuration period, const opendnp3::TaskConfig& config = opendnp3::TaskConfig::Default()) = 0;

	/**
	* Initiate a single user defined scan via a vector of headers
	*/
	virtual void Scan(const std::vector<opendnp3::Header>& headers, const opendnp3::TaskConfig& config = opendnp3::TaskConfig::Default()) = 0;

	/**
	* Initiate a single scan that requests all objects (0x06 qualifier code) for a certain group and variation
	*/
	virtual void ScanAllObjects(opendnp3::GroupVariationID gvId, const opendnp3::TaskConfig& config = opendnp3::TaskConfig::Default()) = 0;

	/**
	* Initiate a single class-based scan
	*/
	virtual void ScanClasses(const opendnp3::ClassField& field, const opendnp3::TaskConfig& config = opendnp3::TaskConfig::Default()) = 0;
	/**
	* Initiate a single start/stop (range) scan
	*/
	virtual void ScanRange(opendnp3::GroupVariationID gvId, uint16_t start, uint16_t stop, const opendnp3::TaskConfig& config = opendnp3::TaskConfig::Default()) = 0;

	/**
	* Write a time and interval object to a specific index
	*/
	virtual void Write(const opendnp3::TimeAndInterval& value, uint16_t index, const opendnp3::TaskConfig& config = opendnp3::TaskConfig::Default()) = 0;

	/**
	* Perform a cold or warm restart and get back the time-to-complete value
	*/
	virtual void Restart(opendnp3::RestartType op, const opendnp3::RestartOperationCallbackT& callback, opendnp3::TaskConfig config = opendnp3::TaskConfig::Default()) = 0;

	/**
	* Perform any operation that requires just a function code
	*/
	virtual void PerformFunction(const std::string& name, opendnp3::FunctionCode func, const std::vector<opendnp3::Header>& headers, const opendnp3::TaskConfig& config = opendnp3::TaskConfig::Default()) = 0;

};

}

#endif

