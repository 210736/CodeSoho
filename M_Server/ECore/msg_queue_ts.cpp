//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2003-11-30
// Last: 2004-03-26
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "msg_queue_ts.h"

namespace ECore {
//-----------------------------------------------------------------------------
// 构造和析构
//-----------------------------------------------------------------------------
MsgQueueTS::MsgQueueTS(BOOL bActiveEvent, BOOL bAddSizeAhead, DWORD dwCacheSize)
{
	m_bEvent	= bActiveEvent;
	m_bAddSize	= bAddSizeAhead;
	
	m_nMsg		=	0;
	m_hEvent	=	NULL;
	m_pBegin	=	NULL;
	m_pEnd		=	NULL;

	if( m_bEvent )
		m_hEvent = ::CreateEvent(NULL, FALSE, TRUE, NULL);	// 自动激活
}

MsgQueueTS::~MsgQueueTS()
{
	Msg* pMsg = m_pBegin;

	while( m_pBegin )
	{
		pMsg = m_pBegin->pNext;
		MCFREE(m_pBegin);
		m_pBegin = pMsg;
	}

	SAFE_CLOSE_HANDLE(m_hEvent);
}

}