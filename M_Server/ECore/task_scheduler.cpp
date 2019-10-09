//--------------------------------------------------------------------------------
// File: task_scheduler.cpp
// Desc: 任务调度器，外部负责提交和删除任务，调度器在每次更新时会将其任务列表中的
//       所有任务通过完成端口发送完成通知，内部的线程管理器会得到完成端口通知执行
//       每一个任务，任务调度器直到所有的任务都执行完成后再返回
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
// 构造函数
//---------------------------------------------------------------------------------
TaskScheduler::TaskScheduler() : m_hCompletionPort(NULL), m_nWorkerNum(NULL), m_Condition(FALSE), m_nCurTaskLeft(0)
{

}

//---------------------------------------------------------------------------------
// 析构函数
//---------------------------------------------------------------------------------
TaskScheduler::~TaskScheduler()
{

}

//---------------------------------------------------------------------------------
// 初始化
//---------------------------------------------------------------------------------
BOOL TaskScheduler::Init(INT nWorkerNum/* =0 */)
{
	// 得到worker线程的数量
	m_nWorkerNum = nWorkerNum;
	if( m_nWorkerNum <= 0 )	
		m_nWorkerNum = 8;

	// 创建完成端口
	 m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	 // 根据worker线程的数量线程执行体
	 for(INT n = 0; n < m_nWorkerNum; ++n)
	 {
		 if( INVALID == m_ThreadMgr.CreateThread(new ExecutorTask(m_hCompletionPort, this, 0), TRUE) )
			 return FALSE;
	 }

	 return TRUE;
}

//-----------------------------------------------------------------------------------
// 销毁
//-----------------------------------------------------------------------------------
VOID TaskScheduler::Destory()
{
	// 删除任务队列
	m_setTask.clear();

	// 根据worker线程个数发送等同数量的结束完成通知
	for(INT n = 0; n < m_nWorkerNum; ++n)
	{
		PostQueuedCompletionStatus(m_hCompletionPort, 0, (ULONG_PTR)INVALID_POINTER, NULL);
	}

	// 关闭线程池
	m_ThreadMgr.ShutDown();

	// 关闭完成端口
	CloseHandle(m_hCompletionPort);

}

//------------------------------------------------------------------------------------
// 更新
//------------------------------------------------------------------------------------
VOID TaskScheduler::Update()
{
	// 进入工作
	m_WorkMutex.Lock();

	// 得到现在列表里的任务个数，设置任务等待完成个数
	INT nSize = (INT)m_setTask.size();
	InterlockedExchangeEx((LPLONG)&m_nCurTaskLeft, nSize); 

	if (m_nCurTaskLeft <= 0)
	{
		m_WorkMutex.Unlock();
		return;
	}

	// 得到每个任务，发送完成通知
	Task* pTask	= NULL;
	std::set<Task*>::iterator it;
	std::set<Task*>::iterator itEnd = m_setTask.end();

	for(it = m_setTask.begin(); it != itEnd; ++it)
	{
		pTask = *it;
		PostQueuedCompletionStatus(m_hCompletionPort, 0, (ULONG_PTR)pTask, NULL);
	}

	// 离开工作
	m_WorkMutex.Unlock();

	// 等待所有任务完成
	m_Condition.Wait();
}

//------------------------------------------------------------------------------------
// 增加一个任务
//------------------------------------------------------------------------------------
VOID TaskScheduler::AddTask(Task* pTask)
{
	if( !VALID(pTask) ) return;

	m_WorkMutex.Lock();

	m_setTask.insert(pTask);

	m_WorkMutex.Unlock();
}

//-------------------------------------------------------------------------------------
// 减少一个任务
//-------------------------------------------------------------------------------------
VOID TaskScheduler::RemoveTask(Task* pTask)
{
	if( !VALID(pTask) ) return;

	m_WorkMutex.Lock();

	m_setTask.erase(pTask);

	m_WorkMutex.Unlock();
}

//-------------------------------------------------------------------------------------
// 得到所有worker线程的系统分配线程ID
//-------------------------------------------------------------------------------------
VOID TaskScheduler::GetAllWorkerThreadSystemID(TList<DWORD>& listID)
{
	m_ThreadMgr.GetAlThreadSystemID(listID);
}

} // namespace ECore{