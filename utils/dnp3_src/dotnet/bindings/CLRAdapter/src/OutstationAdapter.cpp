

#include "OutstationAdapter.h"

#include "ChangeSetAdapter.h"
#include "Conversions.h"

namespace Automatak
{
	namespace DNP3
	{
		namespace Adapter
		{

			OutstationAdapter::OutstationAdapter(const std::shared_ptr<asiodnp3::IOutstation>& outstation) : 
				outstation(new std::shared_ptr<asiodnp3::IOutstation>(outstation))
			{}

			OutstationAdapter::!OutstationAdapter()
			{
				delete outstation;
			}

			void OutstationAdapter::SetLogFilters(LogFilter filters)
			{				
				(*outstation)->SetLogFilters(filters.Flags);
			}

			void OutstationAdapter::Load(IChangeSet^ changes)
			{
				auto adapter = gcnew ChangeSetAdapter();
				changes->Apply(adapter);
				adapter->Apply(**outstation);
			}			

			void OutstationAdapter::SetRestartIIN()
			{
				(*outstation)->SetRestartIIN();
			}

			void OutstationAdapter::Shutdown()
			{
				(*outstation)->Shutdown();
			}

			void OutstationAdapter::Enable()
			{
				(*outstation)->Enable();
			}

			void OutstationAdapter::Disable()
			{
				(*outstation)->Disable();
			}

			IStackStatistics^ OutstationAdapter::GetStackStatistics()
			{
				auto stats = (*outstation)->GetStackStatistics();
				return Conversions::ConvertStackStats(stats);
			}

		}
	}
}
