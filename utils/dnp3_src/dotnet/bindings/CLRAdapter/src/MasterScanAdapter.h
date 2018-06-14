#ifndef OPENDNP3NET_MASTERSCANADAPTER_H_
#define OPENDNP3NET_MASTERSCANADAPTER_H_

using namespace System::Collections::ObjectModel;

#include <asiodnp3/IMasterScan.h>
#include <vcclr.h>

using namespace Automatak::DNP3::Interface;
using namespace System::Collections::Generic;

namespace Automatak
{
	namespace DNP3
	{
		namespace Adapter
		{			
			private ref class MasterScanAdapter : IMasterScan
			{
				public:

				MasterScanAdapter(const std::shared_ptr<asiodnp3::IMasterScan>& scan) : 
						scan(new std::shared_ptr <asiodnp3::IMasterScan>(scan))
				{}

				~MasterScanAdapter()
				{
					this->!MasterScanAdapter();
				}
				
				!MasterScanAdapter()
				{
					delete scan;
				}

				virtual void Demand()
				{
					(*this->scan)->Demand();
				}

				private:

				std::shared_ptr <asiodnp3::IMasterScan>* scan;
			};

		}
	}
}

#endif
