//-------------------------------------------------------------------------------------
// File: thread_mgr.cpp
// Desc: 线程管理器，它负责所有线程的创建和删除，并提供查询功能
// Auth: Aslan
// Date: 2009-12-09
// Last: 2009-12-09
//-------------------------------------------------------------------------------------
#include "stdafx.h"
#include "thread.h"
#include "executor.h"
#include "thread_mgr.h"

namespace ECore{

//--------------------------------------------------------------------------------------
// 构造函数
//--------------------------------------------------------------------------------------
ThreadMgr::ThreadMgr() : m_dwIDGen(0)
{

}

//--------------------------------------------------------------------------------------
// 等待某个线程执行完毕并删除它
//--------------------------------------------------------------------------------------
VOID ThreadMgr::WaitAndDestroyThread(Thread* pThread)
{
	if( !VALID(pThread) ) return;

	if( pThread->m_bSuspended )
	{
		::ResumeThread(pThread->m_hHandle);
	}

	::WaitForSingleObject(pThread->m_hHandle, INFINITE);
	SAFE_CLOSE_HANDLE(pThread->m_hHandle);
	SAFE_DEL(pThread);
}

//--------------------------------------------------------------------------------------
// 某个线程结束时调用
//--------------------------------------------------------------------------------------
VOID ThreadMgr::ThreadStopped(DWORD dwID)
{
	m_Mutex.Lock();
	Thread* pThread = m_mapThread.Peek(dwID);
	m_mapThread.Erase(dwID);
	m_Mutex.Unlock();

	// 因为线程函数还没有停止，所以此处直接删除Handle，不要wait，删除之后，线程函数也会终止，线程完整退出
	if( VALID(pThread) )
	{
		SAFE_CLOSE_HANDLE(pThread->m_hHandle);
		SAFE_DEL(pThread);
	}
}

//--------------------------------------------------------------------------------------
// 根据一个ID停止一个线程，一般用于外部调用
//--------------------------------------------------------------------------------------
VOID ThreadMgr::StopThread(DWORD dwID, BOOL bWait)
{
	if( !bWait )
	{
		m_Mutex.Lock();
		Thread* pThread = m_mapThread.Peek(dwID);
		if( VALID(pThread) )
			pThread->Stop();
		m_Mutex.Unlock();
	}
	else
	{
		m_Mutex.Lock();
		Thread* pThread = m_mapThread.Peek(dwID);
		m_mapThread.Erase(dwID);
		m_Mutex.Unlock();

		if( VALID(pThread) )
		{
			pThread->Stop();
			WaitAndDestroyThread(pThread);
		}
	}
}

//--------------------------------------------------------------------------------------
// 关闭线程管理器
//--------------------------------------------------------------------------------------
VOID ThreadMgr::ShutDown()
{
	if( m_mapThread.Empty() ) return;

	std::list<DWORD> listThread;

	m_Mutex.Lock();
	m_mapThread.ExportAllKey(listThread);
	m_Mutex.Unlock();

	std::list<DWORD>::iterator it;
	for(it = listThread.begin(); it != listThread.end(); ++it)
	{
		DWORD dwThreadID = *it;

		StopThread(dwThreadID, TRUE);
	}

	ASSERT(m_mapThread.Empty());
}

//--------------------------------------------------------------------------------------
// 某个线程是否健康
//--------------------------------------------------------------------------------------
BOOL ThreadMgr::IsThreadActive(DWORD dwID)
{
	m_Mutex.Lock();

	BOOL bRet = TRUE;

	Thread* pThread = m_mapThread.Peek(dwID);
	if( !VALID(pThread) )
	{
		bRet = FALSE;
	}
	else
	{
		DWORD dwResult = ::WaitForSingleObject(pThread->m_hHandle, 0);
		switch(dwResult)
		{
		case WAIT_OBJECT_0:
			bRet = FALSE;
			break;

		case WAIT_TIMEOUT:
			bRet = TRUE;
			break;

		default:
			bRet = FALSE;
			break;
		}
	}

	m_Mutex.Unlock();

	return bRet;
}

//--------------------------------------------------------------------------------------
// 通用线程函数
//--------------------------------------------------------------------------------------
static unsigned int WINAPI ThreadProc(void* pParam)
{
	// 将参数解释成线程指针
	Thread * pThread = (Thread*)pParam;
	DWORD dwThreadID = pThread->GetID();

	ThreadMgr* pMgr = pThread->GetMgr();
	ASSERT(VALID(pMgr));

	// 初始化随机数种子
	srand(dwThreadID);

	// 开始执行
	pThread->GetExecutor()->Run();


	pMgr->ThreadStopped(dwThreadID);

	// 执行完ThreadStopped后，pThread会不存在，不要再调用了！！！！！

	return 0;
}

//--------------------------------------------------------------------------------------
// 创建线程
//--------------------------------------------------------------------------------------
DWORD ThreadMgr::CreateThread(Executor* pExecutor, BOOL bDestroyAtEnd, BOOL bInitSuspend)
{
	if( !VALID(pExecutor) ) return INVALID;

	// 创建线程对象
	Thread* pThread = new Thread(pExecutor, this, bDestroyAtEnd, bInitSuspend);

	// 得到初始化参数
	DWORD dwInitFlag = 0;
	if( bInitSuspend ) dwInitFlag = CREATE_SUSPENDED;

	// 创建线程
	DWORD dwThreadID = 0;
	HANDLE hHandle = (HANDLE)_beginthreadex(NULL, 0, &ThreadProc, (LPVOID)pThread, dwInitFlag, (unsigned int*)&dwThreadID);

	if( 0 == hHandle )
	{
		SAFE_DEL(pThread);
		return INVALID;
	}

	// 设置进程相关信息
	pThread->m_hHandle		=	hHandle;
	pThread->m_dwThreadID	=	dwThreadID;

	// 分配一个线程ID
	m_Mutex.Lock();
	pThread->m_dwID	= ++m_dwIDGen;
	while( m_mapThread.IsExist(pThread->m_dwID) )
	{
		pThread->m_dwID = ++m_dwIDGen;
	}

	// 加入线程列表中
	m_mapThread.Add(pThread->m_dwID, pThread);

	// 得到线程ID
	DWORD dwID = pThread->m_dwID;

	m_Mutex.Unlock();

	return dwID;
}

//-----------------------------------------------------------------------------------------
// 挂起一个线程
//-----------------------------------------------------------------------------------------
VOID ThreadMgr::SuspendOneThread(DWORD dwID)
{
	m_Mutex.Lock();

	Thread* pThread = m_mapThread.Peek(dwID);
	if( VALID(pThread) && !pThread->m_bSuspended )
	{
		SuspendThread(pThread->m_hHandle);
		pThread->m_bSuspended = TRUE;
	}

	m_Mutex.Unlock();
}

//------------------------------------------------------------------------------------------
// 恢复一个线程
//------------------------------------------------------------------------------------------
VOID ThreadMgr::ResumeOneThread(DWORD dwID)
{
	m_Mutex.Lock();

	Thread* pThread = m_mapThread.Peek(dwID);
	if( VALID(pThread) && pThread->m_bSuspended )
	{
		ResumeThread(pThread->m_hHandle);
		pThread->m_bSuspended = FALSE;
	}

	m_Mutex.Unlock();
}

//-------------------------------------------------------------------------------------------
// 得到某个线程的系统分配线程ID
//-------------------------------------------------------------------------------------------
DWORD ThreadMgr::GetOneThreadSystemID(DWORD dwID)
{
	DWORD dwSystemThreadID = INVALID;

	m_Mutex.Lock();
	Thread* pThread = m_mapThread.Peek(dwID);
	if( VALID(pThread) )
	{
		dwSystemThreadID = pThread->m_dwThreadID;
	}
	m_Mutex.Unlock();

	return dwSystemThreadID;
}

//---------------------------------------------------------------------------------------------
// 得到所有线程的系统分配线程ID
//---------------------------------------------------------------------------------------------
VOID ThreadMgr::GetAlThreadSystemID(TList<DWORD>& listID)
{
	m_Mutex.Lock();

	Thread* pThread = NULL;
	TMap<DWORD, Thread*>::Iterator it = m_mapThread.Begin();
	while( m_mapThread.PeekNext(it, pThread) )
	{
		listID.PushBack(pThread->m_dwThreadID);
	}

	m_Mutex.Unlock();
}


} // namespace ECore{