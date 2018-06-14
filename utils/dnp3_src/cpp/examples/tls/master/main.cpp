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
#include <asiodnp3/DNP3Manager.h>
#include <asiodnp3/PrintingSOEHandler.h>
#include <asiodnp3/PrintingChannelListener.h>
#include <asiodnp3/ConsoleLogger.h>
#include <asiodnp3/DefaultMasterApplication.h>
#include <asiodnp3/PrintingCommandCallback.h>

#include <asiopal/UTCTimeSource.h>

#include <opendnp3/LogLevels.h>

using namespace std;
using namespace openpal;
using namespace asiopal;
using namespace asiodnp3;
using namespace opendnp3;

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cout << "usage: master-tls-demo <peer certificate> <private key/certificate>" << std::endl;
		return -1;
	}

	std::string peerCertificate(argv[1]);
	std::string privateKey(argv[2]);

	std::cout << "Using peer cert: " << peerCertificate << std::endl;
	std::cout << "Using private key file: " << privateKey << std::endl;


	// Specify what log levels to use. NORMAL is warning and above
	// You can add all the comms logging by uncommenting below
	const uint32_t FILTERS = levels::NORMAL | levels::ALL_APP_COMMS;

	// This is the main point of interaction with the stack
	// send log messages to the console
	DNP3Manager manager(1, ConsoleLogger::Create());

	std::error_code ec;

	// Connect via a TCPClient socket to a outstation
	auto channel = manager.AddTLSClient(
	                   "tls-client",
	                   FILTERS,
	                   ChannelRetry::Default(),
	                   "127.0.0.1",
	                   "0.0.0.0",
	                   20001,
	                   TLSConfig(peerCertificate, privateKey, privateKey),
	                   PrintingChannelListener::Create(),
	                   ec
	               );

	if (ec)
	{
		std::cout << "Unable to create tls client: " << ec.message() << std::endl;
		return ec.value();
	}

	// The master config object for a master. The default are
	// useable, but understanding the options are important.
	MasterStackConfig stackConfig;

	// you can override application layer settings for the master here
	// in this example, we've change the application layer timeout to 2 seconds
	stackConfig.master.responseTimeout = TimeDuration::Seconds(2);
	stackConfig.master.disableUnsolOnStartup = true;

	// You can override the default link layer settings here
	// in this example we've changed the default link layer addressing
	stackConfig.link.LocalAddr = 1;
	stackConfig.link.RemoteAddr = 10;

	// Create a new master on a previously declared port, with a
	// name, log level, command acceptor, and config info. This
	// returns a thread-safe interface used for sending commands.
	auto master = channel->AddMaster(
	                  "master",										// id for logging
	                  PrintingSOEHandler::Create(),					// callback for data processing
	                  asiodnp3::DefaultMasterApplication::Create(),	// master application instance
	                  stackConfig										// stack configuration
	              );


	// do an integrity poll (Class 3/2/1/0) once per minute
	auto integrityScan = master->AddClassScan(ClassField::AllClasses(), TimeDuration::Minutes(1));

	// do a Class 1 exception poll every 5 seconds
	auto exceptionScan = master->AddClassScan(ClassField(ClassField::CLASS_1), TimeDuration::Seconds(2));

	// Enable the master. This will start communications.
	master->Enable();

	do
	{
		std::cout << "Enter a command" << std::endl;
		std::cout << "x - exits program" << std::endl;
		std::cout << "a - performs and ad-hoc range scan" << std::endl;
		std::cout << "i - integrity demand scan" << std::endl;
		std::cout << "e - exception demand scan" << std::endl;
		std::cout << "d - diable unsolcited" << std::endl;
		std::cout << "r - cold restart" << std::endl;
		std::cout << "c - send crob" << std::endl;

		char cmd;
		std::cin >> cmd;
		switch(cmd)
		{
		case('a') :
			master->ScanRange(GroupVariationID(1, 2), 0, 3);
			break;
		case('d') :
			master->PerformFunction("disable unsol", FunctionCode::DISABLE_UNSOLICITED,
			{ Header::AllObjects(60, 2), Header::AllObjects(60, 3), Header::AllObjects(60, 4) }
			                       );
			break;
		case('r') :
			{
				auto print = [](const RestartOperationResult & result)
				{
					if(result.summary == TaskCompletion::SUCCESS)
					{
						std::cout << "Success, Time: " << result.restartTime.GetMilliseconds() << std::endl;
					}
					else
					{
						std::cout << "Failure: " << TaskCompletionToString(result.summary) << std::endl;
					}
				};
				master->Restart(RestartType::COLD, print);
				break;
			}
		case('x'):
			// C++ destructor on DNP3Manager cleans everything up for you
			return 0;
		case('i'):
			integrityScan->Demand();
			break;
		case('e'):
			exceptionScan->Demand();
			break;
		case('c'):
			{
				ControlRelayOutputBlock crob(ControlCode::LATCH_ON);
				master->SelectAndOperate(crob, 0, PrintingCommandCallback::Get());
				break;
			}
		default:
			std::cout << "Unknown action: " << cmd << std::endl;
			break;
		}
	}
	while(true);

	return 0;
}
