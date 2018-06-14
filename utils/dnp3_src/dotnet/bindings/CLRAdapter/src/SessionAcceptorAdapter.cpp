#include "SessionAcceptorAdapter.h"

#include "Conversions.h"
#include "SOEHandlerAdapter.h"
#include "MasterApplicationAdapter.h"
#include "MasterSessionAdapter.h"

namespace Automatak { namespace DNP3 { namespace Adapter {

SessionAcceptorAdapter::SessionAcceptorAdapter(asiodnp3::ISessionAcceptor& proxy) :	
	proxy(&proxy)
{}

IMasterSession^ SessionAcceptorAdapter::AcceptSession(
	System::String^ loggerid,
	ISOEHandler^ SOEHandler,
	IMasterApplication^ application,
	MasterStackConfig^ config
	)
{
	auto id = Conversions::ConvertString(loggerid);
	auto mconfig = Conversions::ConvertConfig(config);
	
	auto handler = std::shared_ptr<SOEHandlerAdapter>(new SOEHandlerAdapter(SOEHandler));
	
	auto app = std::shared_ptr<MasterApplicationAdapter<opendnp3::IMasterApplication>>(
		new MasterApplicationAdapter<opendnp3::IMasterApplication>(application)
	);
	
	auto session = proxy->AcceptSession(id, handler, app, mconfig);

	if (!session)
	{
		return nullptr;
	}	
	
	return gcnew MasterSessionAdapter(session);
}

}}} 