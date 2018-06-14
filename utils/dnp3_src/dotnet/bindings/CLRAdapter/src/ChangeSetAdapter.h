#ifndef __CHANGESET_ADAPTER_H_
#define __CHANGESET_ADAPTER_H_

#include <asiodnp3/IOutstation.h>
#include <asiodnp3/UpdateBuilder.h>

#include <vcclr.h>

using namespace Automatak::DNP3::Interface;

namespace Automatak
{
	namespace DNP3
	{
		namespace Adapter
		{
			private ref class ChangeSetAdapter : public Automatak::DNP3::Interface::IDatabase
			{
			public:

				ChangeSetAdapter();

				~ChangeSetAdapter();
				
				virtual void Update(Binary^ update, System::UInt16 index, EventMode mode);
				virtual void Update(DoubleBitBinary^ update, System::UInt16 index, EventMode mode);				
				virtual void Update(Analog^ update, System::UInt16 index, EventMode mode);
				virtual void Update(Counter^ update, System::UInt16 index, EventMode mode);
				virtual void Update(FrozenCounter^ update, System::UInt16 index, EventMode mode);
				virtual void Update(BinaryOutputStatus^ update, System::UInt16 index, EventMode mode);
				virtual void Update(AnalogOutputStatus^ update, System::UInt16 index, EventMode mode);
				virtual void Update(TimeAndInterval^ update, System::UInt16 index);							

				void Apply(asiodnp3::IOutstation& proxy);
								
			private:
				
				asiodnp3::UpdateBuilder* builder;
			};
								
			
		}
	}
}

#endif
