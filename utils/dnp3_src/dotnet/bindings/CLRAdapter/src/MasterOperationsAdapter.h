#ifndef OPENDNP3CLR_MASTER_OPERATIONS_ADAPTER_H
#define OPENDNP3CLR_MASTER_OPERATIONS_ADAPTER_H

using namespace System::Collections::ObjectModel;
using namespace System::Collections::Generic;

#include <asiodnp3/IMasterOperations.h>

#include "CallbackAdapters.h"
#include "MasterConversions.h"
#include "MasterScanAdapter.h"

using namespace Automatak::DNP3::Interface;

namespace Automatak
{
	namespace DNP3
	{
		namespace Adapter
		{			
			private ref class MasterOperationsAdapter abstract
			{
			public:

				MasterOperationsAdapter(asiodnp3::IMasterOperations* operations);						

				virtual void SetLogFilters(LogFilter flags);				
			
				virtual Task<TaskCompletion>^ Scan(IEnumerable<Header^>^ headers, TaskConfig^ config);

				virtual Task<TaskCompletion>^ ScanAllObjects(System::Byte group, System::Byte variation, TaskConfig^ config);

				virtual Task<TaskCompletion>^ ScanClasses(ClassField field, TaskConfig^ config);
				
				virtual Task<TaskCompletion>^ ScanRange(System::Byte group, System::Byte variation, System::UInt16 start, System::UInt16 stop, TaskConfig^ config);

				virtual Task<TaskCompletion>^ Write(TimeAndInterval^ value, System::UInt16 index, TaskConfig^ config);

				virtual Task<RestartResultType^>^ Restart(RestartType restartType, TaskConfig^ config);

				virtual Task<TaskCompletion>^ PerformFunction(System::String^ name, FunctionCode func, IEnumerable<Header^>^ headers, TaskConfig^ config);

				virtual IMasterScan^ AddScan(IEnumerable<Header^>^ headers, System::TimeSpan period, TaskConfig^ config);

				virtual IMasterScan^ AddAllObjectsScan(System::Byte group, System::Byte variation, System::TimeSpan period, TaskConfig^ config);

				virtual IMasterScan^ AddClassScan(ClassField field, System::TimeSpan period, TaskConfig^ config);

				virtual IMasterScan^ AddRangeScan(System::Byte group, System::Byte variation, System::UInt16 start, System::UInt16 stop, System::TimeSpan period, TaskConfig^ config);												

				/// --- implement ICommandProcessor ----

				virtual Task<CommandTaskResult^>^ SelectAndOperate(ICommandHeaders^ headers, TaskConfig^ config);
				virtual Task<CommandTaskResult^>^ DirectOperate(ICommandHeaders^ headers, TaskConfig^ config);

				virtual Task<CommandTaskResult^>^ SelectAndOperate(ControlRelayOutputBlock^ command, System::UInt16 index, TaskConfig^ config);			
				virtual Task<CommandTaskResult^>^ SelectAndOperate(AnalogOutputInt16^ command, System::UInt16 index, TaskConfig^ config);				
				virtual Task<CommandTaskResult^>^ SelectAndOperate(AnalogOutputInt32^ command, System::UInt16 index, TaskConfig^ config);				
				virtual Task<CommandTaskResult^>^ SelectAndOperate(AnalogOutputFloat32^ command, System::UInt16 index, TaskConfig^ config);				
				virtual Task<CommandTaskResult^>^ SelectAndOperate(AnalogOutputDouble64^ command, System::UInt16 index, TaskConfig^ config);
				
				virtual Task<CommandTaskResult^>^ DirectOperate(ControlRelayOutputBlock^ command, System::UInt16 index, TaskConfig^ config);
				virtual Task<CommandTaskResult^>^ DirectOperate(AnalogOutputDouble64^ command, System::UInt16 index, TaskConfig^ config);				
				virtual Task<CommandTaskResult^>^ DirectOperate(AnalogOutputInt16^ command, System::UInt16 index, TaskConfig^ config);
				virtual Task<CommandTaskResult^>^ DirectOperate(AnalogOutputInt32^ command, System::UInt16 index, TaskConfig^ config);
				virtual Task<CommandTaskResult^>^ DirectOperate(AnalogOutputFloat32^ command, System::UInt16 index, TaskConfig^ config);
				

			private:				
				
				asiodnp3::IMasterOperations* operations;
			};

		}
	}
}
#endif
