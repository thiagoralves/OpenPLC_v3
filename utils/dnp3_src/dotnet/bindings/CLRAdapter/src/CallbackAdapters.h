
#ifndef OPENDNP3CLR_CALLBACK_ADAPTER_H
#define OPENDNP3CLR_CALLBACK_ADAPTER_H

using namespace System::Collections::ObjectModel;
using namespace System::Threading::Tasks;

#include <openpal/util/Uncopyable.h>

#include <opendnp3/master/CommandCallbackT.h>
#include <opendnp3/master/RestartOperationResult.h>

using namespace Automatak::DNP3::Interface;

namespace Automatak
{
	namespace DNP3
	{
		namespace Adapter
		{						
			class CallbackAdapters : private openpal::StaticOnly
			{

				public:
													
				static opendnp3::CommandCallbackT Get(TaskCompletionSource<CommandTaskResult^>^ tcs);

				static opendnp3::RestartOperationCallbackT Get(TaskCompletionSource<RestartResultType^>^ tcs);
				
			};			

		}
	}
}

#endif
