//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2009-12-21
//-----------------------------------------------------------------------------
#pragma once
#include "epk_define.h"
#include "epk_reader.h"
#include "disk_io.h"

namespace ECore{
#define MAX_EPK_PER_VFS	8
//-----------------------------------------------------------------------------
// 虚拟文件系统
//-----------------------------------------------------------------------------
class ECORE_API VFS
{
public:
	//-------------------------------------------------------------------------
	// 包相关
	//-------------------------------------------------------------------------
	INLINE BOOL	SetWorkingPackage(LPCWSTR szPackage);	// 设置第一个包
	INLINE BOOL	SetWorkingPackage(LPCSTR szPackage);
	INLINE BOOL	HasWorkingPackage();

	INLINE BOOL SetHighResPackage(LPCSTR szPackage);	// 设置高清包
	INLINE BOOL AddBackupPackage(LPCSTR szPackage);		// 添加备用包，给微端使用
	INLINE BOOL CheckBackupPackage();	// 检查微端包是否存在，有就读

	//-------------------------------------------------------------------------
	// 单文件基本操作
	//-------------------------------------------------------------------------
	INLINE HANDLE Open(LPCWSTR szFileName);
	INLINE HANDLE Open(LPCSTR szFileName);
	INLINE HANDLE OpenForBink(LPCWSTR szBink);
	INLINE BOOL	Close(HANDLE hHandle);
	INLINE VOID CloseForBink(HANDLE hHandle);
	INLINE DWORD Read(HANDLE hHandle, PVOID lpMemOut, DWORD dwSize);
	INLINE DWORD Seek(HANDLE hHandle, INT nOffset, DWORD dwOrigin);
	INLINE DWORD Tell(HANDLE hHandle);

	//-------------------------------------------------------------------------
	// 其他特殊操作
	//-------------------------------------------------------------------------
	INLINE BOOL	IsFileExist(LPCWSTR szFileName);
	INLINE BOOL	IsFileExist(LPCSTR szFileName);
	INLINE DWORD GetSize(LPCWSTR szFileName);
	INLINE DWORD GetSize(LPCSTR szFileName);
	INLINE DWORD GetSizeHR(LPCSTR szFileName);
	INLINE DWORD GetSize(HANDLE hHandle);
	INLINE BOOL	GetTime(LPCWSTR szFileName, LPFILETIME pFileTime);	// 得到文件最后修改时间
	INLINE BOOL	GetTime(LPCSTR szFileName, LPFILETIME pFileTime);	// 得到文件最后修改时间

	//-------------------------------------------------------------------------
	// 读包中单个文件到目标内存，返回原文件字节大小，
	// 与Open+Read相比因为直接读入指定内存，少了一次Memcpy
	//-------------------------------------------------------------------------
	INLINE DWORD LoadFile(LPVOID lpMemOut, LPCWSTR szFileName);
	INLINE DWORD LoadFile(LPVOID lpMemOut, LPCSTR szFileName);
	INLINE DWORD LoadFileHR(LPVOID lpMemOut, LPCSTR szFileName);

	VFS():m_nValidVFS(0),m_pHREPK(0) { for(INT n=0; n<MAX_EPK_PER_VFS; ++n) m_pEPK[n] = 0; m_vecEPKName.resize(MAX_EPK_PER_VFS); }
	~VFS(){ for(INT n=0; n<MAX_EPK_PER_VFS; ++n) SAFE_DEL(m_pEPK[n]); }

private:
	TObj<DiskIO>		m_pDiskIO;
	TObj<Log>			m_pLog;

	LONG VOLATILE		m_nValidVFS;
	EPK*				m_pEPK[MAX_EPK_PER_VFS];
	EPK*				m_pHREPK;

	vector<string>		m_vecEPKName;
	map<HANDLE, EPK*>	m_mapHandleEPK;
	AtomMutex			m_Lock;
};



//-----------------------------------------------------------------------------
// 设置当前包
//-----------------------------------------------------------------------------
BOOL VFS::SetWorkingPackage(LPCWSTR szPackage)
{
	if( VALID(m_pEPK[0]) )
	{
		m_pEPK[0]->Unload();
		SAFE_DEL(m_pEPK[0]);
		m_pEPK[0] = NULL;
		m_nValidVFS = 0;
	}

	// 如果包为NULL，代表取消当前的工作包
	if( !szPackage )
		return TRUE;

	m_pEPK[0] = new EPK;
	if( !m_pEPK[0] )
		return FALSE;

	BOOL bResult = m_pEPK[0]->Load(szPackage);
	if( !bResult )
	{
		SAFE_DEL(m_pEPK[0]);
		m_pEPK[0] = NULL;
	}
	else
	{
		m_nValidVFS = 1;
	}

	m_vecEPKName[0] = SS::To8(szPackage);
	return bResult;
}


//-----------------------------------------------------------------------------
// 设置当前包
//-----------------------------------------------------------------------------
BOOL VFS::SetWorkingPackage(LPCSTR szPackage)
{
	if( VALID(m_pEPK[0]) )
	{
		m_pEPK[0]->Unload();
		SAFE_DEL(m_pEPK[0]);
		m_pEPK[0] = NULL;
		m_nValidVFS = 0;
	}

	// 如果包为NULL，代表取消当前的工作包
	if( !szPackage )
	{
		m_vecEPKName[0] = ""; 
		return TRUE;
	}

	m_pEPK[0] = new EPK;
	if( !m_pEPK[0] )
		return FALSE;

	BOOL bResult = m_pEPK[0]->Load(szPackage);
	if( !bResult )
	{
		SAFE_DEL(m_pEPK[0]);
		m_pEPK[0] = NULL;
	}
	else
	{
		m_nValidVFS = 1;
	}

	m_vecEPKName[0] = szPackage;
	return bResult;
}


//-----------------------------------------------------------------------------
// 设置高清包
//-----------------------------------------------------------------------------
BOOL VFS::SetHighResPackage(LPCSTR szPackage)
{
	m_pHREPK = new EPK;
	if( !m_pHREPK )
		return FALSE;

	if( !m_pHREPK->Load(szPackage) )
	{
		SAFE_DEL(m_pHREPK);
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 添加备用包，给微端使用
//-----------------------------------------------------------------------------
BOOL VFS::AddBackupPackage(LPCSTR szPackage)
{
	if( !VALID(m_pEPK[0]) )	// 主包没有，不允许挂副包
		return FALSE;

	for(INT n=1; n<MAX_EPK_PER_VFS; ++n)
	{
		if( m_vecEPKName[n].empty() )
		{
			m_vecEPKName[n] = szPackage;
			m_pEPK[n] = new EPK;
			if( !m_pEPK[n] )
				return FALSE;

			if( !m_pEPK[n]->Load(szPackage) )
			{
				SAFE_DEL(m_pEPK[n]);
				return FALSE;
			}
			
			InterlockedIncrementEx(&m_nValidVFS);
			return TRUE;
		}
	}

	return FALSE;
}


//-----------------------------------------------------------------------------
// 检查微端包是否存在，有就读
//-----------------------------------------------------------------------------
BOOL VFS::CheckBackupPackage()
{
	for(INT n=1; n<MAX_EPK_PER_VFS; ++n)
	{
		if( !VALID(m_pEPK[n]) && !m_vecEPKName[n].empty() )
		{
			if( !DiskIO::IsFileExist(m_vecEPKName[n].c_str()) )
				return FALSE;

			m_pEPK[n] = new EPK;
			if( !m_pEPK[n] )
				return FALSE;

			if( !m_pEPK[n]->Load(m_vecEPKName[n].c_str()) )
			{
				SAFE_DEL(m_pEPK[n]);
				return FALSE;
			}

			InterlockedIncrementEx(&m_nValidVFS);
		}
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
// 当前是否有包打开
//-----------------------------------------------------------------------------
BOOL VFS::HasWorkingPackage()
{
	return (VALID(m_pEPK[0]));
}


//-----------------------------------------------------------------------------
// 打开文件
//-----------------------------------------------------------------------------
HANDLE VFS::Open(LPCWSTR szFileName)
{
	HANDLE hHandle = INVALID_HANDLE;
	if( m_pEPK[0] )	// 如果当前有打开的包，则从包打开
	{
		for(INT n=0; n<m_nValidVFS; ++n)
		{
			hHandle = m_pEPK[n]->Open(szFileName);
			if( INVALID_HANDLE != hHandle )
			{
				m_Lock.Lock();
				m_mapHandleEPK.insert(make_pair(hHandle, m_pEPK[n]));
				m_Lock.Unlock();
				break;
			}
		}
	}
	else
	{
		// 如果当前没有打开包，则从本地打开文件
		hHandle = m_pDiskIO->Create(szFileName, GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	}

	// 写资源文件日志，用于整理微端资源 by 王炜
	if (g_bFileLog)
		m_pLog->WriteFileLog(szFileName, hHandle);

	return hHandle;
}

//-----------------------------------------------------------------------------
// 打开文件
//-----------------------------------------------------------------------------
HANDLE VFS::Open(LPCSTR szFileName)
{
	HANDLE hHandle = INVALID_HANDLE;
	if( m_pEPK[0] )	// 如果当前有打开的包，则从包打开
	{
		for(INT n=0; n<m_nValidVFS; ++n)
		{
			hHandle = m_pEPK[n]->Open(szFileName);
			if( INVALID_HANDLE != hHandle )
			{
				m_Lock.Lock();
				m_mapHandleEPK.insert(make_pair(hHandle, m_pEPK[n]));
				m_Lock.Unlock();
				break;
			}
		}
	}
	else
	{
		// 如果当前没有打开包，则从本地打开文件
		hHandle = m_pDiskIO->Create(szFileName, GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	}

	// 写资源文件日志，用于整理微端资源 by 王炜
	if (g_bFileLog)
		m_pLog->WriteFileLog(szFileName, hHandle);

	return hHandle;
}

//-----------------------------------------------------------------------------
// 打开bink文件
//-----------------------------------------------------------------------------
HANDLE VFS::OpenForBink(LPCWSTR szBink)
{
	if( m_pEPK[0] )	// 如果当前有打开的包，则从包打开
	{
		HANDLE hHandle = INVALID_HANDLE;
		for(INT n=0; n<m_nValidVFS; ++n)
		{
			hHandle = m_pEPK[n]->OpenForBink(szBink);
			if( INVALID_HANDLE != hHandle )
			{
				m_Lock.Lock();
				m_mapHandleEPK.insert(make_pair(hHandle, m_pEPK[n]));
				m_Lock.Unlock();
				return hHandle;
			}
		}
		return INVALID_HANDLE;
	}
	else
	{
		// 如果当前没有打开包，则从本地打开文件
		return m_pDiskIO->Create(szBink, GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	}
}


//-----------------------------------------------------------------------------
// 关闭bink文件
//-----------------------------------------------------------------------------
VOID VFS::CloseForBink(HANDLE hHandle)
{
	ASSERT(hHandle);
	if( !VALID(m_pEPK[0]) )
		m_pDiskIO->Close(hHandle);
}


//-----------------------------------------------------------------------------
// 关闭文件
//-----------------------------------------------------------------------------
BOOL VFS::Close(HANDLE hHandle)
{
	ASSERT(hHandle);
	if( m_pEPK[0] )
	{
		m_Lock.Lock();
		map<HANDLE, EPK*>::iterator it = m_mapHandleEPK.find(hHandle);
		if( it != m_mapHandleEPK.end() )
		{
			BOOL result = (it->second)->Close(hHandle);
			m_mapHandleEPK.erase(it);
			m_Lock.Unlock();
			return result;
		}
		else
		{
			m_Lock.Unlock();
			return FALSE;
		}
	}
	else
		return m_pDiskIO->Close(hHandle);
}


//-----------------------------------------------------------------------------
// 读取文件
//-----------------------------------------------------------------------------
DWORD VFS::Read(HANDLE hHandle, PVOID lpMemOut, DWORD dwSize)
{
	if( m_pEPK[0] )	// 用任何EPK均可进行此操作
		return m_pEPK[0]->Read(hHandle, lpMemOut, dwSize);
	else
		return m_pDiskIO->Read(hHandle, lpMemOut, dwSize);
}


//-----------------------------------------------------------------------------
// 定位文件
//-----------------------------------------------------------------------------
DWORD VFS::Seek(HANDLE hHandle, INT nOffset, DWORD dwOrigin)
{
	if( m_pEPK[0] )	// 用任何EPK均可进行此操作
		return m_pEPK[0]->Seek(hHandle, nOffset, dwOrigin);
	else
		return m_pDiskIO->Seek(hHandle, nOffset, dwOrigin);
}


//-----------------------------------------------------------------------------
// 告知文件指针位置
//-----------------------------------------------------------------------------
DWORD VFS::Tell(HANDLE hHandle)
{
	if( m_pEPK[0] )
		return m_pEPK[0]->Tell(hHandle);
	else
		return m_pDiskIO->Seek(hHandle, 0, SEEK_CUR);
}


//-----------------------------------------------------------------------------
// 得到文件大小
//-----------------------------------------------------------------------------
DWORD VFS::GetSize(HANDLE hHandle)
{
	if( m_pEPK[0] )
		return m_pEPK[0]->GetSize(hHandle);
	else
		return m_pDiskIO->GetSize(hHandle);
}


//-----------------------------------------------------------------------------
// 检查文件是否存在
//-----------------------------------------------------------------------------
BOOL VFS::IsFileExist(LPCWSTR szFileName)
{
	if( m_pEPK[0] )	// 如果当前有打开的包，则从包打开
	{
		for(INT n=0; n<m_nValidVFS; ++n)
		{
			DWORD dwSize = m_pEPK[n]->LoadFile(NULL, szFileName);
			if( VALID(dwSize) )
				return TRUE;
		}
		return FALSE;
	}
	else
	{
		return m_pDiskIO->IsFileExist(szFileName);
	}
}


//-----------------------------------------------------------------------------
// 检查文件是否存在
//-----------------------------------------------------------------------------
BOOL VFS::IsFileExist(LPCSTR szFileName)
{
	if( m_pEPK[0] )
	{
		for(INT n=0; n<m_nValidVFS; ++n)
		{
			DWORD dwSize = m_pEPK[n]->LoadFile(NULL, szFileName);
			if( VALID(dwSize) )
				return TRUE;
		}
		return FALSE;
	}
	else
	{
		return m_pDiskIO->IsFileExist(szFileName);
	}
}


//-----------------------------------------------------------------------------
// 得到文件大小
//-----------------------------------------------------------------------------
DWORD VFS::GetSize(LPCWSTR szFileName)
{
	DWORD hHandle = INVALID;
	if( m_pEPK[0] )
	{
		for(INT n=0; n<m_nValidVFS; ++n)
		{
			hHandle = m_pEPK[n]->LoadFile(NULL, szFileName);
			if( VALID(hHandle) )
				break;
		}
	}
	else
	{
		hHandle = m_pDiskIO->GetSize(szFileName);
	}

	// 写资源文件日志，用于整理微端资源 by 王炜
	if (g_bFileLog)
		m_pLog->WriteFileLog(szFileName, (HANDLE)hHandle);

	return hHandle;
}

//-----------------------------------------------------------------------------
// 得到文件大小
//-----------------------------------------------------------------------------
DWORD VFS::GetSize(LPCSTR szFileName)
{
	DWORD hHandle = INVALID;
	if( m_pEPK[0] )
	{
		for(INT n=0; n<m_nValidVFS; ++n)
		{
			hHandle = m_pEPK[n]->LoadFile(NULL, szFileName);
			if( VALID(hHandle) )
				break;
		}
	}
	else
	{
		hHandle = m_pDiskIO->GetSize(szFileName);
	}

	// 写资源文件日志，用于整理微端资源 by 王炜
	if (g_bFileLog)
		m_pLog->WriteFileLog(szFileName, (HANDLE)hHandle);

	return hHandle;
}


//-----------------------------------------------------------------------------
// 得到文件大小
//-----------------------------------------------------------------------------
DWORD VFS::GetSizeHR(LPCSTR szFileName)
{
	DWORD hHandle = INVALID;
	if( m_pEPK[0] )
	{
		if( m_pHREPK )
			hHandle = m_pHREPK->LoadFile(NULL, szFileName);

		if( !VALID(hHandle) )
		{
			for(INT n=0; n<m_nValidVFS; ++n)
			{
				hHandle = m_pEPK[n]->LoadFile(NULL, szFileName);
				if( VALID(hHandle) )
					break;
			}
		}
	}
	else
	{
		hHandle = m_pDiskIO->GetSize(szFileName);
	}

	// 写资源文件日志，用于整理微端资源 by 王炜
	if (g_bFileLog)
		m_pLog->WriteFileLog(szFileName, (HANDLE)hHandle);

	return hHandle;
}



//-----------------------------------------------------------------------------
// 得到文件最后修改时间
//-----------------------------------------------------------------------------
BOOL VFS::GetTime(LPCWSTR szFileName, LPFILETIME pFileTime)
{
	if( m_pEPK[0] )
	{
		for(INT n=0; n<m_nValidVFS; ++n)
		{
			if( m_pEPK[n]->GetTime(szFileName, pFileTime) )
				return TRUE;
		}

		return FALSE;
	}
	else
	{
		return m_pDiskIO->GetTime(szFileName, pFileTime);
	}

}

//-----------------------------------------------------------------------------
// 得到文件最后修改时间
//-----------------------------------------------------------------------------
BOOL VFS::GetTime(LPCSTR szFileName, LPFILETIME pFileTime)
{
	if( m_pEPK[0] )
	{
		for(INT n=0; n<m_nValidVFS; ++n)
		{
			if( m_pEPK[n]->GetTime(szFileName, pFileTime) )
				return TRUE;
		}

		return FALSE;
	}
	else
	{
		return m_pDiskIO->GetTime(szFileName, pFileTime);
	}
}


//-----------------------------------------------------------------------------
// 对普通文件来说是一样的，而对包来说，因为包是压缩数据，这样读取可以少一次内存拷贝
//-----------------------------------------------------------------------------
DWORD VFS::LoadFile(LPVOID lpMemOut, LPCWSTR szFileName)
{
	DWORD hHandle = INVALID;
	if( m_pEPK[0] )
	{
		for(INT n=0; n<m_nValidVFS; ++n)
		{
			hHandle = m_pEPK[n]->LoadFile(lpMemOut, szFileName);
			if( VALID(hHandle) )
				break;
		}
	}
	else
	{
		hHandle = m_pDiskIO->LoadToMem(lpMemOut, szFileName);
	}

	// 写资源文件日志，用于整理微端资源 by 王炜
	if (g_bFileLog)
		m_pLog->WriteFileLog(szFileName, (HANDLE)hHandle);

	return hHandle;
}

//-----------------------------------------------------------------------------
// 对普通文件来说是一样的，而对包来说，因为包是压缩数据，这样读取可以少一次内存拷贝
//-----------------------------------------------------------------------------
DWORD VFS::LoadFile(LPVOID lpMemOut, LPCSTR szFileName)
{
	DWORD hHandle = INVALID;
	if( m_pEPK[0] )
	{
		for(INT n=0; n<m_nValidVFS; ++n)
		{
			hHandle = m_pEPK[n]->LoadFile(lpMemOut, szFileName);
			if( VALID(hHandle) )
				break;
		}
	}
	else
	{
		hHandle = m_pDiskIO->LoadToMem(lpMemOut, szFileName);
	}

	// 写资源文件日志，用于整理微端资源 by 王炜
	if (g_bFileLog)
		m_pLog->WriteFileLog(szFileName, (HANDLE)hHandle);

	return hHandle;
}


//-----------------------------------------------------------------------------
// 对普通文件来说是一样的，而对包来说，因为包是压缩数据，这样读取可以少一次内存拷贝
//-----------------------------------------------------------------------------
DWORD VFS::LoadFileHR(LPVOID lpMemOut, LPCSTR szFileName)
{
	DWORD hHandle = INVALID;
	if( m_pEPK[0] )
	{
		if( m_pHREPK )
			hHandle = m_pHREPK->LoadFile(lpMemOut, szFileName);

		if( !VALID(hHandle) )
		{
			for(INT n=0; n<m_nValidVFS; ++n)
			{
				hHandle = m_pEPK[n]->LoadFile(lpMemOut, szFileName);
				if( VALID(hHandle) )
					break;
			}
		}
	}
	else
	{
		hHandle = m_pDiskIO->LoadToMem(lpMemOut, szFileName);
	}

	// 写资源文件日志，用于整理微端资源 by 王炜
	if (g_bFileLog)
		m_pLog->WriteFileLog(szFileName, (HANDLE)hHandle);

	return hHandle;
}
}