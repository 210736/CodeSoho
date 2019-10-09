// M_ServerTest.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "M_ServerTest.h"
#include <process.h>
#include <objbase.h>
#include "../Ecore/Ecore.h"
#include "../MConsole/MConsole.h"

unsigned int __stdcall ThreadUpdate(void* p);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
//	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, (&ThreadUpdate), NULL, 0, NULL);
	// IF EXIST "$(SolutionDir)\Mconsole.dll" xcopy /Q /E /Y /I "$(SolutionDir)\Mconsole.dll" "$(SolutionDir)\x64\$(SolutionDir)"
	// IF EXIST "E:\M_Server\QTProject\MConsole\debug\Mconsole.dll" xcopy /Q /E /Y /I "E:\M_Server\QTProject\MConsole\debug\Mconsole.dll" "E:\M_Server\Solution\x64\Debug"
//	WaitForInputIdle(GetCurrentProcess(), INFINITE);
//	auto length = wcslen(lpCmdLine);
	WCHAR* curry_Pos = lpCmdLine;
	//WCHAR** agrv = (WCHAR**)malloc(sizeof(WCHAR*)*512);
	WCHAR* agrv[512];
	memset(agrv, 0, sizeof(agrv));
	WCHAR** startPos = agrv;
	INT32 argC = 0;
	for (WCHAR* tempChar = lpCmdLine; TRUE; ++tempChar)
	{
		if (*tempChar == L' ' || *tempChar == L'\0')
		{
			if (tempChar == curry_Pos && *curry_Pos == L' ')
			{
				++curry_Pos;
				continue;
			}
			size_t char_lenth	= tempChar - curry_Pos;
			/*agrv = (WCHAR**)malloc(sizeof(WCHAR*));*/
			
			*startPos			= (WCHAR*)malloc(sizeof(WCHAR)*char_lenth + sizeof(WCHAR));
			memset(*startPos, 0,			sizeof(WCHAR)*char_lenth);
			memcpy(*startPos, curry_Pos,	sizeof(WCHAR)*char_lenth);

			(*startPos)[char_lenth] = L'\0';
			curry_Pos += char_lenth + 1;
			
			++startPos;
			++argC;
			if (*tempChar == L'\0')
			{
				break;
			}
		}
	}
	
	//LPTSTR** tChar_array = new LPTSTR*[]
	// 设置进程优先级
	::SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);

	// 禁止屏幕保护及电源管理
	::SystemParametersInfo(SPI_SETLOWPOWERTIMEOUT, 0, NULL, 0);
	::SystemParametersInfo(SPI_SETPOWEROFFTIMEOUT, 0, NULL, 0);
	::SystemParametersInfo(SPI_SETSCREENSAVETIMEOUT, 0, NULL, 0);
	
	// 不允许重复启动
	//HANDLE hMutex = ::CreateMutex(NULL, FALSE, _T("M_Server_TEST"));
	//if (GetLastError() == ERROR_ALREADY_EXISTS)
	//{
	//	CloseHandle(hMutex);

	//	// 该程序已经存在或者与此相同互斥的程序已经启动
	//	MessageBox(NULL, _T("M_Server_TEST already Running"), _T("Warning"), MB_OK);
	//	return 0;
	//}

	//初始化OLE/COM库环境
	::CoInitialize(NULL);

	// 初始化网络
	//NetUtil::InitNetwork();

	// 初始化ECore
	ECore::InitECore();
	//ESvrShare::InitESvrShare();
	
//	run_QTUI(0, NULL);
	HMODULE hInstance1 = GetModuleHandle(L"ECore.dll");
	HMODULE hInstance2 = GetModuleHandle(L"MConsole.dll");
	HMODULE hInstance3 = GetModuleHandle(L"M_ServerTest.exe");
	 
	HINSTANCE hInstance11 = GetModuleHandle(L"ECore.dll");
	HINSTANCE hInstance22 = GetModuleHandle(L"MConsole.dll");
	HINSTANCE hInstance33 = GetModuleHandle(L"M_ServerTest.exe");
	HINSTANCE hInstance44 = GetModuleHandle(_T("kernel32.dll"));
//	Trun_QTUI(UINT64(hInstance44), ECore::SS::ToW((UINT64)hInstance3));
	Trun_QTUI2(argC,(CONST WCHAR**) agrv);
	// 销毁ECore
	ECore::DestroyECore();

	return 0;
}

unsigned int __stdcall ThreadUpdate(void* p)
{
	HMODULE hDll = LoadLibrary(L"E:\\M_Server\\QTProject\\MConsole\\debug\\MConsole.dll");
	if (!hDll)
	{
		//printf("哭了\n");
	}
	else
	{
		typedef int(*Fun)(int argc, char *argv[]);//此处是定义一个跟导出函数类型形同的函数指
		//	addFunPro AddFun = (addFunPro)GetProcAddress(hInst, MAKEINTRESOURCE(1));//获得导出函数的地址  
		Fun AddFun = (Fun)GetProcAddress(hDll, "run_QTUI");//获得导出函数的地址  
		if (!AddFun)
		{
			//printf("获取函数地址失败！%d\n", sizeof(int*));
		}
		else
		{
			char** ared = new char*[5];
			int tempRet = AddFun(0, ared);
		}
	}
	return 0;
}


