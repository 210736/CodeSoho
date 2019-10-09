//---------------------------------------------------------------------------------
// File: thread.cpp
// Desc: 线程对象，它负责执行里面的执行体，维护线程的句柄和线程ID，并加入一些统计信息
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
// 构造函数
//-----------------------------------------------------------------------------------
Thread::Thread(Executor* pExecutor, ThreadMgr* pMgr, BOOL bDestoryAtEnd, BOOL bSuspended)
		: m_pExecutor(pExecutor), m_pMgr(pMgr), m_hHandle(0), m_dwThreadID((DWORD)INVALID),
		  m_dwID((DWORD)INVALID), m_bDestryAtEnd(bDestoryAtEnd), m_bSuspended(bSuspended)
{
	ASSERT(VALID(pExecutor));
}

//------------------------------------------------------------------------------------
// 析构函数
//------------------------------------------------------------------------------------
Thread::~Thread()
{
	if( m_bDestryAtEnd && VALID(m_pExecutor) )
	{
		SAFE_DEL(m_pExecutor);
	}
}

//-------------------------------------------------------------------------------------
// 停止执行
//-------------------------------------------------------------------------------------
VOID Thread::Stop()
{
	if( VALID(m_pExecutor) )
	{
		m_pExecutor->OnShutDown();
	}
}

} // namespace ECore{