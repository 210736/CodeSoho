//-----------------------------------------------------------------------------
// filename: kernel_mutex.cpp
// author: Sxg
// actor:
// data: 2011-04-06
// last:
// brief: 操作系统事件内核对象(互斥量)封装
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "kernel_mutex.h"

namespace ECore{

	//-----------------------------------------------------------------------------
	// 构造&析构
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
	// 创建
	//-----------------------------------------------------------------------------
	BOOL KernelMutex::Create(BOOL bInitialState)
	{
		// close
		Close();

		// 初始状态
		m_bInitialState		= bInitialState;

		// 创建
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
	// 打开
	//-----------------------------------------------------------------------------
	BOOL KernelMutex::Open()
	{
		// close
		Close();

		// 打开
		m_hMutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, m_szName);
		m_dwLastError = ::GetLastError();
		if( !m_hMutex )
		{
			return FALSE;
		}

		return TRUE;
	}

	//-----------------------------------------------------------------------------
	// 销毁
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