//--------------------------------------------------------------------------------
// File: task_scheduler.h
// Desc: 任务调度器，外部负责提交和删除任务，调度器在每次更新时会将其任务列表中的
//       所有任务通过完成端口发送完成通知，内部的线程管理器会得到完成端口通知执行
//       每一个任务，任务调度器直到所有的任务都执行完成后再返回
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
	// 初始化，销毁和更新
	//-----------------------------------------------------------------------------
	BOOL	Init(INT nWorkerNum=0);
	VOID	Update();
	VOID	Destory();

	//-----------------------------------------------------------------------------
	// 加入一个任务
	//-----------------------------------------------------------------------------
	VOID	AddTask(Task* pTask);
	VOID	RemoveTask(Task* pTask);

	//-----------------------------------------------------------------------------
	// 得到所有worker线程的系统分配线程ID
	//-----------------------------------------------------------------------------
	VOID	GetAllWorkerThreadSystemID(TList<DWORD>& listID);

	//-----------------------------------------------------------------------------
	// 某个任务执行完毕
	//-----------------------------------------------------------------------------
	INLINE VOID	OneTaskDone();

private:
	TaskScheduler(const TaskScheduler&);
	const TaskScheduler& operator=(const TaskScheduler&);

private:
	HANDLE				m_hCompletionPort;	// 完成端口

	INT					m_nWorkerNum;		// 工作线程的数量
	ThreadMgr			m_ThreadMgr;		// 线程管理器

	Mutex				m_WorkMutex;		// 工作锁
	Condition			m_Condition;		// 所有任务更新完毕后的条件通知

	std::set<Task*>		m_setTask;			// 任务队列
	INT volatile		m_nCurTaskLeft;		// 当前
};

//-----------------------------------------------------------------------------------
// 某个任务执行完毕
//-----------------------------------------------------------------------------------
VOID TaskScheduler::OneTaskDone()
{
	
	if( 0 == InterlockedDecrementEx((volatile ULONGLONG*)&m_nCurTaskLeft) )
	{
		m_Condition.Set();
	}
}


} // namespace ECore{