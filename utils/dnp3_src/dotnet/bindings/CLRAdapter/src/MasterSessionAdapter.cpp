#include "MasterSessionAdapter.h"

namespace Automatak { namespace DNP3 { namespace Adapter {


MasterSessionAdapter::MasterSessionAdapter(std::shared_ptr<asiodnp3::IMasterSession> proxy) :
	MasterOperationsAdapter(proxy.get()),
	proxy(new std::shared_ptr<asiodnp3::IMasterSession>(proxy))
{

}

void MasterSessionAdapter::BeginShutdown()
{
	(*proxy)->BeginShutdown();
}

Interface::IStackStatistics^ MasterSessionAdapter::GetStackStatistics()
{
	return Conversions::ConvertStackStats((*proxy)->GetStackStatistics());
}

}}}


