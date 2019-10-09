//-----------------------------------------------------------------------------
// filename: kernel_mutex.cpp
// author: Sxg
// actor:
// data: 2011-04-06
// last:
// brief: ����ϵͳ�¼��ں˶���(������)��װ
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "kernel_mutex.h"

namespace ECore{

	//-----------------------------------------------------------------------------
	// ����&����
	//-----------------------------------------------------------------------------
	KernelMutex::KernelMutex(LPCTSTR szName/*=NULL*/)
	{
		m_hMutex			= NULL;
		m_dwLastError		= 0;

		ZeroMemory(m_szName, sizeof(m_szName));

		if( szName != NULL )
			_tcscpy_s(m_szName, MAX_PATH, szName);
	}

	KernelMutex::~KernelMutex()
	{
		Close();
	}

	//-----------------------------------------------------------------------------
	// ����
	//-----------------------------------------------------------------------------
	BOOL KernelMutex::Create(BOOL bInitialState)
	{
		// close
		Close();

		// ��ʼ״̬
		m_bInitialState		= bInitialState;

		// ����
		m_hMutex = ::CreateMutex(NULL, m_bInitialState, m_szName);
		m_dwLastError = ::GetLastError();
		if( !m_hMutex )
		{
			return FALSE;
		}

		ASSERT(m_dwLastError != ERROR_ALREADY_EXISTS);

		return TRUE;
	}

	//-----------------------------------------------------------------------------
	// ��
	//-----------------------------------------------------------------------------
	BOOL KernelMutex::Open()
	{
		// close
		Close();

		// ��
		m_hMutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, m_szName);
		m_dwLastError = ::GetLastError();
		if( !m_hMutex )
		{
			return FALSE;
		}

		return TRUE;
	}

	//-----------------------------------------------------------------------------
	// ����
	//-----------------------------------------------------------------------------
	VOID KernelMutex::Close()
	{
		if( m_hMutex )
		{
			SAFE_CLOSE_HANDLE(m_hMutex);
			m_dwLastError = 0;
		}
	}

} // namespace ECore