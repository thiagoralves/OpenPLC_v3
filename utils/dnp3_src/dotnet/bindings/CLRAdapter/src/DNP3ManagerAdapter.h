#ifndef __CLR_DNP3_MANAGER_ADAPTER_H_
#define __CLR_DNP3_MANAGER_ADAPTER_H_

using namespace System;
using namespace Automatak::DNP3::Interface;

#include <asiopal/ChannelRetry.h>
#include <asiopal/TLSConfig.h>

namespace asiodnp3
{
	class DNP3Manager;
}

namespace Automatak
{
	namespace DNP3
	{
		namespace Adapter
		{
			/// <summary>
			/// Factory class used to get the root DNP3 object
			/// </summary>
			public ref class DNP3ManagerFactory
			{
			public:
				/// <summary>
				/// Create a new IDNP3Manager using the specified concurrency
				/// </summary>
				/// <param name="concurrency">how many threads are allocated to the thread pool</param>
				
				/// <returns>IDNP3Manager interface</returns>
				static IDNP3Manager^ CreateManager(System::Int32 concurrency, ILogHandler^ logHandler);

				/// <summary>
				/// Create a new IDNP3Manager using the default concurrency
				/// </summary>
				/// <param name="logHandler">Callback interface for log messages</param>
				/// <returns>IDNP3Manager interface</returns>
				static IDNP3Manager^ CreateManager(ILogHandler^ logHandler);

			private:
				DNP3ManagerFactory() {}
			};


			ref class DNP3ManagerAdapter : public Automatak::DNP3::Interface::IDNP3Manager
			{
			public:

				DNP3ManagerAdapter(System::Int32 concurrency, ILogHandler^ logHandler);
				~DNP3ManagerAdapter();

				virtual void Shutdown() sealed;				

				virtual IChannel^ AddTCPClient(System::String^ id, System::UInt32 filters, Interface::ChannelRetry^ retry, System::String^ address, System::UInt16 port, Interface::IChannelListener^ listener)  sealed;
				virtual IChannel^ AddTCPServer(System::String^ id, System::UInt32 filters, Interface::ChannelRetry^ retry, System::String^ endpoint, System::UInt16 port, Interface::IChannelListener^ listener) sealed;

				virtual IChannel^ AddTLSClient(System::String^ id, System::UInt32 filters, Interface::ChannelRetry^ retry, System::String^ address, System::UInt16 port, Interface::TLSConfig^ config, Interface::IChannelListener^ listener)  sealed;
				virtual IChannel^ AddTLSServer(System::String^ id, System::UInt32 filters, Interface::ChannelRetry^ retry, System::String^ endpoint, System::UInt16 port, Interface::TLSConfig^ config, Interface::IChannelListener^ listener) sealed;
				
				virtual IChannel^ AddSerial(System::String^ id, System::UInt32 filters, Interface::ChannelRetry^ retry, Automatak::DNP3::Interface::SerialSettings^ settings, Interface::IChannelListener^ listener) sealed;

				virtual Interface::IListener^ CreateListener(System::String^ loggerid, System::UInt32 filters, Interface::IPEndpoint^ endpoint, IListenCallbacks^ callbacks) sealed;

				virtual Interface::IListener^ CreateListener(System::String^ loggerid, System::UInt32 filters, Interface::IPEndpoint^ endpoint, Interface::TLSConfig^ config, IListenCallbacks^ callbacks) sealed;

			private:			

				asiodnp3::DNP3Manager* manager;								

				static asiopal::ChannelRetry Convert(Interface::ChannelRetry^ retry);				
			};

		}
	}
}

#endif
