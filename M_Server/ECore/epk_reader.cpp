//-----------------------------------------------------------------------------
// Time: 2003-1-2
// Auth: Lyp
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "epk_define.h"
#include "epk_reader.h"


namespace ECore{
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
EPK::EPK()
{
	ZeroMemory(&m_Header, sizeof(m_Header));
	m_pTable				=	0;
	m_bLoaded				=	0;
	m_dwAllocGranularity	=	0;
	m_nOpenedFileNum		=	0;
	m_hHandle				=	INVALID_HANDLE;
	m_hMapHandle			=	INVALID_HANDLE;

	SYSTEM_INFO si;
	::GetSystemInfo(&si);
	m_dwAllocGranularity = si.dwAllocationGranularity;	// 得到系统内存分配粒度
}

EPK::~EPK()
{
	Unload();
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOL EPK::Load(LPCWSTR szFileName)
{
	if( m_bLoaded )	// 必须先释放再载入
		this->Unload();

	m_strFileName = szFileName;
	m_hHandle = m_pDiskIO->Create(szFileName, GENERIC_READ,	FILE_SHARE_READ | FILE_SHARE_WRITE, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	if( INVALID_HANDLE == m_hHandle )
		return FALSE;

	// 读入文件头
	m_pDiskIO->Read(m_hHandle, &m_Header, sizeof(m_Header));
	
	// 检查
	if( EPK_LABEL != m_Header.dwLable || ( EPK_VERSION != m_Header.dwVersion && EPK_VERSION_NEW != m_Header.dwVersion ) )
	{
		ERR(_T("Unknow file format %s"), szFileName);
		m_pDiskIO->Close(m_hHandle);
		m_hHandle = INVALID_HANDLE;
		return FALSE;
	}

	// 读入table
	m_pTable = (tagEPKTable*)malloc(sizeof(tagEPKTable)*m_Header.dwValidTableNum);
	if( !m_pTable )
	{
		m_pDiskIO->Close(m_hHandle);
		m_hHandle = INVALID_HANDLE;
		return FALSE;
	}
	
	if( !m_pDiskIO->Seek(m_hHandle, m_Header.dwTableStart) )
	{
		ERR(_T("Could not seek table from the EPK %s, seekpos=%ld"), szFileName, m_Header.dwTableStart);
		m_pDiskIO->Close(m_hHandle);
		m_hHandle = INVALID_HANDLE;
		SAFE_FREE(m_pTable);
		return FALSE;
	}

	if( sizeof(tagEPKTable)*m_Header.dwValidTableNum != m_pDiskIO->Read(m_hHandle, m_pTable, sizeof(tagEPKTable)*m_Header.dwValidTableNum) )
	{
		ERR(_T("Could not load epk %s table"), szFileName);
		m_pDiskIO->Close(m_hHandle);
		m_hHandle = INVALID_HANDLE;
		SAFE_FREE(m_pTable);
		return FALSE;
	}
	
	m_hMapHandle = m_pDiskIO->CreateMapping(m_hHandle, NULL, PAGE_READONLY, 0, (LPCSTR)NULL);	// 尝试建立文件映射
	if( !m_hMapHandle )
	{
		m_hMapHandle = INVALID_HANDLE;
	}
	else
	{
		// 文件映射已经存在
		if( ::GetLastError() == ERROR_ALREADY_EXISTS )
		{
			ERR(_T("File mapping %s has already exist"), szFileName);
			m_pDiskIO->CloseMappingHandle(m_hMapHandle);
			m_hMapHandle = INVALID_HANDLE;
			m_pDiskIO->Close(m_hHandle);
			m_hHandle = INVALID_HANDLE;
			SAFE_FREE(m_pTable);
			return FALSE;
		}
	}

	m_bLoaded = TRUE;
	return TRUE;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOL EPK::Load(LPCSTR szFileName)
{
	if( m_bLoaded )	// 必须先释放再载入
		this->Unload();

	m_strFileName = SS::To16(szFileName);
	m_hHandle = m_pDiskIO->Create(szFileName, GENERIC_READ,	FILE_SHARE_READ | FILE_SHARE_WRITE, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	if( INVALID_HANDLE == m_hHandle )
		return FALSE;

	// 读入文件头
	m_pDiskIO->Read(m_hHandle, &m_Header, sizeof(m_Header));
	
	// 检查
	if( EPK_LABEL != m_Header.dwLable || ( EPK_VERSION != m_Header.dwVersion && EPK_VERSION_NEW != m_Header.dwVersion ) )
	{
		ERR(_T("Unknow file format %s"), m_strFileName.c_str());
		m_pDiskIO->Close(m_hHandle);
		m_hHandle = INVALID_HANDLE;
		return FALSE;
	}

	// 读入table
	m_pTable = (tagEPKTable*)malloc(sizeof(tagEPKTable)*m_Header.dwValidTableNum);
	if( !m_pTable )
	{
		m_pDiskIO->Close(m_hHandle);
		m_hHandle = INVALID_HANDLE;
		return FALSE;
	}
	
	if( !m_pDiskIO->Seek(m_hHandle, m_Header.dwTableStart) )
	{
		ERR(_T("Could not seek table from the EPK %s, seekpos=%ld"), m_strFileName.c_str(), m_Header.dwTableStart);
		m_pDiskIO->Close(m_hHandle);
		m_hHandle = INVALID_HANDLE;
		SAFE_FREE(m_pTable);
		return FALSE;
	}

	if( sizeof(tagEPKTable)*m_Header.dwValidTableNum != m_pDiskIO->Read(m_hHandle, m_pTable, sizeof(tagEPKTable)*m_Header.dwValidTableNum) )
	{
		ERR(_T("Could not load epk %s table"), m_strFileName.c_str());
		m_pDiskIO->Close(m_hHandle);
		m_hHandle = INVALID_HANDLE;
		SAFE_FREE(m_pTable);
		return FALSE;
	}
	
	m_hMapHandle = m_pDiskIO->CreateMapping(m_hHandle, NULL, PAGE_READONLY, 0, (LPCSTR)0);	// 尝试建立文件映射
	if( !m_hMapHandle )
	{
		m_hMapHandle = INVALID_HANDLE;
	}
	else
	{
		// 文件映射已经存在
		if( ::GetLastError() == ERROR_ALREADY_EXISTS )
		{
			ERR(_T("File mapping %s has already exist"), m_strFileName.c_str());
			m_pDiskIO->CloseMappingHandle(m_hMapHandle);
			m_hMapHandle = INVALID_HANDLE;
			m_pDiskIO->Close(m_hHandle);
			m_hHandle = INVALID_HANDLE;
			SAFE_FREE(m_pTable);
			return FALSE;
		}
	}

	m_bLoaded = TRUE;
	return TRUE;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOL EPK::Unload()
{
	if( !m_bLoaded )
		return FALSE;

	// 确认所有文件已经关闭,如果有未关闭的文件，可以打开CHECK_OPEN_FILE宏来检查文件名
	ASSERT(m_nOpenedFileNum == 0);

	if( INVALID_HANDLE != m_hMapHandle )
	{
		m_pDiskIO->CloseMappingHandle(m_hMapHandle);
		m_hMapHandle = INVALID_HANDLE;
	}

	m_pDiskIO->Close(m_hHandle);
	m_hHandle = INVALID_HANDLE;

	ZeroMemory(&m_Header, sizeof(m_Header));
	SAFE_FREE(m_pTable);

	m_strFileName.clear();
	m_nOpenedFileNum = 0;
	m_bLoaded = FALSE;
	return TRUE;
}



} // namespace ECore{
