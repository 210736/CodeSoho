//-----------------------------------------------------------------------------
// filename: shared_memory.cpp
// author: Sxg
// actor:
// data: 2009-12-21
// last:
// brief: �����ڴ��װ -- ͨ����װFileMappingʵ��
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "shared_memory.h"

namespace ECore{

//-----------------------------------------------------------------------------
// ����&����
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
// ���������ڴ�
//-----------------------------------------------------------------------------
BOOL SharedMem::Create()
{
	// ������
	Close();

	// ���������ڴ�
	m_hHandle = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, m_n64SharedMemSize, m_szSharedMemName);
	m_dwLastError = ::GetLastError();
	if( NULL == m_hHandle )
	{
		return FALSE;
	}

	ASSERT(m_dwLastError != ERROR_ALREADY_EXISTS);

	// �˴����û���ڴ�ӳ���ļ���ͼ
	return TRUE;
}

//-----------------------------------------------------------------------------
// �򿪹����ڴ�
//-----------------------------------------------------------------------------
BOOL SharedMem::Open()
{
	// ������
	Close();

	// ��
	m_hHandle = ::OpenFileMapping(FILE_MAP_WRITE, FALSE, m_szSharedMemName);
	m_dwLastError = ::GetLastError();
	if( NULL == m_hHandle )
	{
		return FALSE;
	}
	
	return TRUE;
}

//-----------------------------------------------------------------------------
// ���ٹ����ڴ�
//-----------------------------------------------------------------------------
VOID SharedMem::Close()
{
	// �ر���ͼ
	if( m_pData != NULL )
	{
		::UnmapViewOfFile(m_pData);
		m_pData = NULL;
	}

	// �ر��ں˶���
	if( m_hHandle != NULL )
	{	
		SAFE_CLOSE_HANDLE(m_hHandle);
		m_dwLastError = 0;
	}
}

} // namespace ECore