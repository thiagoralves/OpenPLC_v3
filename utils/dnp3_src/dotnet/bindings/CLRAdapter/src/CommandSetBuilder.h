#ifndef DNP3ADAPTER_COMMAND_SET_BUILDER_H
#define DNP3ADAPTER_COMMAND_SET_BUILDER_H

using namespace System::Collections::ObjectModel;
using namespace System::Collections::Generic;

using namespace Automatak::DNP3::Interface;

#include <opendnp3/master/CommandSet.h>

namespace Automatak
{
	namespace DNP3
	{
		namespace Adapter
		{

			private ref class CommandSetBuilder : ICommandBuilder
			{
			public:				

				CommandSetBuilder(opendnp3::CommandSet& commands);

				virtual void Add(IEnumerable<IndexedValue<ControlRelayOutputBlock^>^>^ commands);
				virtual void Add(IEnumerable<IndexedValue<AnalogOutputInt16^>^>^ commands);
				virtual void Add(IEnumerable<IndexedValue<AnalogOutputInt32^>^>^ commands);
				virtual void Add(IEnumerable<IndexedValue<AnalogOutputFloat32^>^>^ commands);
				virtual void Add(IEnumerable<IndexedValue<AnalogOutputDouble64^>^>^ commands);

			private:
				
				opendnp3::CommandSet* commands;
			};

		}
	}
}
#endif
