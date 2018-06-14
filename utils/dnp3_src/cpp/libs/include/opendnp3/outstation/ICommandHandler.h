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
#ifndef OPENDNP3_ICOMMANDHANDLER_H
#define OPENDNP3_ICOMMANDHANDLER_H

#include "opendnp3/app/ControlRelayOutputBlock.h"
#include "opendnp3/app/AnalogOutput.h"
#include "opendnp3/app/ITransactable.h"
#include "opendnp3/gen/OperateType.h"

namespace opendnp3
{

/**
* Interface used to dispatch SELECT / OPERATE / DIRECT OPERATE (Binary/Analog output) from the outstation to application code.
*
* The ITransactable sub-interface is used to determine the start and end of an ASDU containing commands.
*/
class ICommandHandler : public ITransactable
{
public:
	virtual ~ICommandHandler() {}

	/**
	* Ask if the application supports a ControlRelayOutputBlock - group 12 variation 1
	*
	* @param command command to operate
	* @param index index of the command
	* @return result of request
	*/
	virtual CommandStatus Select(const ControlRelayOutputBlock& command, uint16_t index) = 0;

	/**
	* Operate a ControlRelayOutputBlock - group 12 variation 1
	*
	* @param command command to operate
	* @param index index of the command
	* @param opType the operation type the outstation received.
	* @return result of request
	*/
	virtual CommandStatus Operate(const ControlRelayOutputBlock& command, uint16_t index, OperateType opType) = 0;


	/**
	* Ask if the application supports a 16 bit analog output - group 41 variation 2
	*
	* @param command command to operate
	* @param index index of the command
	* @return result of request
	*/
	virtual CommandStatus Select(const AnalogOutputInt16& command, uint16_t index) = 0;

	/**
	* Ask if the application supports a 16 bit analog output - group 41 variation 2
	*
	* @param command command to operate
	* @param index index of the command
	* @param opType the operation type the outstation received.
	* @return result of request
	*/
	virtual CommandStatus Operate(const AnalogOutputInt16& command, uint16_t index, OperateType opType) = 0;


	/**
	* Ask if the application supports a 32 bit analog output - group 41 variation 1
	*
	* @param command command to operate
	* @param index index of the command
	* @return result of request
	*/
	virtual CommandStatus Select(const AnalogOutputInt32& command, uint16_t index) = 0;

	/**
	* Operate a 32 bit analog output - group 41 variation 1
	*
	* @param command command to operate
	* @param index index of the command
	* @param opType the operation type the outstation received.
	* @return result of request
	*/
	virtual CommandStatus Operate(const AnalogOutputInt32& command, uint16_t index, OperateType opType) = 0;

	/**
	* Ask if the application supports a single precision, floating point analog output - group 41 variation 3
	*
	* @param command command to operate
	* @param index index of the command
	* @return result of request
	*/
	virtual CommandStatus Select(const AnalogOutputFloat32& command, uint16_t index) = 0;

	/**
	* Operate a single precision, floating point analog output - group 41 variation 3
	*
	* @param command command to operate
	* @param index index of the command
	* @param opType the operation type the outstation received.
	* @return result of request
	*/
	virtual CommandStatus Operate(const AnalogOutputFloat32& command, uint16_t index, OperateType opType) = 0;

	/**
	* Ask if the application supports a double precision, floating point analog output - group 41 variation 4
	*
	* @param command command to operate
	* @param index index of the command
	* @return result of request
	*/
	virtual CommandStatus Select(const AnalogOutputDouble64& command, uint16_t index) = 0;

	/**
	* Operate a double precision, floating point analog output - group 41 variation 4
	*
	* @param command command to operate
	* @param index index of the command
	* @param opType the operation type the outstation received.
	* @return result of request
	*/
	virtual CommandStatus Operate(const AnalogOutputDouble64& command, uint16_t index, OperateType opType) = 0;

};

}

#endif

