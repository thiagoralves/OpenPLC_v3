

#include "MasterAdapter.h"
#include "Conversions.h"

using namespace Automatak::DNP3::Interface;

namespace Automatak
{
	namespace DNP3
	{
		namespace Adapter
		{
			MasterAdapter::MasterAdapter(const std::shared_ptr<asiodnp3::IMaster>& master) : 
				MasterOperationsAdapter(master.get()), 
				master(new std::shared_ptr<asiodnp3::IMaster>(master))
			{}

			MasterAdapter::!MasterAdapter()
			{
				delete master;				
			}

			void MasterAdapter::Enable()
			{
				(*master)->Enable();				
			}

			void MasterAdapter::Disable()
			{
				(*master)->Disable();				
			}

			void MasterAdapter::Shutdown()
			{
				(*master)->Shutdown();				
			}

			Interface::IStackStatistics^ MasterAdapter::GetStackStatistics()
			{
				return Conversions::ConvertStackStats((*master)->GetStackStatistics());
			}
		}
	}
}

