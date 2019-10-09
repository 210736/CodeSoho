//-----------------------------------------------------------------------------
// filename: event.cpp
// author: Sxg
// actor:
// data: 2009-12-21
// last:
// brief: ����ϵͳ�¼��ں˶����װ
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "event.h"

namespace ECore{

//-----------------------------------------------------------------------------
// ����&����
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
// ����
//-----------------------------------------------------------------------------
BOOL Event::Create(BOOL bManual, BOOL bInitialState)
{
	// close
	Close();

	// ���ó�ʼ״̬
	m_bManual			= bManual;
	m_bInitialState		= bInitialState;

	// ����
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
// ��
//-----------------------------------------------------------------------------
BOOL Event::Open()
{
	// close
	Close();

	// ��
	m_hEvent = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, m_szName);
	m_dwLastError = ::GetLastError();
	if( !m_hEvent )
	{
		return FALSE;
	}
	
	return TRUE;
}

//-----------------------------------------------------------------------------
// ����
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