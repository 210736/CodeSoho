// MoniClient.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MoniClient.h"
#include "MoniClientDlg.h"

#include <tlhelp32.h> //20100601 Ding Yiming Add

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMoniClientApp

BEGIN_MESSAGE_MAP(CMoniClientApp, CWinApp)
	//{{AFX_MSG_MAP(CMoniClientApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMoniClientApp construction

CMoniClientApp::CMoniClientApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMoniClientApp object

CMoniClientApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMoniClientApp initialization

BOOL CMoniClientApp::InitInstance()
{
	//20100601 Ding Yiming Add
	if(IsAlreadyRun())
	{
		AfxMessageBox(_T("系统监控代理已启动!"));
		exit(0);
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CMoniClientDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

//-------------------------------------------------------------------------
// Function Name    :IsAlreadyRun
// Parameter(s)     :
// Return           :BOOL
// Create			:2010-06-01 09:47 Ding Yiming
// Memo             :判断软件进程是否已经运行
//-------------------------------------------------------------------------
bool CMoniClientApp::IsAlreadyRun(void)
{

	HANDLE handle=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	PROCESSENTRY32* info=new PROCESSENTRY32;
	info->dwSize=sizeof(PROCESSENTRY32);
	int numofthisprocess=0;
	if(Process32First(handle,info))
	{
		if(GetLastError()==ERROR_NO_MORE_FILES )
		{
			AfxMessageBox("No More Process");
		}
		else
		{
			CString id;
			id=info->szExeFile;
			CString exeName=AfxGetApp()->m_pszExeName;
			exeName+=".exe";

			
			if(id.CompareNoCase(exeName)==0)
		//	if(exeName.CompareNoCase(info->szExeFile)==0)
			{
				numofthisprocess++;
				if(numofthisprocess>=2)
				{

					delete info;
					return true;
				}
			}
			while(Process32Next(handle,info)!=FALSE)
			{
			//	id.Format("%s",info->szExeFile);
				id=info->szExeFile;
				if(id.CompareNoCase(exeName)==0)
				//if(exeName.CompareNoCase(info->szExeFile)==0)
				{
					numofthisprocess++;
					if(numofthisprocess>=2)
					{
						delete info;
						return true;
					}

				}
			}
		}
	}

	delete info;
	CloseHandle(handle);
	return false;
}