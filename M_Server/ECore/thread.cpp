//---------------------------------------------------------------------------------
// File: thread.cpp
// Desc: �̶߳���������ִ�������ִ���壬ά���̵߳ľ�����߳�ID��������һЩͳ����Ϣ
// Auth: Aslan
// Date: 2009-12-09
// Last: 2009-12-09
//---------------------------------------------------------------------------------
#include "stdafx.h"
#include "thread.h"
#include "thread_mgr.h"
#include "executor.h"

namespace ECore{

//-----------------------------------------------------------------------------------
// ���캯��
//-----------------------------------------------------------------------------------
Thread::Thread(Executor* pExecutor, ThreadMgr* pMgr, BOOL bDestoryAtEnd, BOOL bSuspended)
		: m_pExecutor(pExecutor), m_pMgr(pMgr), m_hHandle(0), m_dwThreadID((DWORD)INVALID),
		  m_dwID((DWORD)INVALID), m_bDestryAtEnd(bDestoryAtEnd), m_bSuspended(bSuspended)
{
	ASSERT(VALID(pExecutor));
}

//------------------------------------------------------------------------------------
// ��������
//------------------------------------------------------------------------------------
Thread::~Thread()
{
	if( m_bDestryAtEnd && VALID(m_pExecutor) )
	{
		SAFE_DEL(m_pExecutor);
	}
}

//-------------------------------------------------------------------------------------
// ִֹͣ��
//-------------------------------------------------------------------------------------
VOID Thread::Stop()
{
	if( VALID(m_pExecutor) )
	{
		m_pExecutor->OnShutDown();
	}
}

} // namespace ECore{