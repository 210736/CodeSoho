//-----------------------------------------------------------------------------
// File: condition.h
// Desc: 简单同步事件实现类
// Auth: Lyp
// Date: 2003/11/30
//-----------------------------------------------------------------------------
#pragma once

namespace ECore{

//-----------------------------------------------------------------------------
// 简单同步事件实现类
//-----------------------------------------------------------------------------
class Condition
{
public:
	Condition(BOOL bManual, LPCTSTR szName=NULL)	{ m_hEvent = ::CreateEvent(NULL, bManual, FALSE, szName); ASSERT(m_hEvent != NULL); }
	~Condition()									{ ::CloseHandle(m_hEvent); }

	//--------------------------------------------------------------------------
	// 设置，反设置，等待
	//--------------------------------------------------------------------------
	VOID	Set()				{ ::SetEvent(m_hEvent); }
	VOID	ReSet()				{ ::ResetEvent(m_hEvent); }
	VOID	Wait()				{ ::WaitForSingleObject(m_hEvent, INFINITE); }

private:
	HANDLE	m_hEvent;
};
}
