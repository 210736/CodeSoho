//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2003-11-30
// Last: 2008-06-12
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {
//-----------------------------------------------------------------------------
// 取的是内部分配的内存,外边负责释放
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
	// 特殊操作
	//-----------------------------------------------------------------------------
	// 将内部指针指向队列头
	INLINE VOID		ResetIterator();
	// 将内部指针后移
	INLINE VOID		IncIterator();
	// 获得当前内部指针指向消息，但并不取出
	INLINE LPBYTE	PeekMsg(DWORD& dwMsgSize);
	// 移除当前内部指针指向消息，并后移指针
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
// 添加消息到队列尾，返回是否添加成功
//-----------------------------------------------------------------------------
BOOL MsgQueue::AddMsg(LPCVOID pMsg, DWORD dwSize)
{
	// 申请新的单元
	Msg* pNewMsg = (Msg*)MCALLOC(sizeof(Msg)+dwSize);
	if( !pNewMsg )
		return FALSE;

	// 初始化
	pNewMsg->pPrev = NULL;
	pNewMsg->pNext = NULL;
	pNewMsg->dwDataSize = dwSize;


	// 拷贝内容
	memcpy(pNewMsg->byData, pMsg, dwSize);

	if( !m_pBegin )	// 队列空
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
// 添加消息到队列首，返回是否添加成功
//-----------------------------------------------------------------------------
INLINE BOOL MsgQueue::AddMsgToFront( LPCVOID pMsg, DWORD dwSize )
{
	// 申请新的单元
	Msg* pNewMsg = (Msg*)MCALLOC(sizeof(Msg) + dwSize);
	if( !pNewMsg )
		return FALSE;

	// 初始化
	pNewMsg->pPrev = NULL;
	pNewMsg->pNext = NULL;
	pNewMsg->dwDataSize = dwSize;

	// 拷贝内容
	memcpy(pNewMsg->byData, pMsg, dwSize);

	if( !m_pBegin )	// 队列空
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
// 从队列头取消息，返回消息长度
//-----------------------------------------------------------------------------
LPBYTE MsgQueue::GetMsg(DWORD &dwMsgSize)
{
	dwMsgSize = 0;
	if( m_nMsg <= 0 )
		return NULL;	// no msg

	// 取出消息
	LPBYTE pMsg = (LPBYTE)m_pBegin->byData;
	dwMsgSize = m_pBegin->dwDataSize;

	Msg* pItem = m_pBegin;
	m_pBegin = m_pBegin->pNext;
	if( m_pBegin )
		m_pBegin->pPrev = NULL;

	// 如果当前指针指向头部，则设置当前指针为新的头部
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
// 对应于GetMsg(),因为内存是内部分配的，所以应该交还内部来释放
//-----------------------------------------------------------------------------
VOID MsgQueue::FreeMsg(LPVOID pMsg) 
{
	Msg* pReal = CONTAINING_RECORD(pMsg, Msg, byData);
	MCFREE(pReal);
}


//-----------------------------------------------------------------------------
// 将内部指针指向队列头
//-----------------------------------------------------------------------------
VOID MsgQueue::ResetIterator()
{
	m_pCurrent = m_pBegin;
}


//-----------------------------------------------------------------------------
// 将内部指针后移
//-----------------------------------------------------------------------------
VOID MsgQueue::IncIterator()
{
	if( !m_pCurrent )
		return;

	m_pCurrent = m_pCurrent->pNext;
}


//-----------------------------------------------------------------------------
// 获得当前内部指针指向消息，但并不取出
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
// 移除当前内部指针指向消息，并后移指针
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
// 清除所有消息
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