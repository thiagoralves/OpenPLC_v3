
#include "CommandSetBuilder.h"

#include "Conversions.h"

namespace Automatak
{
	namespace DNP3
	{
		namespace Adapter
		{

			
CommandSetBuilder::CommandSetBuilder(opendnp3::CommandSet& commands) : commands(&commands)
{}

void CommandSetBuilder::Add(IEnumerable<IndexedValue<ControlRelayOutputBlock^>^>^ commands)
{
	auto& header = this->commands->StartHeader<opendnp3::ControlRelayOutputBlock>();
	for each(auto pair in commands)
	{
		header.Add(Conversions::ConvertCommand(pair->Value), pair->Index);
	}
}

void CommandSetBuilder::Add(IEnumerable<IndexedValue<AnalogOutputInt16^>^>^ commands)
{
	auto& header = this->commands->StartHeader<opendnp3::AnalogOutputInt16>();
	for each(auto pair in commands)
	{
		header.Add(Conversions::ConvertCommand(pair->Value), pair->Index);
	}
}

void CommandSetBuilder::Add(IEnumerable<IndexedValue<AnalogOutputInt32^>^>^ commands)
{
	auto& header = this->commands->StartHeader<opendnp3::AnalogOutputInt32>();
	for each(auto pair in commands)
	{
		header.Add(Conversions::ConvertCommand(pair->Value), pair->Index);
	}
}

void CommandSetBuilder::Add(IEnumerable<IndexedValue<AnalogOutputFloat32^>^>^ commands)
{
	auto& header = this->commands->StartHeader<opendnp3::AnalogOutputFloat32>();
	for each(auto pair in commands)
	{
		header.Add(Conversions::ConvertCommand(pair->Value), pair->Index);
	}
}

void CommandSetBuilder::Add(IEnumerable<IndexedValue<AnalogOutputDouble64^>^>^ commands)
{
	auto& header = this->commands->StartHeader<opendnp3::AnalogOutputDouble64>();
	for each(auto pair in commands)
	{
		header.Add(Conversions::ConvertCommand(pair->Value), pair->Index);
	}
}




		}
	}
}

