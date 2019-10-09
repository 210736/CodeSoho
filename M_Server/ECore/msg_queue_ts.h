//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2003-11-30
// Last: 2004-03-26
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class ECORE_API MsgQueueTS
{
public:
	INLINE BOOL		AddMsg(LPCVOID pMsg, DWORD dwSize);
	INLINE LPBYTE	GetMsg(DWORD& dwMsgSize);
	INLINE VOID		FreeMsg(LPVOID pMsg);
	INLINE VOID		Clear();

	INLINE INT		GetMsgNum()	{ return m_nMsg; }

	//--------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------
	INLINE HANDLE	GetEvent() { return m_hEvent; }

	INLINE VOID		SetAddSizeAhead(BOOL bAdd) { m_bAddSize = bAdd; }

	//--------------------------------------------------------------------------
	// bActiveEvent: ��Ϣ����ʱ������event, bAddSizeAhead: ��ͷ��������Ϣ����
	//--------------------------------------------------------------------------
	MsgQueueTS(BOOL bActiveEvent, BOOL bAddSizeAhead, DWORD dwCacheSize=1024*1024);
	~MsgQueueTS();


private:

#pragma warning(push)
#pragma warning(disable: 4200)

	struct Msg
	{
		Msg*	pNext;
		DWORD	dwDataSize;
		DWORD	dwReserved;
		BYTE	byData[0];
	};

#pragma warning(pop)

	Msg*	m_pBegin;
	Msg*	m_pEnd;

	Mutex	m_Lock;
	HANDLE	m_hEvent;

	INT		m_nMsg;
	BOOL	m_bEvent;
	BOOL	m_bAddSize;	// �Ƿ�ͷ��������Ϣ����

};



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOL MsgQueueTS::AddMsg(LPCVOID pMsg, DWORD dwSize)
{
	if( !VALID(pMsg) )
		return FALSE;
	if( 0 == dwSize )
		return FALSE;

	DWORD dwOriginSize = dwSize;
	if( m_bAddSize )				// �˳�Ա�ڶ��󴴽���Ͳ���ı�
		dwSize += sizeof(DWORD);

	// �����µĵ�Ԫ
	Msg* pMsgItem = (Msg*)MCALLOC(sizeof(Msg) + dwSize);
	if( !pMsgItem )
		return FALSE;

	// ��ʼ��
	pMsgItem->pNext = NULL;
	pMsgItem->dwDataSize = dwSize;
	
	// ��������
	if( m_bAddSize )
	{
		*(DWORD*)(pMsgItem->byData) = dwOriginSize;
		memcpy(pMsgItem->byData+sizeof(DWORD), pMsg, dwOriginSize);
	}
	else
	{
		memcpy(pMsgItem->byData, pMsg, dwSize);
	}

	m_Lock.Lock();

	// �������
	if( !m_pBegin )	// ���п�
	{
		m_pBegin = pMsgItem;
		m_pEnd = pMsgItem;
	}
	else					// �ǿ�
	{
		m_pEnd->pNext = pMsgItem;
		m_pEnd = pMsgItem;
	}

	++m_nMsg;

	if( m_bEvent )	// �����߳�
		::SetEvent(m_hEvent);	

	m_Lock.Unlock();
	return TRUE;
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
LPBYTE MsgQueueTS::GetMsg(DWORD& dwMsgSize)
{
	dwMsgSize = 0;
	if( m_nMsg <= 0 )
		return NULL;

	m_Lock.Lock();

	if( m_nMsg <= 0 )
	{
		m_Lock.Unlock();
		return NULL;
	}

	// ȡ����Ϣ
	LPBYTE pMsg = m_pBegin->byData;
	dwMsgSize = m_pBegin->dwDataSize;

	Msg* pMsgItem = m_pBegin;
	m_pBegin = m_pBegin->pNext;

	--m_nMsg;
	m_Lock.Unlock();


	return pMsg;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
VOID MsgQueueTS::FreeMsg(LPVOID pMsg)
{
	Msg* pReal = CONTAINING_RECORD(pMsg, Msg, byData);
	MCFREE(pReal);
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
VOID MsgQueueTS::Clear()
{
	LPBYTE pMsg = NULL;
	DWORD dwSize = 0;
	while( pMsg = GetMsg(dwSize) )
		FreeMsg(pMsg);
}

}