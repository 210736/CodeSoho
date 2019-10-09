//-----------------------------------------------------------------------------
// filename: event.cpp
// author: Sxg
// actor:
// data: 2009-12-21
// last:
// brief: 操作系统事件内核对象封装
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "event.h"

namespace ECore{

//-----------------------------------------------------------------------------
// 构造&析构
//-----------------------------------------------------------------------------
Event::Event(LPCTSTR szName/*=NULL*/)
{
	m_hEvent			= NULL;
	m_dwLastError		= 0;

	ZeroMemory(m_szName, sizeof(m_szName));

	if( szName != NULL )
	{
		_tcscpy_s(m_szName, MAX_PATH, szName);
		m_szName[MAX_PATH - 1] = 0;
	}
}

Event::~Event()
{
	Close();
}

//-----------------------------------------------------------------------------
// 创建
//-----------------------------------------------------------------------------
BOOL Event::Create(BOOL bManual, BOOL bInitialState)
{
	// close
	Close();

	// 设置初始状态
	m_bManual			= bManual;
	m_bInitialState		= bInitialState;

	// 创建
	m_hEvent = ::CreateEvent(NULL, m_bManual, m_bInitialState, m_szName);
	m_dwLastError = ::GetLastError();
	if( !m_hEvent )
	{
		return FALSE;
	}

	ASSERT(m_dwLastError != ERROR_ALREADY_EXISTS);

	return TRUE;
}

//-----------------------------------------------------------------------------
// 打开
//-----------------------------------------------------------------------------
BOOL Event::Open()
{
	// close
	Close();

	// 打开
	m_hEvent = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, m_szName);
	m_dwLastError = ::GetLastError();
	if( !m_hEvent )
	{
		return FALSE;
	}
	
	return TRUE;
}

//-----------------------------------------------------------------------------
// 销毁
//-----------------------------------------------------------------------------
VOID Event::Close()
{
	if( m_hEvent )
	{
		SAFE_CLOSE_HANDLE(m_hEvent);
		m_dwLastError = 0;
	}
}

} // namespace ECore