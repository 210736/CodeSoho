//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2003-11-30
// Last: 2008-06-12
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {
//-----------------------------------------------------------------------------
// ȡ�����ڲ�������ڴ�,��߸����ͷ�
//-----------------------------------------------------------------------------
class ECORE_API MsgQueue
{
public:
	INLINE BOOL		AddMsg(LPCVOID pMsg, DWORD dwSize);
	INLINE BOOL		AddMsgToFront(LPCVOID pMsg, DWORD dwSize);
	INLINE LPBYTE	GetMsg(DWORD& dwMsgSize);
	INLINE VOID		FreeMsg(LPVOID pMsg);
	
	INLINE VOID		Clear();	
	INLINE INT		GetMsgNum()	{ return m_nMsg; }

	//-----------------------------------------------------------------------------
	// �������
	//-----------------------------------------------------------------------------
	// ���ڲ�ָ��ָ�����ͷ
	INLINE VOID		ResetIterator();
	// ���ڲ�ָ�����
	INLINE VOID		IncIterator();
	// ��õ�ǰ�ڲ�ָ��ָ����Ϣ��������ȡ��
	INLINE LPBYTE	PeekMsg(DWORD& dwMsgSize);
	// �Ƴ���ǰ�ڲ�ָ��ָ����Ϣ��������ָ��
	INLINE VOID		RemoveMsg();
	
	MsgQueue():m_nMsg(0), m_pBegin(0), m_pEnd(0), m_pCurrent(0) {}
	~MsgQueue() { Clear(); }


private:

#pragma warning(push)
#pragma warning(disable: 4200)

	struct Msg
	{
		Msg*	pPrev;
		Msg*	pNext;
		DWORD	dwDataSize;
		BYTE	byData[0];
	};

#pragma warning(pop)

	Msg*	m_pBegin;
	Msg*	m_pEnd;
	Msg*	m_pCurrent;
	INT		m_nMsg;
};



//-----------------------------------------------------------------------------
// �����Ϣ������β�������Ƿ���ӳɹ�
//-----------------------------------------------------------------------------
BOOL MsgQueue::AddMsg(LPCVOID pMsg, DWORD dwSize)
{
	// �����µĵ�Ԫ
	Msg* pNewMsg = (Msg*)MCALLOC(sizeof(Msg)+dwSize);
	if( !pNewMsg )
		return FALSE;

	// ��ʼ��
	pNewMsg->pPrev = NULL;
	pNewMsg->pNext = NULL;
	pNewMsg->dwDataSize = dwSize;


	// ��������
	memcpy(pNewMsg->byData, pMsg, dwSize);

	if( !m_pBegin )	// ���п�
	{
		m_pBegin	=	pNewMsg;
		m_pEnd		=	pNewMsg;
		m_pCurrent	=	pNewMsg;
	}
	else
	{
		m_pEnd->pNext	=	pNewMsg;
		pNewMsg->pPrev	=	m_pEnd;
		m_pEnd			=	pNewMsg;

	}

	++m_nMsg;
	return TRUE;
}

//-----------------------------------------------------------------------------
// �����Ϣ�������ף������Ƿ���ӳɹ�
//-----------------------------------------------------------------------------
INLINE BOOL MsgQueue::AddMsgToFront( LPCVOID pMsg, DWORD dwSize )
{
	// �����µĵ�Ԫ
	Msg* pNewMsg = (Msg*)MCALLOC(sizeof(Msg) + dwSize);
	if( !pNewMsg )
		return FALSE;

	// ��ʼ��
	pNewMsg->pPrev = NULL;
	pNewMsg->pNext = NULL;
	pNewMsg->dwDataSize = dwSize;

	// ��������
	memcpy(pNewMsg->byData, pMsg, dwSize);

	if( !m_pBegin )	// ���п�
	{
		m_pBegin	=	pNewMsg;
		m_pEnd		=	pNewMsg;
		m_pCurrent	=	pNewMsg;
	}
	else
	{
		pNewMsg->pNext	=	m_pBegin;

		m_pBegin->pPrev =	pNewMsg;
		m_pBegin		=	pNewMsg;
	}

	++m_nMsg;
	return TRUE;
}


//-----------------------------------------------------------------------------
// �Ӷ���ͷȡ��Ϣ��������Ϣ����
//-----------------------------------------------------------------------------
LPBYTE MsgQueue::GetMsg(DWORD &dwMsgSize)
{
	dwMsgSize = 0;
	if( m_nMsg <= 0 )
		return NULL;	// no msg

	// ȡ����Ϣ
	LPBYTE pMsg = (LPBYTE)m_pBegin->byData;
	dwMsgSize = m_pBegin->dwDataSize;

	Msg* pItem = m_pBegin;
	m_pBegin = m_pBegin->pNext;
	if( m_pBegin )
		m_pBegin->pPrev = NULL;

	// �����ǰָ��ָ��ͷ���������õ�ǰָ��Ϊ�µ�ͷ��
	if( pItem == m_pCurrent )
		m_pCurrent = m_pBegin;

	--m_nMsg;
	if( 0 == m_nMsg )
	{
		m_pBegin = m_pEnd = m_pCurrent = NULL;
	}

	return pMsg;
}


//-----------------------------------------------------------------------------
// ��Ӧ��GetMsg(),��Ϊ�ڴ����ڲ�����ģ�����Ӧ�ý����ڲ����ͷ�
//-----------------------------------------------------------------------------
VOID MsgQueue::FreeMsg(LPVOID pMsg) 
{
	Msg* pReal = CONTAINING_RECORD(pMsg, Msg, byData);
	MCFREE(pReal);
}


//-----------------------------------------------------------------------------
// ���ڲ�ָ��ָ�����ͷ
//-----------------------------------------------------------------------------
VOID MsgQueue::ResetIterator()
{
	m_pCurrent = m_pBegin;
}


//-----------------------------------------------------------------------------
// ���ڲ�ָ�����
//-----------------------------------------------------------------------------
VOID MsgQueue::IncIterator()
{
	if( !m_pCurrent )
		return;

	m_pCurrent = m_pCurrent->pNext;
}


//-----------------------------------------------------------------------------
// ��õ�ǰ�ڲ�ָ��ָ����Ϣ��������ȡ��
//-----------------------------------------------------------------------------
LPBYTE MsgQueue::PeekMsg(DWORD& dwMsgSize)
{
	dwMsgSize = 0;
	if( !m_pCurrent || m_nMsg <= 0 )
		return NULL;

	dwMsgSize = m_pCurrent->dwDataSize;
	return m_pCurrent->byData;
}


//-----------------------------------------------------------------------------
// �Ƴ���ǰ�ڲ�ָ��ָ����Ϣ��������ָ��
//-----------------------------------------------------------------------------
VOID MsgQueue::RemoveMsg()
{
	if( !m_pCurrent )
		return;

	Msg* pItem = m_pCurrent;
	if( m_pCurrent == m_pBegin )
	{
		m_pBegin = m_pBegin->pNext;
		if( m_pBegin )
			m_pBegin->pPrev = NULL;
		m_pCurrent = m_pBegin;

	}
	else if( m_pCurrent == m_pEnd )
	{
		m_pEnd = m_pEnd->pPrev;
		if( m_pEnd )
			m_pEnd->pNext = NULL;

		m_pCurrent = NULL;
	}
	else
	{
		m_pCurrent->pPrev->pNext = m_pCurrent->pNext;
		m_pCurrent->pNext->pPrev = m_pCurrent->pPrev;
		m_pCurrent = m_pCurrent->pNext;
	}

	MCFREE(pItem);
	--m_nMsg;
	if( 0 == m_nMsg )
	{
		m_pBegin = m_pEnd = m_pCurrent = NULL;
	}
}

//-----------------------------------------------------------------------------
// ���������Ϣ
//-----------------------------------------------------------------------------
VOID MsgQueue::Clear()
{
	Msg* pMsg = m_pBegin; 
	while( m_pBegin )
	{
		pMsg = m_pBegin->pNext;
		MCFREE(m_pBegin);
		m_pBegin = pMsg;
	}
	m_nMsg = 0;
	m_pBegin = NULL;
	m_pEnd = NULL;
	m_pCurrent = NULL;
}

}