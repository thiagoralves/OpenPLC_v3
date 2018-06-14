#ifndef DNP3CLR_LOG_ADAPTER_H
#define DNP3CLR_LOG_ADAPTER_H

using namespace System::Collections::ObjectModel;

#include <openpal/logging/ILogHandler.h>
#include <memory>

#include <vcclr.h>
#include <memory>

using namespace Automatak::DNP3::Interface;

namespace Automatak
{
	namespace DNP3
	{
		namespace Adapter
		{

			private class LogAdapter final : public openpal::ILogHandler
			{
			public:

				LogAdapter(Automatak::DNP3::Interface::ILogHandler^ proxy);

				static std::shared_ptr<openpal::ILogHandler> Create(Automatak::DNP3::Interface::ILogHandler^ proxy);

				// logging error messages, etc
				virtual void Log(const openpal::LogEntry& Entry) override;

			private:
				gcroot < Automatak::DNP3::Interface::ILogHandler^ > proxy;
			};

		}
	}
}

#endif
