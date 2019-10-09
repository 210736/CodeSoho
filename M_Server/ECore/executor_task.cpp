//-----------------------------------------------------------------------------
// File: executor_task.cpp
// Desc: 基于任务的线程执行体，该执行体与某个初始化好的完成端口绑定，进行Run的时候
//       任务执行体等待完成端口的完成通知，并将该通知解释为某个任务来进行执行，然后
//       继续等待，直到得到返回的完成通知即可
// Auth: Aslan
// Date: 2009-12-09
// Last: 2009-12-09
//-----------------------------------------------------------------------------
#pragma once

#include "stdafx.h"
#include "task.h"
#include "task_scheduler.h"
#include "executor_task.h"

namespace ECore{

//-----------------------------------------------------------------------------
// 构造函数和析构函数
//-----------------------------------------------------------------------------
ExecutorTask::ExecutorTask(HANDLE hCompletionPort, TaskScheduler* pScheduler, LPVOID nIndex) 
	: Executor(), m_hCompletionPort(hCompletionPort), m_pScheduler(pScheduler)
{
}

ExecutorTask::~ExecutorTask()
{
}

//-----------------------------------------------------------------------------
// 执行接口
//-----------------------------------------------------------------------------
VOID ExecutorTask::Run()
{
	DWORD			dwDummy		=	0;		// 完成字节数
	Task*			pTask		=	NULL;	// 任务
	LPOVERLAPPED	pOverlapped	=	NULL;	// 重叠IO

	// 查询完成端口的状态
	INT nRet = GetQueuedCompletionStatus(m_hCompletionPort, &dwDummy, (PULONG_PTR)&pTask, &pOverlapped, INFINITE);

	// 如果等待失败或者得到的pTask是个非法指针，则退出
	while( nRet && VALID(pTask) )
	{
		// 执行任务
		pTask->Do();

		// 通知完成
		m_pScheduler->OneTaskDone();

		// 再次得到完成端口的通知
		nRet = GetQueuedCompletionStatus(m_hCompletionPort, &dwDummy, (PULONG_PTR)&pTask, &pOverlapped, INFINITE);
	}

}

//-----------------------------------------------------------------------------
// 关闭回调接口
//-----------------------------------------------------------------------------
VOID ExecutorTask::OnShutDown()
{
	
}

} // namespace ECore{