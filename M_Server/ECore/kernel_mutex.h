//-----------------------------------------------------------------------------
// filename: kernel_mutex.h
// author: Sxg
// actor:
// data: 2011-04-06
// last:
// brief: 操作系统事件内核对象(互斥量)封装
//-----------------------------------------------------------------------------
#pragma once

namespace ECore{

	//-----------------------------------------------------------------------------
	// 内核事件对象封装
	//-----------------------------------------------------------------------------
	class ECORE_API KernelMutex
	{
	public:
		//-------------------------------------------------------------------------
		// 构造和析构
		//-------------------------------------------------------------------------
		KernelMutex(LPCTSTR szName=NULL);
		~KernelMutex();

		//-------------------------------------------------------------------------
		// 创建，打开，关闭
		//-------------------------------------------------------------------------
		BOOL	Create(BOOL bInitialState);
		BOOL	Open();
		VOID	Close();

		//-------------------------------------------------------------------------
		// lock/unlock
		//-------------------------------------------------------------------------
		INLINE VOID	Lock()					{ ASSERT(m_hMutex != NULL); ::WaitForSingleObject(m_hMutex, INFINITE); }
		INLINE VOID	Unlock()				{ ASSERT(m_hMutex != NULL); ::ReleaseMutex(m_hMutex); }
		INLINE BOOL	TryLock(DWORD dwTime)	{ ASSERT(m_hMutex != NULL); return ::WaitForSingleObject(m_hMutex, dwTime) != WAIT_TIMEOUT; }

		//-------------------------------------------------------------------------
		// 获取错误码
		//-------------------------------------------------------------------------
		DWORD	GetLastError()				{ return m_dwLastError; }

	private:
		BOOL	m_bInitialState;		// 初始化激活状态 -- FALSE: 可以被其他线程获取；TRUE: 已被线程获取
		TCHAR	m_szName[MAX_PATH];		// 事件内核对象名字

		HANDLE	m_hMutex;				// 句柄
		DWORD	m_dwLastError;			// 最近一次操作错误码
	};

} // namespace ECore

