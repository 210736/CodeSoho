//-------------------------------------------------------------------------------------
// File: thread_mgr.cpp
// Desc: �̹߳������������������̵߳Ĵ�����ɾ�������ṩ��ѯ����
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
// ���캯��
//--------------------------------------------------------------------------------------
ThreadMgr::ThreadMgr() : m_dwIDGen(0)
{

}

//--------------------------------------------------------------------------------------
// �ȴ�ĳ���߳�ִ����ϲ�ɾ����
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
// ĳ���߳̽���ʱ����
//--------------------------------------------------------------------------------------
VOID ThreadMgr::ThreadStopped(DWORD dwID)
{
	m_Mutex.Lock();
	Thread* pThread = m_mapThread.Peek(dwID);
	m_mapThread.Erase(dwID);
	m_Mutex.Unlock();

	// ��Ϊ�̺߳�����û��ֹͣ�����Դ˴�ֱ��ɾ��Handle����Ҫwait��ɾ��֮���̺߳���Ҳ����ֹ���߳������˳�
	if( VALID(pThread) )
	{
		SAFE_CLOSE_HANDLE(pThread->m_hHandle);
		SAFE_DEL(pThread);
	}
}

//--------------------------------------------------------------------------------------
// ����һ��IDֹͣһ���̣߳�һ�������ⲿ����
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
// �ر��̹߳�����
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
// ĳ���߳��Ƿ񽡿�
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
// ͨ���̺߳���
//--------------------------------------------------------------------------------------
static unsigned int WINAPI ThreadProc(void* pParam)
{
	// ���������ͳ��߳�ָ��
	Thread * pThread = (Thread*)pParam;
	DWORD dwThreadID = pThread->GetID();

	ThreadMgr* pMgr = pThread->GetMgr();
	ASSERT(VALID(pMgr));

	// ��ʼ�����������
	srand(dwThreadID);

	// ��ʼִ��
	pThread->GetExecutor()->Run();


	pMgr->ThreadStopped(dwThreadID);

	// ִ����ThreadStopped��pThread�᲻���ڣ���Ҫ�ٵ����ˣ���������

	return 0;
}

//--------------------------------------------------------------------------------------
// �����߳�
//--------------------------------------------------------------------------------------
DWORD ThreadMgr::CreateThread(Executor* pExecutor, BOOL bDestroyAtEnd, BOOL bInitSuspend)
{
	if( !VALID(pExecutor) ) return INVALID;

	// �����̶߳���
	Thread* pThread = new Thread(pExecutor, this, bDestroyAtEnd, bInitSuspend);

	// �õ���ʼ������
	DWORD dwInitFlag = 0;
	if( bInitSuspend ) dwInitFlag = CREATE_SUSPENDED;

	// �����߳�
	DWORD dwThreadID = 0;
	HANDLE hHandle = (HANDLE)_beginthreadex(NULL, 0, &ThreadProc, (LPVOID)pThread, dwInitFlag, (unsigned int*)&dwThreadID);

	if( 0 == hHandle )
	{
		SAFE_DEL(pThread);
		return INVALID;
	}

	// ���ý��������Ϣ
	pThread->m_hHandle		=	hHandle;
	pThread->m_dwThreadID	=	dwThreadID;

	// ����һ���߳�ID
	m_Mutex.Lock();
	pThread->m_dwID	= ++m_dwIDGen;
	while( m_mapThread.IsExist(pThread->m_dwID) )
	{
		pThread->m_dwID = ++m_dwIDGen;
	}

	// �����߳��б���
	m_mapThread.Add(pThread->m_dwID, pThread);

	// �õ��߳�ID
	DWORD dwID = pThread->m_dwID;

	m_Mutex.Unlock();

	return dwID;
}

//-----------------------------------------------------------------------------------------
// ����һ���߳�
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
// �ָ�һ���߳�
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
// �õ�ĳ���̵߳�ϵͳ�����߳�ID
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
// �õ������̵߳�ϵͳ�����߳�ID
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