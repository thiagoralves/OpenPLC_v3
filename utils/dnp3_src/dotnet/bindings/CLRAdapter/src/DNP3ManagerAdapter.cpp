
#include "DNP3ManagerAdapter.h"

#include "Conversions.h"
#include "LogAdapter.h"
#include "ChannelAdapter.h"
#include "ChannelListenerAdapter.h"
#include "ListenCallbacksAdapter.h"
#include "ListenerAdapter.h"

#include <asiodnp3/DNP3Manager.h>

using namespace asiopal;

namespace Automatak
{
	namespace DNP3
	{
		namespace Adapter
		{

			IDNP3Manager^ DNP3ManagerFactory::CreateManager(System::Int32 concurrency, ILogHandler^ logHandler)
			{
				return gcnew DNP3ManagerAdapter(concurrency, logHandler);
			}

			IDNP3Manager^ DNP3ManagerFactory::CreateManager(ILogHandler^ logHandler)
			{
				return gcnew DNP3ManagerAdapter(Environment::ProcessorCount, logHandler);
			}


			DNP3ManagerAdapter::DNP3ManagerAdapter(System::Int32 concurrency, ILogHandler^ logHandler) :
				manager(new asiodnp3::DNP3Manager(concurrency, LogAdapter::Create(logHandler)))				
			{

			}

			DNP3ManagerAdapter::~DNP3ManagerAdapter()
			{
				delete manager;				
			}

			void DNP3ManagerAdapter::Shutdown()
			{
				manager->Shutdown();
			}			

			IChannel^ DNP3ManagerAdapter::AddTCPClient(System::String^ id, System::UInt32 filters, Interface::ChannelRetry^ retry, System::String^ address, System::UInt16 port, Automatak::DNP3::Interface::IChannelListener^ listener)
			{
				std::string stdName = Conversions::ConvertString(id);
				std::string stdAddress = Conversions::ConvertString(address);
								
				auto listenAdapter = std::shared_ptr<asiodnp3::IChannelListener>(new ChannelListenerAdapter(listener));

				auto channel = this->manager->AddTCPClient(stdName.c_str(), filters, Convert(retry), stdAddress, "", port, listenAdapter);

				return channel ? gcnew ChannelAdapter(channel) : nullptr;
			}

			IChannel^ DNP3ManagerAdapter::AddTCPServer(System::String^ id, System::UInt32 filters, Interface::ChannelRetry^ retry, System::String^ endpoint, System::UInt16 port, Automatak::DNP3::Interface::IChannelListener^ listener)
			{
				std::string stdName = Conversions::ConvertString(id);
				std::string stdEndpoint = Conversions::ConvertString(endpoint);
								
				auto listenAdapter = std::shared_ptr<asiodnp3::IChannelListener>(new ChannelListenerAdapter(listener));

				auto channel = this->manager->AddTCPServer(stdName.c_str(), filters, Convert(retry), stdEndpoint, port, listenAdapter);

				return channel ? gcnew ChannelAdapter(channel) : nullptr;
			}

			IChannel^ DNP3ManagerAdapter::AddTLSClient(System::String^ id, System::UInt32 filters, Interface::ChannelRetry^ retry, System::String^ address, System::UInt16 port, Automatak::DNP3::Interface::TLSConfig^ config, Automatak::DNP3::Interface::IChannelListener^ listener)
			{
				std::string stdName = Conversions::ConvertString(id);
				std::string stdAddress = Conversions::ConvertString(address);
								
				auto listenAdapter = std::shared_ptr<asiodnp3::IChannelListener>(new ChannelListenerAdapter(listener));
								
				std::error_code ec;
				auto channel = this->manager->AddTLSClient(stdName.c_str(), filters, Convert(retry), stdAddress, "", port, Conversions::Convert(config), listenAdapter, ec);
				if (ec)
				{
					throw gcnew System::Exception(Conversions::ConvertString(ec.message()));
				}
				else
				{
					return channel ? gcnew ChannelAdapter(channel) : nullptr;
				}								
			}
			
			IChannel^ DNP3ManagerAdapter::AddTLSServer(System::String^ id, System::UInt32 filters, Interface::ChannelRetry^ retry, System::String^ endpoint, System::UInt16 port, Automatak::DNP3::Interface::TLSConfig^ config, Automatak::DNP3::Interface::IChannelListener^ listener)
			{
				std::string stdName = Conversions::ConvertString(id);
				std::string stdEndpoint = Conversions::ConvertString(endpoint);
								
				auto listenAdapter = std::shared_ptr<asiodnp3::IChannelListener>(new ChannelListenerAdapter(listener));
				
				std::error_code ec;
				auto channel = this->manager->AddTLSServer(stdName.c_str(), filters, Convert(retry), stdEndpoint, port, Conversions::Convert(config), listenAdapter, ec);
				if (ec)
				{
					throw gcnew System::Exception(Conversions::ConvertString(ec.message()));
				}
				else
				{
					return channel ? gcnew ChannelAdapter(channel) : nullptr;
				}				
			}

			IChannel^ DNP3ManagerAdapter::AddSerial(System::String^ id, System::UInt32 filters, Interface::ChannelRetry^ retry, Automatak::DNP3::Interface::SerialSettings^ settings, Automatak::DNP3::Interface::IChannelListener^ listener)
			{
				std::string stdName = Conversions::ConvertString(id);
				auto s = Conversions::ConvertSerialSettings(settings);				

				auto listenAdapter = std::shared_ptr<asiodnp3::IChannelListener>(new ChannelListenerAdapter(listener));
				
				auto channel = this->manager->AddSerial(stdName.c_str(), filters, Convert(retry), s, listenAdapter);
				
				return channel ? gcnew ChannelAdapter(channel) : nullptr;				
			}

			Interface::IListener^ DNP3ManagerAdapter::CreateListener(System::String^ loggerid, System::UInt32 filters, Interface::IPEndpoint^ endpoint, IListenCallbacks^ callbacks)
			{
				auto id = Conversions::ConvertString(loggerid);
				auto levels = openpal::LogFilters(filters);
				auto ep = Conversions::Convert(endpoint);
				auto cb = std::shared_ptr<asiodnp3::IListenCallbacks>(new ListenCallbacksAdapter(callbacks));

				std::error_code ec;
				auto listener = manager->CreateListener(id, levels, ep, cb, ec);

				if (ec)
				{
					throw gcnew System::Exception("An error occured configuring your TCP listener. Check you log.");
				}

				return gcnew ListenerAdapter(listener);
			}

			Interface::IListener^ DNP3ManagerAdapter::CreateListener(System::String^ loggerid, System::UInt32 filters, Interface::IPEndpoint^ endpoint, Interface::TLSConfig^ config, IListenCallbacks^ callbacks)
			{
				auto id = Conversions::ConvertString(loggerid);
				auto levels = openpal::LogFilters(filters);
				auto ep = Conversions::Convert(endpoint);
				auto tlsConfig = Conversions::Convert(config);
				auto cb = std::shared_ptr<asiodnp3::IListenCallbacks>(new ListenCallbacksAdapter(callbacks));

				std::error_code ec;
				auto listener = manager->CreateListener(id, levels, ep, tlsConfig, cb, ec);

				if (ec)
				{
					throw gcnew System::Exception("An error occured configuring your TLS listener. Check you log.");
				}

				return gcnew ListenerAdapter(listener);
			}

			asiopal::ChannelRetry DNP3ManagerAdapter::Convert(Interface::ChannelRetry^ retry)
			{				
				return asiopal::ChannelRetry(Conversions::ConvertTimespan(retry->minRetryDelay), Conversions::ConvertTimespan(retry->maxRetryDelay));
			}			
		}
	}
}