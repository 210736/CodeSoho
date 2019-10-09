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
// �����ļ�ϵͳ
//-----------------------------------------------------------------------------
class ECORE_API VFS
{
public:
	//-------------------------------------------------------------------------
	// �����
	//-------------------------------------------------------------------------
	INLINE BOOL	SetWorkingPackage(LPCWSTR szPackage);	// ���õ�һ����
	INLINE BOOL	SetWorkingPackage(LPCSTR szPackage);
	INLINE BOOL	HasWorkingPackage();

	INLINE BOOL SetHighResPackage(LPCSTR szPackage);	// ���ø����
	INLINE BOOL AddBackupPackage(LPCSTR szPackage);		// ��ӱ��ð�����΢��ʹ��
	INLINE BOOL CheckBackupPackage();	// ���΢�˰��Ƿ���ڣ��оͶ�

	//-------------------------------------------------------------------------
	// ���ļ���������
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
	// �����������
	//-------------------------------------------------------------------------
	INLINE BOOL	IsFileExist(LPCWSTR szFileName);
	INLINE BOOL	IsFileExist(LPCSTR szFileName);
	INLINE DWORD GetSize(LPCWSTR szFileName);
	INLINE DWORD GetSize(LPCSTR szFileName);
	INLINE DWORD GetSizeHR(LPCSTR szFileName);
	INLINE DWORD GetSize(HANDLE hHandle);
	INLINE BOOL	GetTime(LPCWSTR szFileName, LPFILETIME pFileTime);	// �õ��ļ�����޸�ʱ��
	INLINE BOOL	GetTime(LPCSTR szFileName, LPFILETIME pFileTime);	// �õ��ļ�����޸�ʱ��

	//-------------------------------------------------------------------------
	// �����е����ļ���Ŀ���ڴ棬����ԭ�ļ��ֽڴ�С��
	// ��Open+Read�����Ϊֱ�Ӷ���ָ���ڴ棬����һ��Memcpy
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
// ���õ�ǰ��
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

	// �����ΪNULL������ȡ����ǰ�Ĺ�����
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
// ���õ�ǰ��
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

	// �����ΪNULL������ȡ����ǰ�Ĺ�����
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
// ���ø����
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
// ��ӱ��ð�����΢��ʹ��
//-----------------------------------------------------------------------------
BOOL VFS::AddBackupPackage(LPCSTR szPackage)
{
	if( !VALID(m_pEPK[0]) )	// ����û�У�������Ҹ���
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
// ���΢�˰��Ƿ���ڣ��оͶ�
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
// ��ǰ�Ƿ��а���
//-----------------------------------------------------------------------------
BOOL VFS::HasWorkingPackage()
{
	return (VALID(m_pEPK[0]));
}


//-----------------------------------------------------------------------------
// ���ļ�
//-----------------------------------------------------------------------------
HANDLE VFS::Open(LPCWSTR szFileName)
{
	HANDLE hHandle = INVALID_HANDLE;
	if( m_pEPK[0] )	// �����ǰ�д򿪵İ�����Ӱ���
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
		// �����ǰû�д򿪰�����ӱ��ش��ļ�
		hHandle = m_pDiskIO->Create(szFileName, GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	}

	// д��Դ�ļ���־����������΢����Դ by ���
	if (g_bFileLog)
		m_pLog->WriteFileLog(szFileName, hHandle);

	return hHandle;
}

//-----------------------------------------------------------------------------
// ���ļ�
//-----------------------------------------------------------------------------
HANDLE VFS::Open(LPCSTR szFileName)
{
	HANDLE hHandle = INVALID_HANDLE;
	if( m_pEPK[0] )	// �����ǰ�д򿪵İ�����Ӱ���
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
		// �����ǰû�д򿪰�����ӱ��ش��ļ�
		hHandle = m_pDiskIO->Create(szFileName, GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	}

	// д��Դ�ļ���־����������΢����Դ by ���
	if (g_bFileLog)
		m_pLog->WriteFileLog(szFileName, hHandle);

	return hHandle;
}

//-----------------------------------------------------------------------------
// ��bink�ļ�
//-----------------------------------------------------------------------------
HANDLE VFS::OpenForBink(LPCWSTR szBink)
{
	if( m_pEPK[0] )	// �����ǰ�д򿪵İ�����Ӱ���
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
		// �����ǰû�д򿪰�����ӱ��ش��ļ�
		return m_pDiskIO->Create(szBink, GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	}
}


//-----------------------------------------------------------------------------
// �ر�bink�ļ�
//-----------------------------------------------------------------------------
VOID VFS::CloseForBink(HANDLE hHandle)
{
	ASSERT(hHandle);
	if( !VALID(m_pEPK[0]) )
		m_pDiskIO->Close(hHandle);
}


//-----------------------------------------------------------------------------
// �ر��ļ�
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
// ��ȡ�ļ�
//-----------------------------------------------------------------------------
DWORD VFS::Read(HANDLE hHandle, PVOID lpMemOut, DWORD dwSize)
{
	if( m_pEPK[0] )	// ���κ�EPK���ɽ��д˲���
		return m_pEPK[0]->Read(hHandle, lpMemOut, dwSize);
	else
		return m_pDiskIO->Read(hHandle, lpMemOut, dwSize);
}


//-----------------------------------------------------------------------------
// ��λ�ļ�
//-----------------------------------------------------------------------------
DWORD VFS::Seek(HANDLE hHandle, INT nOffset, DWORD dwOrigin)
{
	if( m_pEPK[0] )	// ���κ�EPK���ɽ��д˲���
		return m_pEPK[0]->Seek(hHandle, nOffset, dwOrigin);
	else
		return m_pDiskIO->Seek(hHandle, nOffset, dwOrigin);
}


//-----------------------------------------------------------------------------
// ��֪�ļ�ָ��λ��
//-----------------------------------------------------------------------------
DWORD VFS::Tell(HANDLE hHandle)
{
	if( m_pEPK[0] )
		return m_pEPK[0]->Tell(hHandle);
	else
		return m_pDiskIO->Seek(hHandle, 0, SEEK_CUR);
}


//-----------------------------------------------------------------------------
// �õ��ļ���С
//-----------------------------------------------------------------------------
DWORD VFS::GetSize(HANDLE hHandle)
{
	if( m_pEPK[0] )
		return m_pEPK[0]->GetSize(hHandle);
	else
		return m_pDiskIO->GetSize(hHandle);
}


//-----------------------------------------------------------------------------
// ����ļ��Ƿ����
//-----------------------------------------------------------------------------
BOOL VFS::IsFileExist(LPCWSTR szFileName)
{
	if( m_pEPK[0] )	// �����ǰ�д򿪵İ�����Ӱ���
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
// ����ļ��Ƿ����
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
// �õ��ļ���С
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

	// д��Դ�ļ���־����������΢����Դ by ���
	if (g_bFileLog)
		m_pLog->WriteFileLog(szFileName, (HANDLE)hHandle);

	return hHandle;
}

//-----------------------------------------------------------------------------
// �õ��ļ���С
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

	// д��Դ�ļ���־����������΢����Դ by ���
	if (g_bFileLog)
		m_pLog->WriteFileLog(szFileName, (HANDLE)hHandle);

	return hHandle;
}


//-----------------------------------------------------------------------------
// �õ��ļ���С
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

	// д��Դ�ļ���־����������΢����Դ by ���
	if (g_bFileLog)
		m_pLog->WriteFileLog(szFileName, (HANDLE)hHandle);

	return hHandle;
}



//-----------------------------------------------------------------------------
// �õ��ļ�����޸�ʱ��
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
// �õ��ļ�����޸�ʱ��
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
// ����ͨ�ļ���˵��һ���ģ����԰���˵����Ϊ����ѹ�����ݣ�������ȡ������һ���ڴ濽��
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

	// д��Դ�ļ���־����������΢����Դ by ���
	if (g_bFileLog)
		m_pLog->WriteFileLog(szFileName, (HANDLE)hHandle);

	return hHandle;
}

//-----------------------------------------------------------------------------
// ����ͨ�ļ���˵��һ���ģ����԰���˵����Ϊ����ѹ�����ݣ�������ȡ������һ���ڴ濽��
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

	// д��Դ�ļ���־����������΢����Դ by ���
	if (g_bFileLog)
		m_pLog->WriteFileLog(szFileName, (HANDLE)hHandle);

	return hHandle;
}


//-----------------------------------------------------------------------------
// ����ͨ�ļ���˵��һ���ģ����԰���˵����Ϊ����ѹ�����ݣ�������ȡ������һ���ڴ濽��
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

	// д��Դ�ļ���־����������΢����Դ by ���
	if (g_bFileLog)
		m_pLog->WriteFileLog(szFileName, (HANDLE)hHandle);

	return hHandle;
}
}