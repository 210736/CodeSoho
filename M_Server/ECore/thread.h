//---------------------------------------------------------------------------------
// File: thread.h
// Desc: 线程对象，它负责执行里面的执行体，维护线程的句柄和线程ID，并加入一些统计信息
// Auth: Aslan
// Date: 2009-12-09
// Last: 2009-12-09
//---------------------------------------------------------------------------------
#pragma once

namespace ECore{
//----------------------------------------------------------------------------------
// 线程对象
//----------------------------------------------------------------------------------
class ECORE_API Thread
{
	friend class ThreadMgr;
public:
	//------------------------------------------------------------------------------
	// 构造和析构函数
	//------------------------------------------------------------------------------
	Thread(Executor* pExecutor, ThreadMgr* pMgr, BOOL bDestoryAtEnd=FALSE, BOOL bSuspended=FALSE);
	~Thread();

	//------------------------------------------------------------------------------
	// 返回各种信息
	//------------------------------------------------------------------------------
	Executor*		GetExecutor() const		{ return m_pExecutor; }
	ThreadMgr*		GetMgr() const			{ return m_pMgr; }
	DWORD			GetID() const			{ return m_dwID; }

	//-------------------------------------------------------------------------------
	// 让线程停止执行
	//-------------------------------------------------------------------------------
	VOID			Stop();

private:
	Thread(const Thread&);
	const Thread& operator=(const Thread&);

private:
	Executor*		m_pExecutor;		// 执行体
	ThreadMgr*		m_pMgr;				// 线程管理器

	HANDLE			m_hHandle;			// 线程句柄
	DWORD			m_dwThreadID;		// 线程ID
	DWORD			m_dwID;				// 线程管理器分配的一个ID
	BOOL			m_bDestryAtEnd;		// 线程退出时是否删除执行体
	BOOL			m_bSuspended;		// 是否挂起
};

} // namespace ECore{