//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2003/12/01
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "error.h"

namespace ECore {

//-----------------------------------------------------------------------------
// 显示一般调试消息，并获取系统出错信息
//-----------------------------------------------------------------------------
INT Error::Msg(LPCWSTR lpFormat, ...)
{
	WCHAR	szBuf[1024]		=	{0};
	LPVOID	lpMsgBuf		=	NULL;
	DWORD	dwErr			=	0;
	INT		nResult			=	0;

	lpMsgBuf = NULL;

	// 首先获得系统错误代码
	dwErr = ::GetLastError();

	va_list argptr;
	va_start(argptr, lpFormat);
	vswprintf_s(szBuf, lpFormat, argptr);
	va_end(argptr);

	if( dwErr != 0 )	// 从系统代码得到错误信息
	{
		FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM 
			| FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwErr,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPWSTR)&lpMsgBuf, 0, NULL);
		swprintf_s(szBuf, L"%s\r\nSystemMsg<%lu>:%s", szBuf, dwErr, (CONST PWCHAR)lpMsgBuf);
		LocalFree(lpMsgBuf);
	}

	m_pLog->Write(szBuf);

	swprintf_s(szBuf, L"%s\r\nContinue?", szBuf);
	nResult = ::MessageBoxW(NULL, szBuf, L" Message:", MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON1 | MB_TOPMOST);
	if( IDNO == nResult )	// 重新抛出一个结构化异常，以便记录当前堆栈等信息
	{
		// Microsoft C++ exception code
		CONST UINT C_PLUS_PLUS_EXCEPTION	= 0xe06d7363;
		RaiseException(C_PLUS_PLUS_EXCEPTION, 0, 0, NULL);
	}
	
	return nResult;
}


INT Error::Msg(LPCSTR lpFormat, ...)
{
	CHAR	szBuf[HUGE_STRING] = {0};
	LPVOID	lpMsgBuf = NULL;
	DWORD	dwErr = 0;
	INT		nResult = 0;

	// 首先获得系统错误代码
	dwErr = ::GetLastError();

	va_list argptr;
	va_start(argptr, lpFormat);
	vsprintf_s(szBuf, lpFormat, argptr);
	va_end(argptr);

	if( dwErr != 0 )	// 从系统代码得到错误信息
	{
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM 
			| FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwErr,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPSTR)&lpMsgBuf, 0, NULL);
		sprintf_s(szBuf, "%s\r\nSystemMsg<%lu>:%s", szBuf, dwErr, (CONST PCHAR)lpMsgBuf);
		LocalFree(lpMsgBuf);
	}

	m_pLog->Write(szBuf);

	sprintf_s(szBuf, "%s\r\nContinue?", szBuf);
	nResult = ::MessageBoxA(NULL, szBuf, " Message:", MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON1 | MB_TOPMOST);
	if( IDNO == nResult )	// 重新抛出一个结构化异常，以便记录当前堆栈等信息
	{
		// Microsoft C++ exception code
		CONST UINT C_PLUS_PLUS_EXCEPTION	= 0xe06d7363;
		RaiseException(C_PLUS_PLUS_EXCEPTION, 0, 0, NULL);
	}

	return nResult;
}

//-----------------------------------------------------------------------------
// 显示一般调试消息，并抛出异常
//-----------------------------------------------------------------------------
INT Error::Fault(LPCTSTR lpFormat, ...)
{
	TCHAR	szBuf[1024]	=	{0};
	LPVOID	lpMsgBuf	=	NULL;
	DWORD	dwErr		=	0;
	INT		nResult		=	0;

	lpMsgBuf = NULL;

	// 首先获得系统错误代码
	dwErr = ::GetLastError();

	va_list argptr;
	va_start(argptr, lpFormat);
	_vstprintf_s(szBuf, lpFormat, argptr);
	va_end(argptr);

	if( dwErr != 0 )	// 从系统代码得到错误信息
	{
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM 
			| FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwErr,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf, 0, NULL);
		_stprintf_s(szBuf, _T("%s\r\nSystemMsg<%lu>:%s\r\n"), szBuf, dwErr, (CONST PTCHAR)lpMsgBuf);
		LocalFree(lpMsgBuf);
	}

	nResult = ::MessageBox(NULL, szBuf, _T(" Message:"), MB_ICONWARNING | MB_OK | MB_DEFBUTTON1 | MB_TOPMOST);

	m_pLog->Write(szBuf);
	
	// 抛出一个结构化异常，以便记录当前堆栈等信息
	// Microsoft C++ exception code
	CONST UINT C_PLUS_PLUS_EXCEPTION	= 0xe06d7363;
	RaiseException(C_PLUS_PLUS_EXCEPTION, 0, 0, NULL);

	return 0;
}

//-----------------------------------------------------------------------------
// 向IDE输出调试信息
//-----------------------------------------------------------------------------
VOID Trace(LPCTSTR lpszFormat, ...)
{
	va_list args;
	TCHAR szBuffer[512];

	va_start(args, lpszFormat);
	_vstprintf_s(szBuffer, _countof(szBuffer), lpszFormat, args); 
	va_end(args);

	::OutputDebugString(szBuffer);
}


} // namespace ECore{
