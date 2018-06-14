

#include "CallbackAdapters.h"

#include "Conversions.h"

#include <vcclr.h>

namespace Automatak
{
	namespace DNP3
	{
		namespace Adapter
		{						
																
			opendnp3::CommandCallbackT CallbackAdapters::Get(TaskCompletionSource<CommandTaskResult^>^ tcs)
			{
				gcroot<TaskCompletionSource<CommandTaskResult^>^> handle(tcs);

				return [handle](const opendnp3::ICommandTaskResult& res) -> void
				{
					auto result = Conversions::ConvertCommandTaskResult(res);
					handle->SetResult(result);
				};
			}

			opendnp3::RestartOperationCallbackT CallbackAdapters::Get(TaskCompletionSource<RestartResultType^>^ tcs)
			{
				gcroot<TaskCompletionSource<RestartResultType^>^> handle(tcs);

				return [handle](const opendnp3::RestartOperationResult& res) -> void
				{
					auto result = gcnew RestartResultType((TaskCompletion)res.summary, Conversions::ConvertTimeDuration(res.restartTime));

					handle->SetResult(result);
				};
			}				

		}
	}
}


