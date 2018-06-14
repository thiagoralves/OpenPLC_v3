
#ifndef CHANNEL_LISTENER_ADAPTER_H
#define CHANNEL_LISTENER_ADAPTER_H

#include <vcclr.h>

#include <asiodnp3/IChannelListener.h>

using namespace Automatak::DNP3::Interface;

namespace Automatak
{
	namespace DNP3
	{
		namespace Adapter
		{
						
			private class ChannelListenerAdapter final : public asiodnp3::IChannelListener
			{
			public:

				ChannelListenerAdapter(Automatak::DNP3::Interface::IChannelListener^ proxy) : proxy(proxy) {}	

				virtual void OnStateChange(opendnp3::ChannelState state) override
				{
					this->proxy->OnStateChange((Interface::ChannelState) state);
				}

			private:				

				gcroot < Automatak::DNP3::Interface::IChannelListener^ > proxy;
			};


		}
	}
}

#endif
