//-----------------------------------------------------------------------------
// Auth: Lyp
// Data: 2003-1-2
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "disk_io.h"


namespace ECore {
//-----------------------------------------------------------------------------
// open or create a file
//-----------------------------------------------------------------------------
HANDLE DiskIO::Create(LPCWSTR lpFileName, DWORD dwDesiredAccess, 
	DWORD dwShareMode,  LPVOID lpSecurityAttributes, 
	DWORD dwCreationDisposition, 
	DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	ASSERT(lpFileName);

	return ::CreateFileW(lpFileName, dwDesiredAccess, dwShareMode,
		(LPSECURITY_ATTRIBUTES)lpSecurityAttributes, dwCreationDisposition,
		dwFlagsAndAttributes, hTemplateFile);
}

HANDLE DiskIO::Create(LPCSTR lpFileName, DWORD dwDesiredAccess, 
					  DWORD dwShareMode,  LPVOID lpSecurityAttributes, 
					  DWORD dwCreationDisposition, 
					  DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	ASSERT(lpFileName);

	return ::CreateFileA(lpFileName, dwDesiredAccess, dwShareMode,
		(LPSECURITY_ATTRIBUTES)lpSecurityAttributes, dwCreationDisposition,
		dwFlagsAndAttributes, hTemplateFile);
}

//-----------------------------------------------------------------------------
// close the file
//-----------------------------------------------------------------------------
BOOL DiskIO::Close(HANDLE hFileHandle)
{
	ASSERT(!IsInvalidHandle(hFileHandle));
	return CloseHandle(hFileHandle);

}

//-----------------------------------------------------------------------------
// open or create a file mapping, the file mapping's max size is 4G
//-----------------------------------------------------------------------------
HANDLE DiskIO::CreateMapping(HANDLE hFileHandle, LPVOID lpAttrib, 
	DWORD dwProtect, DWORD dwMaxSize, LPCWSTR lpName)
{
	return ::CreateFileMappingW(hFileHandle, (LPSECURITY_ATTRIBUTES)lpAttrib, 
		dwProtect, 0, dwMaxSize, lpName);
}

//-----------------------------------------------------------------------------
// open or create a file mapping, the file mapping's max size is 4G
//-----------------------------------------------------------------------------
HANDLE DiskIO::CreateMapping(HANDLE hFileHandle, LPVOID lpAttrib, 
	DWORD dwProtect, DWORD dwMaxSize, LPCSTR lpName)
{
	return ::CreateFileMappingA(hFileHandle, (LPSECURITY_ATTRIBUTES)lpAttrib, 
		dwProtect, 0, dwMaxSize, lpName);
}


//-----------------------------------------------------------------------------
// close a file mapping handle
//-----------------------------------------------------------------------------
BOOL DiskIO::CloseMappingHandle(HANDLE hFileMappingHandle)
{
	ASSERT(!IsInvalidHandle(hFileMappingHandle));
	return ::CloseHandle(hFileMappingHandle); 

}

//-----------------------------------------------------------------------------
// map view of file, return the base address, the max mapping size is 4G
//-----------------------------------------------------------------------------
LPVOID DiskIO::MapFile(HANDLE hFileMappingHandle, DWORD dwDesireAccess, 
	DWORD dwFileOffset, DWORD dwNumberOfBytesToMap)
{
	return ::MapViewOfFile(hFileMappingHandle, dwDesireAccess,
		0, dwFileOffset, (SIZE_T)dwNumberOfBytesToMap);
}


LPVOID DiskIO::MapFileEx(HANDLE hFileMappingHandle, DWORD dwDesireAccess, 
			DWORD dwFileOffset, DWORD dwNumberOfBytesToMap, LPVOID* ppMem)
{

	DWORD dwMapPos = dwFileOffset - (dwFileOffset % m_dwAllocGranularity);
	DWORD dwOffset = dwFileOffset - dwMapPos;
	LPVOID pMapAddress = 0;
	while( 0 == (pMapAddress = ::MapViewOfFile(hFileMappingHandle, dwDesireAccess, 0,
		dwMapPos, dwNumberOfBytesToMap + dwOffset)) )
		ERR("Can not map file");

	if( ppMem )
		*ppMem = (LPBYTE)pMapAddress + dwOffset;
	return pMapAddress;
}


//-----------------------------------------------------------------------------
// Unmap view of file, require the base address
//-----------------------------------------------------------------------------
BOOL DiskIO::UnMapFile(LPCVOID lpBaseAddress)
{
	return ::UnmapViewOfFile(lpBaseAddress);
}

//-----------------------------------------------------------------------------
// do file read
//-----------------------------------------------------------------------------
BOOL DiskIO::Read(HANDLE hFileHandle, LPVOID lpBuffer, 
	DWORD dwNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, 
	LPVOID lpOverlapped)
{
	ASSERT(!IsInvalidHandle(hFileHandle));
	ASSERT(lpBuffer);

	return ::ReadFile(hFileHandle, lpBuffer, dwNumberOfBytesToRead,
		lpNumberOfBytesRead, (LPOVERLAPPED)lpOverlapped);

}

//-----------------------------------------------------------------------------
// do file read
//-----------------------------------------------------------------------------
DWORD DiskIO::Read(HANDLE hFileHandle, LPVOID lpBuffer, 
	DWORD dwNumberOfBytesToRead)
{
	DWORD dwNumberOfBytesRead = 0;
	BOOL bResult = FALSE;

	ASSERT(!IsInvalidHandle(hFileHandle));
	ASSERT(lpBuffer);

	bResult = ::ReadFile(hFileHandle, lpBuffer, dwNumberOfBytesToRead,
		&dwNumberOfBytesRead, NULL);

	return dwNumberOfBytesRead;
}

//-----------------------------------------------------------------------------
// do file write
//-----------------------------------------------------------------------------
BOOL DiskIO::Write(HANDLE hFileHandle, LPCVOID lpBuffer, 
	DWORD dwNumberOfBytesToWrite, 
	LPDWORD lpNumberOfBytesWritten, LPVOID lpOverlapped)
{
	ASSERT(!IsInvalidHandle(hFileHandle));
	ASSERT(lpBuffer);

	return ::WriteFile(hFileHandle, lpBuffer, dwNumberOfBytesToWrite,
		lpNumberOfBytesWritten, (LPOVERLAPPED)lpOverlapped);
}




//-----------------------------------------------------------------------------
// do file write
//-----------------------------------------------------------------------------
BOOL DiskIO::Write(HANDLE hFileHandle, LPCVOID lpBuffer, 
	DWORD dwNumberOfBytesToWrite)
{
	DWORD dwNumberOfBytesWritten = 0;
	BOOL bResult = FALSE;

	ASSERT(!IsInvalidHandle(hFileHandle));
	ASSERT(lpBuffer);

	bResult = ::WriteFile(hFileHandle, lpBuffer, dwNumberOfBytesToWrite,
		&dwNumberOfBytesWritten, NULL) == TRUE;

	if( dwNumberOfBytesWritten != dwNumberOfBytesToWrite )
		ERR(_T("Error in write file"));

	return bResult;
}

//-----------------------------------------------------------------------------
// set file pointer
//-----------------------------------------------------------------------------
DWORD DiskIO::Seek(HANDLE hFileHandle, DWORD dwDistanceToMove, DWORD dwMoveMethod)
{
	ASSERT(!IsInvalidHandle(hFileHandle));
	LARGE_INTEGER li, liOut;
	li.HighPart = 0;
	li.LowPart = dwDistanceToMove;
	liOut.HighPart = 0;
	liOut.LowPart = dwDistanceToMove;
	SetFilePointerEx(hFileHandle, li, &liOut, dwMoveMethod);
	return liOut.LowPart;
}



//-----------------------------------------------------------------------------
// set file pointer from beginning of the file
//-----------------------------------------------------------------------------
DWORD DiskIO::Seek(HANDLE hFileHandle, DWORD dwOffsetFromStart)
{
	ASSERT(!IsInvalidHandle(hFileHandle));
	LARGE_INTEGER li, liOut;
	li.HighPart = 0;
	li.LowPart = dwOffsetFromStart;
	liOut.HighPart = 0;
	liOut.LowPart = dwOffsetFromStart;
	SetFilePointerEx(hFileHandle, li, &liOut, FILE_BEGIN);
	return liOut.LowPart;
}


//-----------------------------------------------------------------------------
// set file pointer
//-----------------------------------------------------------------------------
BOOL DiskIO::SeekEx(HANDLE hFileHandle, INT64 n64DistanceToMove, DWORD dwMoveMethod)
{
	ASSERT(!IsInvalidHandle(hFileHandle));

	return SetFilePointerEx(hFileHandle, *(LARGE_INTEGER*)&n64DistanceToMove, NULL, dwMoveMethod);
}

//-----------------------------------------------------------------------------
// get length of the file
//-----------------------------------------------------------------------------
DWORD DiskIO::GetSize(LPCWSTR pFileName)
{
	DWORD dwFileSize = 0;
	HANDLE hHandle = ::CreateFileW(pFileName, GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);

	if( IsInvalidHandle(hHandle) )
		return (DWORD)INVALID;	// 如果文件不存在，则返回GT_INVALID

	dwFileSize = ::GetFileSize(hHandle, NULL);
	CloseHandle(hHandle);
	return dwFileSize;
}


DWORD DiskIO::GetSize(LPCSTR pFileName)
{
	DWORD dwFileSize = 0;
	HANDLE hHandle = ::CreateFileA(pFileName, GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);

	if( IsInvalidHandle(hHandle) )
		return (DWORD)INVALID;	// 如果文件不存在，则返回GT_INVALID

	dwFileSize = ::GetFileSize(hHandle, NULL);
	CloseHandle(hHandle);
	return dwFileSize;
}

//-----------------------------------------------------------------------------
// get length of the file
//-----------------------------------------------------------------------------
DWORD DiskIO::GetSize(HANDLE hFileHandle)
{
	ASSERT(!IsInvalidHandle(hFileHandle));
	return ::GetFileSize(hFileHandle, NULL);

}

//-----------------------------------------------------------------------------
// get length of the file
//-----------------------------------------------------------------------------
BOOL DiskIO::GetSizeEx(HANDLE hFileHandle, INT64* pn64Size)
{
	ASSERT(!IsInvalidHandle(hFileHandle));
	return ::GetFileSizeEx(hFileHandle, (PLARGE_INTEGER)pn64Size);
}

//-----------------------------------------------------------------------------
// the file may be truncate or extend, if succeeds, return nonzero
//-----------------------------------------------------------------------------
DWORD DiskIO::SetEOF(HANDLE hFileHandle)
{
	ASSERT(!IsInvalidHandle(hFileHandle));
	return ::SetEndOfFile(hFileHandle);
}

//-------------------------------------------------------------------------
// 得到文件最后修改日期
//-------------------------------------------------------------------------
BOOL DiskIO::GetTime(LPCWSTR szFileName, LPFILETIME lpLastWriteTime)
{
	DWORD dwFileSize = 0;
	HANDLE hHandle = ::CreateFileW(szFileName, GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);

	if( IsInvalidHandle(hHandle) )
		return FALSE;	// 如果文件不存在，则返回FALSE

	::GetFileTime(hHandle, NULL, NULL, lpLastWriteTime);
	CloseHandle(hHandle);
	return TRUE;
}


BOOL DiskIO::GetTime(LPCSTR szFileName, LPFILETIME lpLastWriteTime)
{
	DWORD dwFileSize = 0;
	HANDLE hHandle = ::CreateFileA(szFileName, GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);

	if( IsInvalidHandle(hHandle) )
		return FALSE;	// 如果文件不存在，则返回FALSE

	::GetFileTime(hHandle, NULL, NULL, lpLastWriteTime);
	CloseHandle(hHandle);
	return TRUE;
}



//-----------------------------------------------------------------------------
// check the file whether exist
//-----------------------------------------------------------------------------
BOOL DiskIO::IsFileExist(LPCWSTR pFileName)
{		
	ASSERT(pFileName);
	WIN32_FIND_DATAW FindData;
	BOOL bResult = TRUE;

	HANDLE hHandle = ::FindFirstFileW(pFileName, &FindData);
	if( IsInvalidHandle(hHandle) ) return FALSE;

	if( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		bResult = FALSE;

	::FindClose(hHandle);
	return bResult;
}

BOOL DiskIO::IsFileExist(LPCSTR pFileName)
{		
	ASSERT(pFileName);
	WIN32_FIND_DATAA FindData;
	BOOL bResult = TRUE;

	HANDLE hHandle = ::FindFirstFileA(pFileName, &FindData);
	if( IsInvalidHandle(hHandle) ) return FALSE;

	if( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		bResult = FALSE;

	::FindClose(hHandle);
	return bResult;
}

//-----------------------------------------------------------------------------
// delete a file
//-----------------------------------------------------------------------------
BOOL DiskIO::DeleteFile( LPCWSTR pFileName )
{
	return ::DeleteFileW(pFileName);
}


//-----------------------------------------------------------------------------
// delete a file
//-----------------------------------------------------------------------------
BOOL DiskIO::DeleteFile( LPCSTR pFileName )
{
	return ::DeleteFileA(pFileName);
}

//-----------------------------------------------------------------------------
// is filename refer to a directory
//-----------------------------------------------------------------------------
BOOL DiskIO::IsDir(LPCWSTR pFileName)
{
	ASSERT(pFileName);
	WIN32_FIND_DATAW FindData;
	BOOL bResult = FALSE;

	HANDLE hHandle = ::FindFirstFileW(pFileName, &FindData);
	if( IsInvalidHandle(hHandle) ) return FALSE;

	if( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		bResult = TRUE;

	::FindClose(hHandle);
	return bResult;
}

//-----------------------------------------------------------------------------
// is filename refer to a directory
//-----------------------------------------------------------------------------
BOOL DiskIO::IsDir(LPCSTR pFileName)
{
	ASSERT(pFileName);
	WIN32_FIND_DATAA FindData;
	BOOL bResult = FALSE;

	HANDLE hHandle = ::FindFirstFileA(pFileName, &FindData);
	if( IsInvalidHandle(hHandle) ) return FALSE;

	if( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		bResult = TRUE;

	::FindClose(hHandle);
	return bResult;
}

//-----------------------------------------------------------------------------
// 使用此函数比使用open\read要快速
//-----------------------------------------------------------------------------
DWORD DiskIO::LoadToMem(LPVOID pMem, LPCWSTR pFileName)
{
	ASSERT(pFileName);

	HANDLE hHandle = ::CreateFileW(pFileName, GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN |
		FILE_FLAG_RANDOM_ACCESS, NULL);

	if( IsInvalidHandle(hHandle) ) return (DWORD)INVALID;

	DWORD dwSize = ::GetFileSize(hHandle, NULL);
	if( dwSize == INVALID )
	{ 
		ERR(_T("Couldn't get file size : %s"), pFileName);
		CloseHandle(hHandle);
		return (DWORD)INVALID;
	}

	if( NULL == pMem )	// 外部调用只需要返回文件大小
	{
		CloseHandle(hHandle);
		return dwSize;
	}

	DWORD dwBytesRead = 0;
	BOOL bResult = ::ReadFile(hHandle, pMem, dwSize, &dwBytesRead, NULL);
	if( bResult == FALSE || dwBytesRead != dwSize )
	{
		ERR(_T("Couldn't read file : %s"), pFileName);
		CloseHandle(hHandle);
		return (DWORD)INVALID;
	}

	CloseHandle(hHandle);
	return dwSize;
}


DWORD DiskIO::LoadToMem(LPVOID pMem, LPCSTR pFileName)
{
	ASSERT(pFileName);

	HANDLE hHandle = ::CreateFileA(pFileName, GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN |
		FILE_FLAG_RANDOM_ACCESS, NULL);

	if( IsInvalidHandle(hHandle) ) return (DWORD)INVALID;

	DWORD dwSize = ::GetFileSize(hHandle, NULL);
	if( dwSize == INVALID )
	{ 
		ERR(_T("Couldn't get file size : %s"), pFileName);
		CloseHandle(hHandle);
		return (DWORD)INVALID;
	}

	if( NULL == pMem )	// 外部调用只需要返回文件大小
	{
		CloseHandle(hHandle);
		return dwSize;
	}

	DWORD dwBytesRead = 0;
	BOOL bResult = ::ReadFile(hHandle, pMem, dwSize, &dwBytesRead, NULL);
	if( bResult == FALSE || dwBytesRead != dwSize )
	{
		ERR(_T("Couldn't read file : %s"), SS::To16(pFileName));
		CloseHandle(hHandle);
		return (DWORD)INVALID;
	}

	CloseHandle(hHandle);
	return dwSize;
}


//-----------------------------------------------------------------------------
// find 
//-----------------------------------------------------------------------------
HANDLE DiskIO::FindFirstFile(LPCTSTR pFileName, LPVOID lpFindData)
{
	ASSERT(pFileName);
	ASSERT(lpFindData);

	return ::FindFirstFile(pFileName, (LPWIN32_FIND_DATA)lpFindData);
}



//-----------------------------------------------------------------------------
// find next
//-----------------------------------------------------------------------------
BOOL DiskIO::FindNextFile(HANDLE hFileHandle, LPVOID lpFindData)
{
	ASSERT(!IsInvalidHandle(hFileHandle));
	ASSERT(lpFindData);

	return ::FindNextFile(hFileHandle, (LPWIN32_FIND_DATA)lpFindData);
}

//-----------------------------------------------------------------------------
// close find handle
//-----------------------------------------------------------------------------
BOOL DiskIO::FindClose(HANDLE hFileHandle)
{
	ASSERT(!IsInvalidHandle(hFileHandle));
	return ::FindClose(hFileHandle); 
}

//-----------------------------------------------------------------------------
// GetFileNumInDir
//-----------------------------------------------------------------------------
DWORD DiskIO::GetFileNumInDir(LPCTSTR pFullName, DWORD& dwTotalBytes)
{
	DWORD dwFileNum = 0;
	ASSERT(pFullName != NULL);

	GetFileNumInDir(pFullName, dwFileNum, dwTotalBytes);
	return dwFileNum;
}


//-----------------------------------------------------------------------------
// 注意!此函数存在问题,需要修正!
//-----------------------------------------------------------------------------
VOID DiskIO::GetFileNumInDir(LPCTSTR pFullName, DWORD& dwFileNum, DWORD& dwBytes)
{
	TCHAR szTemp[MAX_PATH];
	WIN32_FIND_DATA FindData;
	HANDLE hHandle = INVALID_HANDLE_VALUE;

	dwFileNum++;
	_tcsncpy(szTemp, pFullName, MAX_PATH);
	_tcscat(szTemp, _T("\\*.*"));

	hHandle = ::FindFirstFile(szTemp, &FindData);
	if( !IsInvalidHandle(hHandle) )
	{
		while( 0 != ::FindNextFile(hHandle, &FindData) )
		{
			if( FindData.cFileName[0] != _T('.') )
			{
				_tcsncpy(szTemp, pFullName, MAX_PATH);
				_tcscat(szTemp, _T("\\"));
				_tcscat(szTemp, FindData.cFileName);
				GetFileNumInDir(szTemp, dwFileNum, dwBytes);
			}
		}

		::FindClose(hHandle);
	}
	else
	{	/*// 计算字节数太慢了
		hHandle = INVALID_HANDLE_VALUE;
		hHandle = CreateFile(pFullName, GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);

		if (hHandle == INVALID_HANDLE_VALUE)
		return;

		dwBytes += ::GetFileSize(hHandle, NULL);
		CloseHandle(hHandle);
		/**/
	}
}


//-------------------------------------------------------------------------------
// 得到磁盘的剩余空间
//-------------------------------------------------------------------------------
INT64 DiskIO::GetDriveFreeSpace(LPCTSTR szDriver)
{
	INT64 n64FreeBytesToUse = 0;
	typedef INT( WINAPI *GETDISKFREESPACE_PROC)(LPCTSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);
	GETDISKFREESPACE_PROC pGetDiskFreeSpaceEx = 
		(GETDISKFREESPACE_PROC)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetDiskFreeSpaceExA");

	if( pGetDiskFreeSpaceEx )
	{
		ULARGE_INTEGER TotalNumberOfBytes;
		ULARGE_INTEGER TotalNumberOfFreeBytes;

		(pGetDiskFreeSpaceEx)(szDriver,
			(PULARGE_INTEGER)&n64FreeBytesToUse,
			(PULARGE_INTEGER)&TotalNumberOfBytes,
			(PULARGE_INTEGER)&TotalNumberOfFreeBytes);
	}
	else 
	{
		DWORD dwSectPerClust = 0;
		DWORD dwBytesPerSect = 0;
		DWORD dwFreeClusters = 0;
		DWORD dwTotalClusters = 0;

		GetDiskFreeSpace(szDriver, 
			&dwSectPerClust, 
			&dwBytesPerSect,
			&dwFreeClusters, 
			&dwTotalClusters);

		// Process GetDiskFreeSpace results.
		n64FreeBytesToUse = (INT64)dwFreeClusters * dwBytesPerSect * dwSectPerClust;
	}

	return n64FreeBytesToUse;
}


//-------------------------------------------------------------------------------
// 清空目录下的所有文件
//-------------------------------------------------------------------------------
BOOL DiskIO::ClearDirectoryFiles( LPCWSTR szDirName )
{
	WCHAR szTemp[MAX_PATH];
	WIN32_FIND_DATAW FindData;
	HANDLE hHandle = INVALID_HANDLE_VALUE;

	wcsncpy(szTemp, szDirName, MAX_PATH);
	wcscat(szTemp, L"\\*.*");

	hHandle = ::FindFirstFileW(szTemp, &FindData);
	if( IsInvalidHandle(hHandle) )
		return TRUE;

	while( 0 != ::FindNextFileW(hHandle, &FindData) )
	{
		if( FindData.cFileName[0] != L'.' )
		{
			wcsncpy(szTemp, szDirName, MAX_PATH);
			wcscat(szTemp, L"\\");
			wcscat(szTemp, FindData.cFileName);

			if( IsDir(szTemp) )
			{
				if( !ClearDirectoryFiles(szTemp) )
				{
					::FindClose(hHandle);
					return FALSE;
				}
			}
			else
			{
				if( !DeleteFileW(szTemp) )
				{
					::FindClose(hHandle);
					return FALSE;
				}
			}
		}
	}

	::FindClose(hHandle);
	return TRUE;
}



//-------------------------------------------------------------------------------
// 清空目录下的所有文件
//-------------------------------------------------------------------------------
BOOL DiskIO::ClearDirectoryFiles( LPCSTR szDirName )
{
	CHAR szTemp[MAX_PATH];
	WIN32_FIND_DATAA FindData;
	HANDLE hHandle = INVALID_HANDLE_VALUE;

	strcpy_s(szTemp, szDirName);
	strcat_s(szTemp, "\\*.*");

	hHandle = ::FindFirstFileA(szTemp, &FindData);
	if( IsInvalidHandle(hHandle) )
		return TRUE;

	while( 0 != ::FindNextFileA(hHandle, &FindData) )
	{
		if( FindData.cFileName[0] != '.' )
		{
			strcpy_s(szTemp, szDirName);
			strcat_s(szTemp, "\\");
			strcat_s(szTemp, FindData.cFileName);

			if( IsDir(szTemp) )
			{
				if( !ClearDirectoryFiles(szTemp) )
				{
					::FindClose(hHandle);
					return FALSE;
				}
			}
			else
			{
				if( !DeleteFile(szTemp) )
				{
					::FindClose(hHandle);
					return FALSE;
				}
			}
		}
	}

	::FindClose(hHandle);
	return TRUE;
}



//-------------------------------------------------------------------------------
// 构造
//-------------------------------------------------------------------------------
DiskIO::DiskIO()
{
	SYSTEM_INFO si;
	::GetSystemInfo(&si);
	m_dwAllocGranularity = si.dwAllocationGranularity;	// 得到系统内存分配粒度
}


} // namespace ECore{
