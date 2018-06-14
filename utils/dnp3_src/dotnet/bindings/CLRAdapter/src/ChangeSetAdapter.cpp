

#include "ChangeSetAdapter.h"
#include "Conversions.h"


namespace Automatak
{
	namespace DNP3
	{
		namespace Adapter
		{
			ChangeSetAdapter::ChangeSetAdapter() : builder(new asiodnp3::UpdateBuilder())
			{}

			ChangeSetAdapter::~ChangeSetAdapter()
			{
				delete builder;
			}

			void ChangeSetAdapter::Apply(asiodnp3::IOutstation& proxy)
			{
				proxy.Apply(builder->Build());				
			}

			void ChangeSetAdapter::Update(Binary^ update, System::UInt16 index, EventMode mode)
			{
				builder->Update(Conversions::ConvertMeas(update), index, (opendnp3::EventMode) mode);
			}
			
			void ChangeSetAdapter::Update(DoubleBitBinary^ update, System::UInt16 index, EventMode mode)
			{
				builder->Update(Conversions::ConvertMeas(update), index, (opendnp3::EventMode) mode);
			}

			void ChangeSetAdapter::Update(Analog^ update, System::UInt16 index, EventMode mode)
			{
				builder->Update(Conversions::ConvertMeas(update), index, (opendnp3::EventMode) mode);
			}

			void ChangeSetAdapter::Update(Counter^ update, System::UInt16 index, EventMode mode)
			{
				builder->Update(Conversions::ConvertMeas(update), index, (opendnp3::EventMode) mode);
			}

			void ChangeSetAdapter::Update(FrozenCounter^ update, System::UInt16 index, EventMode mode)
			{
				builder->Update(Conversions::ConvertMeas(update), index, (opendnp3::EventMode) mode);
			}
			
			void ChangeSetAdapter::Update(BinaryOutputStatus^ update, System::UInt16 index, EventMode mode)
			{
				builder->Update(Conversions::ConvertMeas(update), index, (opendnp3::EventMode) mode);
			}

			void ChangeSetAdapter::Update(AnalogOutputStatus^ update, System::UInt16 index, EventMode mode)
			{
				builder->Update(Conversions::ConvertMeas(update), index, (opendnp3::EventMode) mode);
			}

			void ChangeSetAdapter::Update(TimeAndInterval^ update, System::UInt16 index)
			{
				builder->Update(Conversions::ConvertMeas(update), index);
			}			
		}
	}
}
