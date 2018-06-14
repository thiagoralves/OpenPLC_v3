
#include "MasterOperationsAdapter.h"
#include "Conversions.h"


using namespace Automatak::DNP3::Interface;

namespace Automatak
{
	namespace DNP3
	{
		namespace Adapter
		{				

				MasterOperationsAdapter::MasterOperationsAdapter(asiodnp3::IMasterOperations* operations) : operations(operations)
				{}				
			
				void MasterOperationsAdapter::SetLogFilters(LogFilter flags)
				{
					operations->SetLogFilters(flags.Flags);
				}
			
				Task<TaskCompletion>^ MasterOperationsAdapter::Scan(IEnumerable<Header^>^ headers, TaskConfig^ config)
				{
					auto proxy = gcnew TaskCompletionProxy(config->callback);
					auto vec = MasterConversions::ConvertToVectorOfHeaders(headers);
					operations->Scan(vec, MasterConversions::Convert(config, proxy));
					return proxy->CompletionTask;
				}

				Task<TaskCompletion>^ MasterOperationsAdapter::ScanAllObjects(System::Byte group, System::Byte variation, TaskConfig^ config)
				{
					auto proxy = gcnew TaskCompletionProxy(config->callback);
					operations->ScanAllObjects(opendnp3::GroupVariationID(group, variation), MasterConversions::Convert(config, proxy));
					return proxy->CompletionTask;
				}

				Task<TaskCompletion>^ MasterOperationsAdapter::ScanClasses(ClassField field, TaskConfig^ config)
				{
					auto proxy = gcnew TaskCompletionProxy(config->callback);
					operations->ScanClasses(Conversions::ConvertClassField(field), MasterConversions::Convert(config, proxy));
					return proxy->CompletionTask;
				}
				
				Task<TaskCompletion>^ MasterOperationsAdapter::ScanRange(System::Byte group, System::Byte variation, System::UInt16 start, System::UInt16 stop, TaskConfig^ config)
				{
					opendnp3::GroupVariationID gvid(group, variation);
					auto proxy = gcnew TaskCompletionProxy(config->callback);
					operations->ScanRange(gvid, start, stop, MasterConversions::Convert(config, proxy));
					return proxy->CompletionTask;
				}

				Task<TaskCompletion>^ MasterOperationsAdapter::Write(TimeAndInterval^ value, System::UInt16 index, TaskConfig^ config)
				{
					auto proxy = gcnew TaskCompletionProxy(config->callback);
					operations->Write(Conversions::ConvertMeas(value), index, MasterConversions::Convert(config, proxy));
					return proxy->CompletionTask;
				}

				Task<RestartResultType^>^ MasterOperationsAdapter::Restart(RestartType restartType, TaskConfig^ config)
				{
					auto tcs = gcnew TaskCompletionSource<RestartResultType^>();

					operations->Restart((opendnp3::RestartType) restartType,  CallbackAdapters::Get(tcs), MasterConversions::Convert(config));

					return tcs->Task;
				}

				Task<TaskCompletion>^ MasterOperationsAdapter::PerformFunction(System::String^ name, FunctionCode func, IEnumerable<Header^>^ headers, TaskConfig^ config)
				{
					auto proxy = gcnew TaskCompletionProxy(config->callback);
					auto vec = MasterConversions::ConvertToVectorOfHeaders(headers);
					auto nativeName = Conversions::ConvertString(name);
					operations->PerformFunction(nativeName, (opendnp3::FunctionCode) func, vec, MasterConversions::Convert(config, proxy));
					return proxy->CompletionTask;
				}

				IMasterScan^ MasterOperationsAdapter::AddScan(IEnumerable<Header^>^ headers, System::TimeSpan period, TaskConfig^ config)
				{
					auto vec = MasterConversions::ConvertToVectorOfHeaders(headers);
					auto scan = operations->AddScan(Conversions::ConvertTimespan(period), vec, MasterConversions::Convert(config));
					return gcnew MasterScanAdapter(scan);
				}

				IMasterScan^ MasterOperationsAdapter::AddAllObjectsScan(System::Byte group, System::Byte variation, System::TimeSpan period, TaskConfig^ config)
				{
					opendnp3::GroupVariationID gvid(group, variation);
					auto scan = operations->AddAllObjectsScan(gvid, Conversions::ConvertTimespan(period), MasterConversions::Convert(config));
					return gcnew MasterScanAdapter(scan);
				}

				IMasterScan^ MasterOperationsAdapter::AddClassScan(ClassField field, System::TimeSpan period, TaskConfig^ config)
				{
					auto scan = operations->AddClassScan(Conversions::ConvertClassField(field), Conversions::ConvertTimespan(period), MasterConversions::Convert(config));
					return gcnew MasterScanAdapter(scan);
				}

				IMasterScan^ MasterOperationsAdapter::AddRangeScan(System::Byte group, System::Byte variation, System::UInt16 start, System::UInt16 stop, System::TimeSpan period, TaskConfig^ config)
				{
					opendnp3::GroupVariationID gvid(group, variation);
					auto scan = operations->AddRangeScan(gvid, start, stop, Conversions::ConvertTimespan(period), MasterConversions::Convert(config));
					return gcnew MasterScanAdapter(scan);
				}
												
				Task<CommandTaskResult^>^ MasterOperationsAdapter::SelectAndOperate(ICommandHeaders^ headers, TaskConfig^ config)
				{
					auto tcs = gcnew TaskCompletionSource<CommandTaskResult^>();					
					operations->SelectAndOperate(MasterConversions::Convert(headers), CallbackAdapters::Get(tcs), MasterConversions::Convert(config));
					return tcs->Task;
				}

				Task<CommandTaskResult^>^ MasterOperationsAdapter::DirectOperate(ICommandHeaders^ headers, TaskConfig^ config)
				{
					auto tcs = gcnew TaskCompletionSource<CommandTaskResult^>();
					operations->DirectOperate(MasterConversions::Convert(headers), CallbackAdapters::Get(tcs), MasterConversions::Convert(config));
					return tcs->Task;
				}

				Task<CommandTaskResult^>^ MasterOperationsAdapter::SelectAndOperate(ControlRelayOutputBlock^ command, System::UInt16 index, TaskConfig^ config)
				{
					ICommandHeaders^ headers = CommandHeader::From(IndexedValue::From(command, index));
					return this->SelectAndOperate(headers, config);					
				}

				Task<CommandTaskResult^>^ MasterOperationsAdapter::SelectAndOperate(AnalogOutputInt16^ command, System::UInt16 index, TaskConfig^ config)
				{
					ICommandHeaders^ headers = CommandHeader::From(IndexedValue::From(command, index));
					return this->SelectAndOperate(headers, config);					
				}

				Task<CommandTaskResult^>^ MasterOperationsAdapter::SelectAndOperate(AnalogOutputInt32^ command, System::UInt16 index, TaskConfig^ config)
				{
					ICommandHeaders^ headers = CommandHeader::From(IndexedValue::From(command, index));
					return this->SelectAndOperate(headers, config);					
				}

				Task<CommandTaskResult^>^ MasterOperationsAdapter::SelectAndOperate(AnalogOutputFloat32^ command, System::UInt16 index, TaskConfig^ config)
				{
					ICommandHeaders^ headers = CommandHeader::From(IndexedValue::From(command, index));
					return this->SelectAndOperate(headers, config);					
				}

				Task<CommandTaskResult^>^ MasterOperationsAdapter::SelectAndOperate(AnalogOutputDouble64^ command, System::UInt16 index, TaskConfig^ config)
				{
					ICommandHeaders^ headers = CommandHeader::From(IndexedValue::From(command, index));
					return this->SelectAndOperate(headers, config);					
				}
				

				Task<CommandTaskResult^>^ MasterOperationsAdapter::DirectOperate(ControlRelayOutputBlock^ command, System::UInt16 index, TaskConfig^ config)
				{
					ICommandHeaders^ headers = CommandHeader::From(IndexedValue::From(command, index));
					return this->DirectOperate(headers, config);					
				}

				Task<CommandTaskResult^>^ MasterOperationsAdapter::DirectOperate(AnalogOutputDouble64^ command, System::UInt16 index, TaskConfig^ config)
				{
					ICommandHeaders^ headers = CommandHeader::From(IndexedValue::From(command, index));
					return this->DirectOperate(headers, config);					
				}
				
				Task<CommandTaskResult^>^ MasterOperationsAdapter::DirectOperate(AnalogOutputInt16^ command, System::UInt16 index, TaskConfig^ config)
				{
					ICommandHeaders^ headers = CommandHeader::From(IndexedValue::From(command, index));
					return this->DirectOperate(headers, config);
				}

				Task<CommandTaskResult^>^ MasterOperationsAdapter::DirectOperate(AnalogOutputInt32^ command, System::UInt16 index, TaskConfig^ config)
				{
					ICommandHeaders^ headers = CommandHeader::From(IndexedValue::From(command, index));
					return this->DirectOperate(headers, config);					
				}

				Task<CommandTaskResult^>^ MasterOperationsAdapter::DirectOperate(AnalogOutputFloat32^ command, System::UInt16 index, TaskConfig^ config)
				{
					ICommandHeaders^ headers = CommandHeader::From(IndexedValue::From(command, index));
					return this->DirectOperate(headers, config);					
				}
		}
	}
}

