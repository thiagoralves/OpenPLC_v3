
#include "ListenCallbacksAdapter.h"

#include "Conversions.h"
#include "SessionAcceptorAdapter.h"
#include "MasterSessionAdapter.h"

namespace Automatak { namespace DNP3 { namespace Adapter {

	ListenCallbacksAdapter::ListenCallbacksAdapter(Interface::IListenCallbacks^ proxy) : 
		m_proxy(proxy)		
	{}

	bool ListenCallbacksAdapter::AcceptConnection(uint64_t sessionid, const std::string& ipaddress)
	{		
		return m_proxy->AcceptConnection(sessionid, Conversions::ConvertString(ipaddress));
	}

	bool ListenCallbacksAdapter::AcceptCertificate(uint64_t sessionid, const asiodnp3::X509Info& info)
	{
		return m_proxy->AcceptCertificate(sessionid, Conversions::Convert(info));
	}

	openpal::TimeDuration ListenCallbacksAdapter::GetFirstFrameTimeout()
	{
		return Conversions::ConvertTimespan(m_proxy->GetFirstFrameTimeout());
	}

	void ListenCallbacksAdapter::OnFirstFrame(uint64_t sessionid, const opendnp3::LinkHeaderFields& header, asiodnp3::ISessionAcceptor& acceptor)
	{
		auto linkheader = Conversions::Convert(header);
		auto adapter = gcnew SessionAcceptorAdapter(acceptor);
		m_proxy->OnFirstFrame(sessionid, linkheader, adapter);
	}

	void ListenCallbacksAdapter::OnConnectionClose(uint64_t sessionid, const std::shared_ptr<asiodnp3::IMasterSession>& session)
	{	

		m_proxy->OnConnectionClose(sessionid, session ? gcnew MasterSessionAdapter(session) : nullptr);
	}

	void ListenCallbacksAdapter::OnCertificateError(uint64_t sessionid, const asiodnp3::X509Info& info, int error)
	{
		m_proxy->OnCertificateError(sessionid, Conversions::Convert(info), error);
	}

}}}


