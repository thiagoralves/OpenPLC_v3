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

#ifndef OPENPAL_MOCKLOGHANDLER_H
#define OPENPAL_MOCKLOGHANDLER_H

#include <openpal/logging/Logger.h>

#include <string>
#include <queue>
#include <mutex>

namespace testlib
{

class LogRecord
{
public:

	LogRecord() = default;
	LogRecord(const openpal::LogEntry& entry);

	std::string		id;
	openpal::LogFilters		filters = 0;
	std::string		location;
	std::string		message;
};

struct MockLogHandlerImpl : public openpal::ILogHandler
{
	virtual void Log(const openpal::LogEntry& entry) override;

	std::mutex mutex;
	bool outputToStdIO = false;
	std::deque<LogRecord> messages;
};

class MockLogHandler
{

public:

	MockLogHandler() :
		impl(std::make_shared<MockLogHandlerImpl>()),
		logger(impl, "test", ~0)
	{}

	void WriteToStdIo();

	void Log(const std::string& location, const std::string& message);

	void ClearLog();

	bool GetNextEntry(LogRecord& record);

private:

	std::shared_ptr<MockLogHandlerImpl> impl;

public:

	openpal::Logger logger;

};






}

#endif
