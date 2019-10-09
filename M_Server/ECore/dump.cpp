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

	
	// ���ɺ��ʵ�log�ļ���
	TCHAR szName[MAX_PATH], szPath[MAX_PATH];
	GetModuleFileName(NULL, szName, MAX_PATH);
	GetModuleFileName(NULL, szPath, MAX_PATH);

	// �õ�����
	TCHAR* pResult = _tcsrchr(szName, _T('\\'));
	pResult = pResult ?	pResult+1 :	pResult = (TCHAR*)szName;
	tstring strParam = pResult;	// EXE���ļ���
	strParam += _T(" ");
	strParam += TObj<Log>()->GetFileName();	// ����log�ļ���
	strParam += _T(" ");	// ����svn�汾��
	strParam += SS::ToW(g_dwSVN);	// ����svn�汾��
	TObj<Log>()->Flush();

	// �õ�Exe·��
	pResult = _tcsrchr(szPath, _T('\\'));
	pResult = pResult ?	pResult+1 :	pResult = (TCHAR*)szName;
	*pResult = 0;


	// ����CrashReport
	ShellExecute(NULL, _T("open"), _T("CrashReport.exe"), strParam.c_str(), szPath, SW_SHOWNORMAL);

	return;
}



} // namespace ECore {











