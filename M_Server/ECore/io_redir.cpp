//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2007/11/14
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "io_redir.h"

namespace ECore{
//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL IORedir::Init()
{
	HANDLE hStdinRd, hStdinWr, hStdoutRd, hStdoutWr;

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	// Save the handle to the current STDOUT. 
	HANDLE hSaveStdout = GetStdHandle(STD_OUTPUT_HANDLE); 

	// Create a pipe for the child process's STDOUT. 
	if( !CreatePipe(&hStdoutRd, &hStdoutWr, &sa, 0) ) 
	{
		ERR(_T("Stdout pipe creation failed")); 
		return FALSE;
	}

	// Set a write handle to the pipe to be STDOUT. 
	if( !SetStdHandle(STD_OUTPUT_HANDLE, hStdoutWr) ) 
	{
		ERR(_T("Redirecting STDOUT failed"));
		return FALSE;
	}

	// Create noninheritable read handle and close the inheritable read handle. 
	BOOL bRet = DuplicateHandle(GetCurrentProcess(), hStdoutRd,
		GetCurrentProcess(), &m_hStdoutRdDup , 0, FALSE, DUPLICATE_SAME_ACCESS);
	if( !bRet )
	{	
		ERR(_T("DuplicateHandle failed"));
		return FALSE;
	}
	CloseHandle(hStdoutRd);

	// The steps for redirecting child process's STDIN: 
	// Save the handle to the current STDIN. 
	HANDLE hSaveStdin = GetStdHandle(STD_INPUT_HANDLE); 

	// Create a pipe for the child process's STDIN. 
	if( ! CreatePipe(&hStdinRd, &hStdinWr, &sa, 0) )
	{
		ERR(_T("Stdin pipe creation failed"));
		return FALSE;
	}

	// Set a read handle to the pipe to be STDIN. 
	if( !SetStdHandle(STD_INPUT_HANDLE, hStdinRd) ) 
	{
		ERR(_T("Redirecting Stdin failed")); 
		return FALSE;
	}

	// Duplicate the write handle to the pipe so it is not inherited. 
	bRet = DuplicateHandle(GetCurrentProcess(), hStdinWr, 
		GetCurrentProcess(), &m_hStdinWrDup, 0, FALSE,
		DUPLICATE_SAME_ACCESS); 
	if( !bRet )
	{
		ERR(_T("DuplicateHandle failed")); 
		return FALSE;
	}

	CloseHandle(hStdinWr); 

	// Now create the child process
	PROCESS_INFORMATION pi;
	STARTUPINFO start_info = { sizeof(STARTUPINFO),NULL, _T(""),
		NULL,0,0,0,0,0,0,0,STARTF_USESHOWWINDOW,0,0,NULL,0,0,0};
	start_info.hStdInput = hStdinRd;
	start_info.hStdOutput = hStdoutWr;
	start_info.hStdError = hStdoutWr;
	start_info.wShowWindow = SW_SHOW;
	start_info.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	if( !CreateProcess(NULL, _T("cmd.exe"), &sa, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &start_info, &pi) )
	{
		ERR(TEXT("CreateProcess failed"));
		return FALSE;
	}
	m_hProcess = pi.hProcess;

	// After process creation, restore the saved STDIN and STDOUT. 
	if( !SetStdHandle(STD_INPUT_HANDLE, hSaveStdin) ) 
	{
		ERR(_T("Re-redirecting Stdin failed")); 
		return FALSE;
	}

	if( !SetStdHandle(STD_OUTPUT_HANDLE, hSaveStdout) ) 
	{
		ERR(_T("Re-redirecting Stdout failed"));
		return FALSE;
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID IORedir::Destroy()
{
	TerminateProcess(m_hProcess, 0);
}



//-----------------------------------------------------------------------------
// recv
//-----------------------------------------------------------------------------
LPBYTE IORedir::Recv(DWORD& dwSize)
{
	DWORD dwAvail = 0;
	PeekNamedPipe(m_hStdoutRdDup, 0, 0,0, &dwAvail, 0);
	if( dwAvail <= 0 )
		return NULL;

	LPBYTE pByte = new BYTE[LONG_STRING];
	ZeroMemory(pByte, LONG_STRING);
	if( dwAvail > LONG_STRING-1 )
		dwAvail = LONG_STRING-1;

	ReadFile(m_hStdoutRdDup, pByte, dwAvail, &dwSize, NULL);
	return pByte;

}


//-----------------------------------------------------------------------------
// send
//-----------------------------------------------------------------------------
BOOL IORedir::Send(LPVOID pMsg, DWORD dwMsgSize)
{
	DWORD dwWrite = 0;
	WriteFile(m_hStdinWrDup, pMsg, dwMsgSize, &dwWrite, NULL);
	return (dwWrite == dwMsgSize);
}


//-----------------------------------------------------------------------------
// 对应于Recv(),因为内存是内部分配的，所以应该交还内部来释放
//-----------------------------------------------------------------------------
VOID IORedir::FreeRecved(LPBYTE pMsg)
{
	ASSERT(NULL != pMsg);
	delete[](pMsg);
}


} // namespace ECore {