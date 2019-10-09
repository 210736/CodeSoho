//--------------------------------------------------------------------------------
// File: task_scheduler.h
// Desc: ������������ⲿ�����ύ��ɾ�����񣬵�������ÿ�θ���ʱ�Ὣ�������б��е�
//       ��������ͨ����ɶ˿ڷ������֪ͨ���ڲ����̹߳�������õ���ɶ˿�ִ֪ͨ��
//       ÿһ���������������ֱ�����е�����ִ����ɺ��ٷ���
// Auth: Aslan
// Date: 2009-12-09
// Last: 2009-12-09
//---------------------------------------------------------------------------------
#pragma once

#include "mutex.h"
#include "condition.h"
#include "thread_mgr.h"

namespace ECore{


class ECORE_API TaskScheduler
{
public:
	TaskScheduler();
	~TaskScheduler();

	//-----------------------------------------------------------------------------
	// ��ʼ�������ٺ͸���
	//-----------------------------------------------------------------------------
	BOOL	Init(INT nWorkerNum=0);
	VOID	Update();
	VOID	Destory();

	//-----------------------------------------------------------------------------
	// ����һ������
	//-----------------------------------------------------------------------------
	VOID	AddTask(Task* pTask);
	VOID	RemoveTask(Task* pTask);

	//-----------------------------------------------------------------------------
	// �õ�����worker�̵߳�ϵͳ�����߳�ID
	//-----------------------------------------------------------------------------
	VOID	GetAllWorkerThreadSystemID(TList<DWORD>& listID);

	//-----------------------------------------------------------------------------
	// ĳ������ִ�����
	//-----------------------------------------------------------------------------
	INLINE VOID	OneTaskDone();

private:
	TaskScheduler(const TaskScheduler&);
	const TaskScheduler& operator=(const TaskScheduler&);

private:
	HANDLE				m_hCompletionPort;	// ��ɶ˿�

	INT					m_nWorkerNum;		// �����̵߳�����
	ThreadMgr			m_ThreadMgr;		// �̹߳�����

	Mutex				m_WorkMutex;		// ������
	Condition			m_Condition;		// �������������Ϻ������֪ͨ

	std::set<Task*>		m_setTask;			// �������
	INT volatile		m_nCurTaskLeft;		// ��ǰ
};

//-----------------------------------------------------------------------------------
// ĳ������ִ�����
//-----------------------------------------------------------------------------------
VOID TaskScheduler::OneTaskDone()
{
	
	if( 0 == InterlockedDecrementEx((volatile ULONGLONG*)&m_nCurTaskLeft) )
	{
		m_Condition.Set();
	}
}


} // namespace ECore{