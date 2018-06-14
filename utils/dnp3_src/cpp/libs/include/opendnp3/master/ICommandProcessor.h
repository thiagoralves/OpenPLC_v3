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
#ifndef OPENDNP3_ICOMMANDPROCESSOR_H
#define OPENDNP3_ICOMMANDPROCESSOR_H

#include "opendnp3/master/CommandSet.h"
#include "opendnp3/master/CommandCallbackT.h"
#include "opendnp3/master/TaskConfig.h"

namespace opendnp3
{

/**
* Interface used to dispatch SELECT / OPERATE / DIRECT OPERATE from application code to a master
*/
class ICommandProcessor
{
public:

	/**
	* Select and operate a set of commands
	*
	* @param commands Set of command headers
	* @param callback callback that will be invoked upon completion or failure
	* @param config optional configuration that controls normal callbacks and allows the user to be specified for SA
	*/
	virtual void SelectAndOperate(CommandSet&& commands, const CommandCallbackT& callback, const TaskConfig& config = TaskConfig::Default()) = 0;

	/**
	* Direct operate a set of commands
	*
	* @param commands Set of command headers
	* @param callback callback that will be invoked upon completion or failure
	* @param config optional configuration that controls normal callbacks and allows the user to be specified for SA
	*/
	virtual void DirectOperate(CommandSet&& commands, const CommandCallbackT& callback, const TaskConfig& config = TaskConfig::Default()) = 0;


	/**
	* Select/operate a single command
	*
	* @param command Command to operate
	* @param index of the command
	* @param callback callback that will be invoked upon completion or failure
	* @param config optional configuration that controls normal callbacks and allows the user to be specified for SA
	*/
	template <class T>
	void SelectAndOperate(const T& command, uint16_t index, const CommandCallbackT& callback, const TaskConfig& config = TaskConfig::Default());

	/**
	* Direct operate a single command
	*
	* @param command Command to operate
	* @param index of the command
	* @param callback callback that will be invoked upon completion or failure
	* @param config optional configuration that controls normal callbacks and allows the user to be specified for SA
	*/
	template <class T>
	void DirectOperate(const T& command, uint16_t index, const CommandCallbackT& callback, const TaskConfig& config = TaskConfig::Default());
};

template <class T>
void ICommandProcessor::SelectAndOperate(const T& command, uint16_t index, const CommandCallbackT& callback, const TaskConfig& config)
{
	CommandSet commands({ WithIndex(command, index) });
	this->SelectAndOperate(std::move(commands), callback, config);
}

template <class T>
void ICommandProcessor::DirectOperate(const T& command, uint16_t index, const CommandCallbackT& callback, const TaskConfig& config)
{
	CommandSet commands({ WithIndex(command, index) });
	this->DirectOperate(std::move(commands), callback, config);
}

}

#endif
