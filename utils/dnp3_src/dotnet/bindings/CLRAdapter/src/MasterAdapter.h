#ifndef OPENDNP3CLR_MASTER_ADAPTER_H
#define OPENDNP3CLR_MASTER_ADAPTER_H

#include "MasterOperationsAdapter.h"
#include <asiodnp3/IMaster.h>

using namespace Automatak::DNP3::Interface;

namespace Automatak
{
	namespace DNP3
	{
		namespace Adapter
		{			
			private ref class MasterAdapter : MasterOperationsAdapter, IMaster
			{
			public:

				MasterAdapter(const std::shared_ptr<asiodnp3::IMaster>& master);

				~MasterAdapter() { this ->!MasterAdapter(); }
				!MasterAdapter();
			
				virtual void Enable();

				virtual void Disable();

				virtual void Shutdown();

				virtual Interface::IStackStatistics^ GetStackStatistics();
				

			private:				
				
				std::shared_ptr<asiodnp3::IMaster>* master;				
			};

		}
	}
}
#endif
