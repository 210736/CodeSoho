//-----------------------------------------------------------------------------
// File: executor_task.cpp
// Desc: ����������߳�ִ���壬��ִ������ĳ����ʼ���õ���ɶ˿ڰ󶨣�����Run��ʱ��
//       ����ִ����ȴ���ɶ˿ڵ����֪ͨ��������֪ͨ����Ϊĳ������������ִ�У�Ȼ��
//       �����ȴ���ֱ���õ����ص����֪ͨ����
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
// ���캯������������
//-----------------------------------------------------------------------------
ExecutorTask::ExecutorTask(HANDLE hCompletionPort, TaskScheduler* pScheduler, LPVOID nIndex) 
	: Executor(), m_hCompletionPort(hCompletionPort), m_pScheduler(pScheduler)
{
}

ExecutorTask::~ExecutorTask()
{
}

//-----------------------------------------------------------------------------
// ִ�нӿ�
//-----------------------------------------------------------------------------
VOID ExecutorTask::Run()
{
	DWORD			dwDummy		=	0;		// ����ֽ���
	Task*			pTask		=	NULL;	// ����
	LPOVERLAPPED	pOverlapped	=	NULL;	// �ص�IO

	// ��ѯ��ɶ˿ڵ�״̬
	INT nRet = GetQueuedCompletionStatus(m_hCompletionPort, &dwDummy, (PULONG_PTR)&pTask, &pOverlapped, INFINITE);

	// ����ȴ�ʧ�ܻ��ߵõ���pTask�Ǹ��Ƿ�ָ�룬���˳�
	while( nRet && VALID(pTask) )
	{
		// ִ������
		pTask->Do();

		// ֪ͨ���
		m_pScheduler->OneTaskDone();

		// �ٴεõ���ɶ˿ڵ�֪ͨ
		nRet = GetQueuedCompletionStatus(m_hCompletionPort, &dwDummy, (PULONG_PTR)&pTask, &pOverlapped, INFINITE);
	}

}

//-----------------------------------------------------------------------------
// �رջص��ӿ�
//-----------------------------------------------------------------------------
VOID ExecutorTask::OnShutDown()
{
	
}

} // namespace ECore{