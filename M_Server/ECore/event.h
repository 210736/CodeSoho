//-----------------------------------------------------------------------------
// filename: event.h
// author: Sxg
// actor:
// data: 2009-12-21
// last:
// brief: 操作系统事件内核对象封装
//-----------------------------------------------------------------------------
#pragma once

namespace ECore{

//-----------------------------------------------------------------------------
// 内核事件对象封装
//-----------------------------------------------------------------------------
class ECORE_API Event
{
public:
	//-------------------------------------------------------------------------
	// 构造和析构
	//-------------------------------------------------------------------------
	Event(LPCTSTR szName=NULL);
	~Event();

	//-------------------------------------------------------------------------
	// 创建，打开，关闭
	//-------------------------------------------------------------------------
	BOOL	Create(BOOL bManual, BOOL bInitialState);
	BOOL	Open();
	VOID	Close();

	//-------------------------------------------------------------------------
	// 设置，反设置，等待
	//-------------------------------------------------------------------------
	INLINE VOID		Set()						{ ASSERT(m_hEvent != NULL); ::SetEvent(m_hEvent); }
	INLINE VOID		Reset()						{ ASSERT(m_hEvent != NULL); ::ResetEvent(m_hEvent); }
	INLINE DWORD	Wait(DWORD dwTime=INFINITE)	{ ASSERT(m_hEvent != NULL); return ::WaitForSingleObject(m_hEvent, dwTime); }
	INLINE BOOL		IsWaitTimeOut(DWORD dwTime)	{ ASSERT(m_hEvent != NULL); return WAIT_TIMEOUT == ::WaitForSingleObject(m_hEvent, dwTime); }

	//-------------------------------------------------------------------------
	// 获取错误码
	//-------------------------------------------------------------------------
	INLINE DWORD	GetLastError()				{ return m_dwLastError; }

private:
	BOOL	m_bManual;				// 手动设置激活状态
	BOOL	m_bInitialState;		// 初始化激活状态
	TCHAR	m_szName[MAX_PATH];		// 事件内核对象名字

	HANDLE	m_hEvent;				// 句柄
	DWORD	m_dwLastError;			// 最近一次操作错误码
};

} // namespace ECore

