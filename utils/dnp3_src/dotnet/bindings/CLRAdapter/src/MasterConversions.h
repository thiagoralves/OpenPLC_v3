#ifndef __MASTER_CONVERSIONS_H_
#define __MASTER_CONVERSIONS_H_

using namespace System::Collections::ObjectModel;
using namespace System::Collections::Generic;

#include "TaskCallbackAdapter.h"
#include "ICommandHeaderWorkAround.h"

#include <vector>

#include <opendnp3/master/HeaderTypes.h>
#include <opendnp3/master/TaskConfig.h>
#include <opendnp3/master/TaskInfo.h>
#include <opendnp3/master/CommandSet.h>

using namespace Automatak::DNP3::Interface;

namespace Automatak
{
	namespace DNP3
	{
		namespace Adapter
		{

			private class MasterConversions
			{
			public:				

				static std::vector<opendnp3::Header> ConvertToVectorOfHeaders(IEnumerable<Header^>^ headers);

				static bool Convert(Header^ header, opendnp3::Header& output);

				static opendnp3::Header Convert(Header^ header);
				static opendnp3::Header ConvertCount8(CountHeader^ header);
				static opendnp3::Header ConvertCount16(CountHeader^ header);
				static opendnp3::Header ConvertRange8(RangeHeader^ header);
				static opendnp3::Header ConvertRange16(RangeHeader^ header);

				static opendnp3::TaskConfig Convert(TaskConfig^ config, ITaskCallback^ wrapper);
				static opendnp3::TaskConfig Convert(TaskConfig^ config);				

				static opendnp3::CommandSet Convert(ICommandHeaders^ headers);
								
				static opendnp3::TaskId Convert(TaskId^ taskId);
				static TaskId^ Convert(const opendnp3::TaskId& id);

				static TaskInfo^ Convert(const opendnp3::TaskInfo& info);

				static opendnp3::ITaskCallback* CreateTaskCallback(ITaskCallback^ callback);
			
			};

		}
	}
}
#endif
