
#include "OutstationCommandHandlerAdapter.h"
#include "Conversions.h"


namespace Automatak
{
	namespace DNP3
	{
		namespace Adapter
		{

			void OutstationCommandHandlerAdapter::Start()
			{
				proxy->Start();
			}
			
			void OutstationCommandHandlerAdapter::End()
			{
				proxy->End();
			}

			OutstationCommandHandlerAdapter::OutstationCommandHandlerAdapter(Automatak::DNP3::Interface::ICommandHandler^ proxy) : proxy(proxy)
			{

			}

			opendnp3::CommandStatus OutstationCommandHandlerAdapter::Select(const opendnp3::ControlRelayOutputBlock& command, uint16_t index)
			{
				return Conversions::ConvertCommandStatus(proxy->Select(Conversions::ConvertCommand(command), index));
			}

			opendnp3::CommandStatus OutstationCommandHandlerAdapter::Select(const opendnp3::AnalogOutputInt32& command, uint16_t index)
			{
				return Conversions::ConvertCommandStatus(proxy->Select(Conversions::ConvertCommand(command), index));
			}

			opendnp3::CommandStatus OutstationCommandHandlerAdapter::Select(const opendnp3::AnalogOutputInt16& command, uint16_t index)
			{
				return Conversions::ConvertCommandStatus(proxy->Select(Conversions::ConvertCommand(command), index));
			}

			opendnp3::CommandStatus OutstationCommandHandlerAdapter::Select(const opendnp3::AnalogOutputFloat32& command, uint16_t index)
			{
				return Conversions::ConvertCommandStatus(proxy->Select(Conversions::ConvertCommand(command), index));
			}

			opendnp3::CommandStatus OutstationCommandHandlerAdapter::Select(const opendnp3::AnalogOutputDouble64& command, uint16_t index)
			{
				return Conversions::ConvertCommandStatus(proxy->Select(Conversions::ConvertCommand(command), index));
			}

			opendnp3::CommandStatus OutstationCommandHandlerAdapter::Operate(const opendnp3::ControlRelayOutputBlock& command, uint16_t index, opendnp3::OperateType opType)
			{
				return Conversions::ConvertCommandStatus(proxy->Operate(Conversions::ConvertCommand(command), index, (OperateType) opType));
			}

			opendnp3::CommandStatus OutstationCommandHandlerAdapter::Operate(const opendnp3::AnalogOutputInt32& command, uint16_t index, opendnp3::OperateType opType)
			{
				return Conversions::ConvertCommandStatus(proxy->Operate(Conversions::ConvertCommand(command), index, (OperateType)opType));
			}

			opendnp3::CommandStatus OutstationCommandHandlerAdapter::Operate(const opendnp3::AnalogOutputInt16& command, uint16_t index, opendnp3::OperateType opType)
			{
				return Conversions::ConvertCommandStatus(proxy->Operate(Conversions::ConvertCommand(command), index, (OperateType)opType));
			}

			opendnp3::CommandStatus OutstationCommandHandlerAdapter::Operate(const opendnp3::AnalogOutputFloat32& command, uint16_t index, opendnp3::OperateType opType)
			{
				return Conversions::ConvertCommandStatus(proxy->Operate(Conversions::ConvertCommand(command), index, (OperateType)opType));
			}

			opendnp3::CommandStatus OutstationCommandHandlerAdapter::Operate(const opendnp3::AnalogOutputDouble64& command, uint16_t index, opendnp3::OperateType opType)
			{
				return Conversions::ConvertCommandStatus(proxy->Operate(Conversions::ConvertCommand(command), index, (OperateType)opType));
			}

		}
	}
}

