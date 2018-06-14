#ifndef CLR_DNP3_LISTENER_ADAPTER_H
#define	CLR_DNP3_LISTENER_ADAPTER_H

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Threading::Tasks;

using namespace Automatak::DNP3::Interface;

#include <asiopal/IListener.h>

namespace Automatak
{
	namespace DNP3
	{
		namespace Adapter
		{
			private ref class ListenerAdapter sealed : Interface::IListener
			{
			public:

				ListenerAdapter(const std::shared_ptr<asiopal::IListener>& proxy) : proxy(new std::shared_ptr<asiopal::IListener>(proxy))
				{}

				ListenerAdapter::~ListenerAdapter()
				{
					this->!ListenerAdapter();
				}

				ListenerAdapter::!ListenerAdapter()
				{
					delete proxy;
				}

				virtual void BeginShutdown()
				{
					(*proxy)->Shutdown();
				}

			private:
				
				const std::shared_ptr<asiopal::IListener>* proxy;
			};

		}
	}
}

#endif
