//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2003/12/8
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "log.h"

#include "disk_io.h"

namespace ECore {
#define MAX_WRITE_SIZE 65535
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL Log::Create(LPCWSTR szFileName)
{
	if( VALID(m_hFileHandle) )
		return TRUE;	// 已经创建

	// 如果没有提供文件名,就使用创建默认文件名
	if( !szFileName )
	{
		// 生成合适的log文件名
		WCHAR szTemp[MAX_PATH];
		GetModuleFileNameW(NULL, szTemp, MAX_PATH);

		// 截去路径
		WCHAR* pResult = wcsrchr(szTemp, L'\\');
		pResult = pResult ?	pResult+1 :	pResult = (WCHAR*)szTemp;
		swprintf_s(m_szFileName, L"log\\%s", pResult);

		// 截去文件扩展名
		pResult = wcsrchr(m_szFileName, L'.');
		if( pResult )
			*pResult = 0;

		// 加上扩展名
		wcscat(m_szFileName, L".log");
	}
	else
	{
		swprintf_s(m_szFileName, szFileName);
	}

	m_hFileHandle = m_pDiskIO->Create(m_szFileName, GENERIC_WRITE, 
		FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if( VALID(m_hFileHandle) )
	{
		// add M$ BOM
		BYTE BOM[2] = {0xFF, 0xFE};
		DWORD dwWrite = 0;
		m_pDiskIO->Write(m_hFileHandle, BOM, sizeof(BOM), &dwWrite, NULL);
		return TRUE;
	}

	return FALSE;
}

BOOL Log::Create(LPCSTR szFileName)
{
	if( VALID(m_hFileHandle) )
		return TRUE;	// 已经创建

	// 如果没有提供文件名,就使用创建默认文件名
	if( !szFileName )
	{
		// 生成合适的log文件名
		CHAR szTemp[MAX_PATH];
		GetModuleFileNameA(NULL, szTemp, MAX_PATH);

		// 截去路径
		CHAR* pResult = strrchr(szTemp, '\\');
		pResult = pResult ?	pResult+1 :	pResult = (CHAR*)szTemp;
		sprintf_s(m_szFileNameA, "log\\%s", pResult);

		// 截去文件扩展名
		pResult = strrchr(m_szFileNameA, '.');
		if( pResult )
			*pResult = '\0';

		// 加上扩展名
		strcat_s(m_szFileNameA, ".log");
	}
	else
	{
		sprintf_s(m_szFileNameA, szFileName);
	}

	m_hFileHandle = m_pDiskIO->Create(m_szFileNameA, GENERIC_WRITE, 
		FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if( VALID(m_hFileHandle) )
		return TRUE;

	return FALSE;
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
VOID Log::Close()
{
	if( VALID(m_hFileHandle) )
	{
		m_pDiskIO->Close(m_hFileHandle);
		m_hFileHandle = INVALID_HANDLE;
	}
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOL Log::Write(LPCWSTR lpFormat, ...)
{
	if( m_pDiskIO->IsInvalidHandle(m_hFileHandle) )
		return FALSE;

	WCHAR szBuf[MAX_WRITE_SIZE];

	ZeroMemory(szBuf, sizeof(szBuf));
	va_list argptr = NULL;
	va_start(argptr, lpFormat);
	vswprintf_s(szBuf, lpFormat, argptr);
	va_end(argptr);

	m_pDiskIO->Seek(m_hFileHandle, 0, SEEK_END);
	return m_pDiskIO->Write(m_hFileHandle, szBuf, (DWORD)wcslen(szBuf)*sizeof(WCHAR));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOL Log::Write(LPCSTR lpFormat, ...)
{
	if( m_pDiskIO->IsInvalidHandle(m_hFileHandle) )
		return FALSE;

	CHAR szBuf[MAX_WRITE_SIZE];

	ZeroMemory(szBuf, sizeof(szBuf));
	va_list argptr = NULL;
	va_start(argptr, lpFormat);
	vsprintf_s(szBuf, sizeof(szBuf)/sizeof(CHAR), lpFormat, argptr);
	va_end(argptr);

	m_pDiskIO->Seek(m_hFileHandle, 0, SEEK_END);
	return m_pDiskIO->Write(m_hFileHandle, szBuf, (DWORD)strlen(szBuf)*sizeof(CHAR));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
VOID Log::Flush()
{
	if( VALID(m_hFileHandle) )
		::FlushFileBuffers(m_hFileHandle);
}

//-----------------------------------------------------------------------------
// 写资源文件日志，用于整理微端资源 by 王炜
//-----------------------------------------------------------------------------
void Log::WriteFileLog(LPCSTR lpStr, HANDLE hHandle)
{
	if (!strstr(lpStr, "data\\"))
		return;

	char FileName[HUGE_STRING];
	if (INVALID_HANDLE == hHandle)
		strcpy_s(FileName, HUGE_STRING, "Log\\FileLost.log");
	else
		strcpy_s(FileName, HUGE_STRING, "Log\\FileLoad.log");

	m_FileLogLock.Lock();
	FILE* fp = fopen(FileName, "a+");
	if (fp)
	{
		fprintf(fp, lpStr);
		fprintf(fp, "\n");
		fflush(fp);
		fclose(fp);
		m_FileLogLock.Unlock();
	}
	else
	{
		m_FileLogLock.Unlock();
		ERR("can't open log file");
	}
}

//-----------------------------------------------------------------------------
// 写资源文件日志，用于整理微端资源 by 王炜
//-----------------------------------------------------------------------------
void Log::WriteFileLog(LPCWSTR lpWStr, HANDLE hHandle)
{
	char szStr[HUGE_STRING];
	WideCharToMultiByte(CP_ACP, 0, lpWStr, -1, szStr, HUGE_STRING, NULL, NULL);
	WriteFileLog(szStr, hHandle);
}

} // namespace ECore {


