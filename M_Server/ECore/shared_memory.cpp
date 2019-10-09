//-----------------------------------------------------------------------------
// filename: shared_memory.cpp
// author: Sxg
// actor:
// data: 2009-12-21
// last:
// brief: 共享内存封装 -- 通过封装FileMapping实现
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "shared_memory.h"

namespace ECore{

//-----------------------------------------------------------------------------
// 构造&析构
//-----------------------------------------------------------------------------
SharedMem::SharedMem(LPCTSTR szSharedMemName, INT64 n64SharedMemSize)
{
	_tcscpy_s(m_szSharedMemName, MAX_PATH, szSharedMemName);
	m_szSharedMemName[MAX_PATH - 1] = 0;

	m_n64SharedMemSize	= n64SharedMemSize;
	m_dwLastError		= 0;
	m_hHandle			= NULL;
	m_pData				= NULL;
}

SharedMem::~SharedMem()
{
	Close();
}

//-----------------------------------------------------------------------------
// 创建共享内存
//-----------------------------------------------------------------------------
BOOL SharedMem::Create()
{
	// 先销毁
	Close();

	// 创建共享内存
	m_hHandle = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, m_n64SharedMemSize, m_szSharedMemName);
	m_dwLastError = ::GetLastError();
	if( NULL == m_hHandle )
	{
		return FALSE;
	}

	ASSERT(m_dwLastError != ERROR_ALREADY_EXISTS);

	// 此处不用获得内存映射文件视图
	return TRUE;
}

//-----------------------------------------------------------------------------
// 打开共享内存
//-----------------------------------------------------------------------------
BOOL SharedMem::Open()
{
	// 先销毁
	Close();

	// 打开
	m_hHandle = ::OpenFileMapping(FILE_MAP_WRITE, FALSE, m_szSharedMemName);
	m_dwLastError = ::GetLastError();
	if( NULL == m_hHandle )
	{
		return FALSE;
	}
	
	return TRUE;
}

//-----------------------------------------------------------------------------
// 销毁共享内存
//-----------------------------------------------------------------------------
VOID SharedMem::Close()
{
	// 关闭视图
	if( m_pData != NULL )
	{
		::UnmapViewOfFile(m_pData);
		m_pData = NULL;
	}

	// 关闭内核对象
	if( m_hHandle != NULL )
	{	
		SAFE_CLOSE_HANDLE(m_hHandle);
		m_dwLastError = 0;
	}
}

} // namespace ECore