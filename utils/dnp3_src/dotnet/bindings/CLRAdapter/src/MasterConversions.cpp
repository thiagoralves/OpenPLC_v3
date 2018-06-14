
#include "MasterConversions.h"

#include "TaskCallbackAdapter.h"

#include "CommandSetBuilder.h"

#include <openpal/container/SecureBuffer.h>

namespace Automatak
{
	namespace DNP3
	{
		namespace Adapter
		{
			std::vector<opendnp3::Header> MasterConversions::ConvertToVectorOfHeaders(IEnumerable<Header^>^ headers)
			{
				std::vector<opendnp3::Header> ret;
				for each(auto header in headers)
				{
					opendnp3::Header output;
					if (Convert(header, output))
					{
						ret.push_back(output);
					}
				}
				return ret;
			}

			bool MasterConversions::Convert(Header^ header, opendnp3::Header& output)
			{
				switch (header->qualifier)
				{
				case(QualifierCode::ALL_OBJECTS) :
				{
					output = opendnp3::Header::AllObjects(header->group, header->variation);
					return true;
				}
				case(QualifierCode::UINT8_CNT) :
				{
					auto value = dynamic_cast<CountHeader^>(header);
					if (value == nullptr)
					{
						return false;
					}
					else
					{
						output = ConvertCount8(value);
						return true;
					}
				}
				case(QualifierCode::UINT16_CNT) :
				{
					auto value = dynamic_cast<CountHeader^>(header);
					if (value == nullptr)
					{
						return false;
					}
					else
					{
						output = ConvertCount16(value);
						return true;
					}
				}
				case(QualifierCode::UINT8_START_STOP) :
				{
					auto value = dynamic_cast<RangeHeader^>(header);
					if (value == nullptr)
					{
						return false;
					}
					else
					{
						output = ConvertRange8(value);
						return true;
					}
				}
				case(QualifierCode::UINT16_START_STOP) :
				{
					auto value = dynamic_cast<RangeHeader^>(header);
					if (value == nullptr)
					{
						return false;
					}
					else
					{
						output = ConvertRange16(value);
						return true;
					}
				}
				default:
					return false;
				}
			}
		
			opendnp3::Header MasterConversions::Convert(Header^ header)
			{
				return opendnp3::Header::AllObjects(header->group, header->variation);
			}

			opendnp3::Header MasterConversions::ConvertCount8(CountHeader^ header)
			{
				return opendnp3::Header::Count8(header->group, header->variation, static_cast<uint8_t>(header->count));
			}

			opendnp3::Header MasterConversions::ConvertCount16(CountHeader^ header)
			{
				return opendnp3::Header::Count16(header->group, header->variation, header->count);
			}

			opendnp3::Header MasterConversions::ConvertRange8(RangeHeader^ header)
			{
				return opendnp3::Header::Range8(header->group, header->variation, static_cast<uint8_t>(header->start), static_cast<uint8_t>(header->stop));
			}

			opendnp3::Header MasterConversions::ConvertRange16(RangeHeader^ header)
			{
				return opendnp3::Header::Range16(header->group, header->variation, header->start, header->stop);
			}

			opendnp3::TaskConfig MasterConversions::Convert(TaskConfig^ config, ITaskCallback^ wrapper)
			{			
				return opendnp3::TaskConfig(Convert(config->taskId), CreateTaskCallback(wrapper));
			}

			opendnp3::TaskConfig MasterConversions::Convert(TaskConfig^ config)
			{
				return opendnp3::TaskConfig(Convert(config->taskId), CreateTaskCallback(config->callback));
			}
			
			opendnp3::CommandSet MasterConversions::Convert(ICommandHeaders^ headers)
			{
				opendnp3::CommandSet commands;

				auto builder = gcnew CommandSetBuilder(commands);

				headers->Build(builder);

				return commands;
			}

			opendnp3::TaskId MasterConversions::Convert(TaskId^ id)
			{
				return id->IsValid ? opendnp3::TaskId::Defined(id->Value) : opendnp3::TaskId::Undefined();
			}

			TaskId^ MasterConversions::Convert(const opendnp3::TaskId& id)
			{
				return id.IsDefined() ? TaskId::Defined(id.GetId()) : TaskId::Undefined;
			}

			TaskInfo^ MasterConversions::Convert(const opendnp3::TaskInfo& info)
			{
				return gcnew TaskInfo((MasterTaskType)info.type, (TaskCompletion)info.result, MasterConversions::Convert(info.id));
			}

			opendnp3::ITaskCallback* MasterConversions::CreateTaskCallback(ITaskCallback^ callback)
			{
				return (callback == nullptr) ? nullptr : TaskCallbackAdapter::Create(callback);
			}
		}
	}
}