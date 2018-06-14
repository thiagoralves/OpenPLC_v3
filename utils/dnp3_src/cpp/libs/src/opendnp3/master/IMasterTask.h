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
#ifndef OPENDNP3_IMASTERTASK_H
#define OPENDNP3_IMASTERTASK_H

#include <openpal/logging/Logger.h>
#include <openpal/executor/MonotonicTimestamp.h>
#include <openpal/executor/IExecutor.h>

#include "opendnp3/app/APDUHeader.h"
#include "opendnp3/app/APDURequest.h"

#include "opendnp3/master/TaskConfig.h"
#include "opendnp3/master/IMasterApplication.h"

namespace opendnp3
{

/**
 * A generic interface for defining master request/response style tasks
 */
class IMasterTask
{

protected:

	class TaskState
	{
	public:

		TaskState(openpal::MonotonicTimestamp expiration_, bool disabled_ = false) : disabled(disabled_), expiration(expiration_)
		{}

		static TaskState Immediately()
		{
			return TaskState(openpal::MonotonicTimestamp(0), false);
		}

		static TaskState Infinite()
		{
			return TaskState(openpal::MonotonicTimestamp::Max(), false);
		}

		static TaskState Retry(openpal::MonotonicTimestamp exp)
		{
			return TaskState(exp, false);
		}

		static TaskState Disabled()
		{
			return TaskState(openpal::MonotonicTimestamp::Max(), true);
		}

		bool disabled;
		openpal::MonotonicTimestamp expiration;

	private:
		TaskState() = delete;
	};


public:

	enum class ResponseResult : uint8_t
	{
		/// The response was bad, the task has failed
		ERROR_BAD_RESPONSE,

		/// An internal error occured like a failure calculating an HMAC
		ERROR_INTERNAL_FAILURE,

		/// The response was good and the task is complete
		OK_FINAL,

		/// The response was good and the task should repeat the format, transmit, and await response sequence
		OK_REPEAT,

		/// The response was good and the task should continue executing. Restart the response timer, and increment expected SEQ#.
		OK_CONTINUE
	};


	IMasterTask(IMasterApplication& app, openpal::MonotonicTimestamp expiration, openpal::Logger logger, TaskConfig config);


	virtual ~IMasterTask();

	/**
	*	Overridable for auth tasks
	*/
	virtual bool IsAuthTask() const
	{
		return false;
	}

	/**
	*
	* @return	the name of the task
	*/
	virtual char const* Name() const = 0;

	/**
	* The task's priority. Lower numbers are higher priority.
	*/
	virtual int Priority() const = 0;

	/**
	* Allows tasks to enter a blocking mode where lower priority
	* tasks cannot run until this task completes
	*/
	virtual bool BlocksLowerPriority() const = 0;

	/**
	* Indicates if the task should be rescheduled (true) or discarded
	* after a single execution (false)
	*/
	virtual bool IsRecurring() const = 0;

	/**
	* The time when this task can run again.
	*/
	openpal::MonotonicTimestamp ExpirationTime() const;

	/**
	* Configure the start expiration time
	*/
	void ConfigureStartExpiration(openpal::MonotonicTimestamp time);

	/**
	* The time when this task expires if it is unable to start
	*/
	openpal::MonotonicTimestamp StartExpirationTime() const;

	/**
	 * Build a request APDU.
	 *
	 * Return false if some kind of internal error prevents the task for formatting the request.
	 */
	virtual bool BuildRequest(APDURequest& request, uint8_t seq) = 0;

	/**
	 * Handler for responses
	 */
	ResponseResult OnResponse(const APDUResponseHeader& response, const openpal::RSlice& objects, openpal::MonotonicTimestamp now);

	/**
	 * Called when a response times out
	 */
	void OnResponseTimeout(openpal::MonotonicTimestamp now);

	/**
	* Called when the layer closes while the task is executing.
	*/
	void OnLowerLayerClose(openpal::MonotonicTimestamp now);

	/**
	* The start timeout expired before the task could be run
	*/
	void OnStartTimeout(openpal::MonotonicTimestamp now);

	/**
	* Called when a task is discared because it's user doesn't exist
	*/
	void OnNoUser(openpal::MonotonicTimestamp now);

	/**
	* Called when a task has an internal error of some sort like not being able to write a request
	*/
	void OnInternalError(openpal::MonotonicTimestamp now);

	/**
	* Called when the request is reject due to an authentication failure
	*/
	void OnAuthenticationFailure(openpal::MonotonicTimestamp now);

	/**
	* Called when the request is reject due to an authorization failure
	*/
	void OnAuthorizationFailure(openpal::MonotonicTimestamp now);

	/**
	* Called when the task first starts, before the first request is formatted
	*/
	void OnStart();

	/**
	* Demand that the task run immediately by setting the expiration to 0
	*/
	void Demand();

protected:

	// called during OnStart() to initialize any state for a new run
	virtual void Initialize() {}

	virtual ResponseResult ProcessResponse(const APDUResponseHeader& response, const openpal::RSlice& objects) = 0;

	virtual TaskState OnTaskComplete(TaskCompletion completion, openpal::MonotonicTimestamp now) = 0;

	virtual bool IsEnabled() const = 0;

	virtual MasterTaskType GetTaskType() const = 0;

	IMasterApplication* pApplication;
	openpal::Logger logger;

	// Validation helpers for various behaviors to avoid deep inheritance
	bool ValidateSingleResponse(const APDUResponseHeader& header);
	bool ValidateNullResponse(const APDUResponseHeader& header, const openpal::RSlice& objects);
	bool ValidateNoObjects(const openpal::RSlice& objects);
	bool ValidateInternalIndications(const APDUResponseHeader& header);

	void NotifyResult(TaskCompletion result);

private:

	IMasterTask();

	TaskState state;
	TaskConfig config;
	openpal::MonotonicTimestamp taskStartExpiration;
};

}

#endif
