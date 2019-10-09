//--------------------------------------------------------------------------------
// File: task_scheduler.cpp
// Desc: ������������ⲿ�����ύ��ɾ�����񣬵�������ÿ�θ���ʱ�Ὣ�������б��е�
//       ��������ͨ����ɶ˿ڷ������֪ͨ���ڲ����̹߳�������õ���ɶ˿�ִ֪ͨ��
//       ÿһ���������������ֱ�����е�����ִ����ɺ��ٷ���
// Auth: Aslan
// Date: 2009-12-09
// Last: 2009-12-09
//---------------------------------------------------------------------------------
#include "stdafx.h"
#include "task.h"
#include "executor.h"
#include "executor_task.h"
#include "task_scheduler.h"
#include "thread_mgr.h"

namespace ECore{

//---------------------------------------------------------------------------------
// ���캯��
//---------------------------------------------------------------------------------
TaskScheduler::TaskScheduler() : m_hCompletionPort(NULL), m_nWorkerNum(NULL), m_Condition(FALSE), m_nCurTaskLeft(0)
{

}

//---------------------------------------------------------------------------------
// ��������
//---------------------------------------------------------------------------------
TaskScheduler::~TaskScheduler()
{

}

//---------------------------------------------------------------------------------
// ��ʼ��
//---------------------------------------------------------------------------------
BOOL TaskScheduler::Init(INT nWorkerNum/* =0 */)
{
	// �õ�worker�̵߳�����
	m_nWorkerNum = nWorkerNum;
	if( m_nWorkerNum <= 0 )	
		m_nWorkerNum = 8;

	// ������ɶ˿�
	 m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	 // ����worker�̵߳������߳�ִ����
	 for(INT n = 0; n < m_nWorkerNum; ++n)
	 {
		 if( INVALID == m_ThreadMgr.CreateThread(new ExecutorTask(m_hCompletionPort, this, 0), TRUE) )
			 return FALSE;
	 }

	 return TRUE;
}

//-----------------------------------------------------------------------------------
// ����
//-----------------------------------------------------------------------------------
VOID TaskScheduler::Destory()
{
	// ɾ���������
	m_setTask.clear();

	// ����worker�̸߳������͵�ͬ�����Ľ������֪ͨ
	for(INT n = 0; n < m_nWorkerNum; ++n)
	{
		PostQueuedCompletionStatus(m_hCompletionPort, 0, (ULONG_PTR)INVALID_POINTER, NULL);
	}

	// �ر��̳߳�
	m_ThreadMgr.ShutDown();

	// �ر���ɶ˿�
	CloseHandle(m_hCompletionPort);

}

//------------------------------------------------------------------------------------
// ����
//------------------------------------------------------------------------------------
VOID TaskScheduler::Update()
{
	// ���빤��
	m_WorkMutex.Lock();

	// �õ������б���������������������ȴ���ɸ���
	INT nSize = (INT)m_setTask.size();
	InterlockedExchangeEx((LPLONG)&m_nCurTaskLeft, nSize); 

	if (m_nCurTaskLeft <= 0)
	{
		m_WorkMutex.Unlock();
		return;
	}

	// �õ�ÿ�����񣬷������֪ͨ
	Task* pTask	= NULL;
	std::set<Task*>::iterator it;
	std::set<Task*>::iterator itEnd = m_setTask.end();

	for(it = m_setTask.begin(); it != itEnd; ++it)
	{
		pTask = *it;
		PostQueuedCompletionStatus(m_hCompletionPort, 0, (ULONG_PTR)pTask, NULL);
	}

	// �뿪����
	m_WorkMutex.Unlock();

	// �ȴ������������
	m_Condition.Wait();
}

//------------------------------------------------------------------------------------
// ����һ������
//------------------------------------------------------------------------------------
VOID TaskScheduler::AddTask(Task* pTask)
{
	if( !VALID(pTask) ) return;

	m_WorkMutex.Lock();

	m_setTask.insert(pTask);

	m_WorkMutex.Unlock();
}

//-------------------------------------------------------------------------------------
// ����һ������
//-------------------------------------------------------------------------------------
VOID TaskScheduler::RemoveTask(Task* pTask)
{
	if( !VALID(pTask) ) return;

	m_WorkMutex.Lock();

	m_setTask.erase(pTask);

	m_WorkMutex.Unlock();
}

//-------------------------------------------------------------------------------------
// �õ�����worker�̵߳�ϵͳ�����߳�ID
//-------------------------------------------------------------------------------------
VOID TaskScheduler::GetAllWorkerThreadSystemID(TList<DWORD>& listID)
{
	m_ThreadMgr.GetAlThreadSystemID(listID);
}

} // namespace ECore{