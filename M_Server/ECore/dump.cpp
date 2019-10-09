//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2003/12/11
// Last: 2004/3/7
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "dump.h"

//#include "net_util.h"

namespace ECore {
MINIDUMP_EXCEPTION_INFORMATION	Dump::m_info;
HANDLE Dump::m_hFile = 0;
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Dump::Dump(UINT unCode, EXCEPTION_POINTERS* pExp)
{
	m_hFile = ::CreateFileA(DUMP_FILE_NAME, GENERIC_WRITE, 
		FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	m_info.ThreadId = GetCurrentThreadId();
	m_info.ExceptionPointers = pExp;
	m_info.ClientPointers = FALSE;
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), m_hFile,  MiniDumpNormal , 
		&m_info, NULL, NULL);

	::CloseHandle(m_hFile);

	
	// 生成合适的log文件名
	TCHAR szName[MAX_PATH], szPath[MAX_PATH];
	GetModuleFileName(NULL, szName, MAX_PATH);
	GetModuleFileName(NULL, szPath, MAX_PATH);

	// 得到参数
	TCHAR* pResult = _tcsrchr(szName, _T('\\'));
	pResult = pResult ?	pResult+1 :	pResult = (TCHAR*)szName;
	tstring strParam = pResult;	// EXE简单文件名
	strParam += _T(" ");
	strParam += TObj<Log>()->GetFileName();	// 加上log文件名
	strParam += _T(" ");	// 加上svn版本号
	strParam += SS::ToW(g_dwSVN);	// 加上svn版本号
	TObj<Log>()->Flush();

	// 得到Exe路径
	pResult = _tcsrchr(szPath, _T('\\'));
	pResult = pResult ?	pResult+1 :	pResult = (TCHAR*)szName;
	*pResult = 0;


	// 调用CrashReport
	ShellExecute(NULL, _T("open"), _T("CrashReport.exe"), strParam.c_str(), szPath, SW_SHOWNORMAL);

	return;
}



} // namespace ECore {











