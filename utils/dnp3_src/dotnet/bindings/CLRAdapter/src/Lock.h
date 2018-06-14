#ifndef __LOCK_H_
#define __LOCK_H_

using namespace System::Threading;

private ref class Lock
{
	Object^ m_pObject;
public:
	Lock( Object^ pObject ) : m_pObject( pObject )
	{
		Monitor::Enter( m_pObject );
	}
	~Lock()
	{
		Monitor::Exit( m_pObject );
	}
};

#endif
