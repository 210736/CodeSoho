//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2004/3/14
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "public_function.h"

// 使用其它部件
#ifndef MEM_TRACE
#include <crtdbg.h>
#endif

#include "obj_mgr.h"
#include "new_protect.h"

#include "Timer.h"
#include "log.h"
#include "error.h"
#include "dump.h"
#include "crash_log.h"
#include "mem_cache.h"
#include "mem_trace.h"
#include "disk_io.h"
#include "filter.h"
#include "vfs.h"
#include "lan_server.h"
#include "lan_client.h"
#include "wan_server.h"
#include "wan_client.h"
#include "ping.h"
#include "broadcast.h"
#include "xml_loader.h"
#include "ini_loader.h"
#include "ssv_loader.h"
#include "thread_mgr.h"
#include "task_scheduler.h"
#include "super_string.h"
#include "io_redir.h"
#include "string_table.h"
#include "fast_code.h"
#include "string_util.h"

#undef new
#undef delete
#undef malloc
#undef calloc
#undef realloc
#undef free


namespace ECore{
//-----------------------------------------------------------------------------
//! 引擎初始化
//-----------------------------------------------------------------------------
ECORE_API BOOL InitECore()
{
	if( NULL != g_pObjMgr )	// 引擎不需要重复初始化
		return TRUE;

#ifndef MEM_TRACE
	_CrtSetDebugFillThreshold(0);
#endif

	SetErrorMode(SEM_NOGPFAULTERRORBOX);	// 避免弹出系统错误框
	_set_new_handler(NewHandle);			// new 保护
	_set_new_mode(1);						// malloc 保护

	g_pObjMgr = new ObjMgr;
	if( NULL == g_pObjMgr )
		return FALSE;

	// 注册各个接口, 因为在引擎析构时，会按照注册的反向顺序来优先
	// 删除后注册的类型, 所以这里请把基本的接口放在前面注册
	Register<MemCache<AtomMutex>>("MemCache<AtomMutex>");
	Register<Timer>("Timer");
	Register<XmlLoader>("XmlLoader");
	Register<SSVLoader>("SSVLoader");
	Register<ThreadMgr>("ThreadMgr");
	Register<TaskScheduler>("TaskScheduler");
	Register<Log>("Log");
	Register<Error>("Error");
	Register<DiskIO>("DiskIO");
	Register<VFS>("VFS");
	Register<Filter>("Filter");
	Register<Lan::Server>("Lan::Server");
	Register<Lan::Client>("Lan::Client");
	Register<Wan::Server>("Wan::Server");
	Register<Wan::Client>("Wan::Client");
	Register<Broadcast>("Broadcast");
	Register<Ping>("Ping");
	Register<IORedir>("IORedir");
	Register<IniLoader>("IniLoader");
	Register<StrTab>("StrTab");

	// 开启全局cache
	g_pMemCache = TObj<MemCache<AtomMutex>>();
	g_pMemCache->SetMaxSize(64*1024*1024);

	// 保存当前路径
	CHAR szPathA[MAX_PATH];
	WCHAR szPath[MAX_PATH];
	GetCurrentDirectoryW(MAX_PATH, szPath);
	g_strOldCurrentPath = szPath;
	GetCurrentDirectoryA(MAX_PATH, szPathA);
	g_strOldCurrentPathA = szPathA;

	// 设置程序默认路径
	GetModuleFileNameW(NULL, szPath, MAX_PATH);
	WCHAR *pResult = wcsrchr(szPath, L'\\');	// 截去文件名，剩下路径
	if(pResult)	
		*pResult = 0;
	
	::SetCurrentDirectoryW(szPath);
	g_strGamePath = szPath;

	// 读入基础配置
	TObj<IniLoader>()->Load(NULL, L"ecore.ini");
	g_strGamePath += TObj<IniLoader>()->Get(L"base_path", L"path", L"");
	g_strLocalPath = TObj<IniLoader>()->Get(L"local_path", L"path", L"");
	g_strLocalPathA = SS::ToA(g_strLocalPath.c_str());
	LPCWSTR szDebug = TObj<IniLoader>()->Get(L"debug", L"gb_debug", L"0");
	if( szDebug[0] == L'1' )
		g_bDebug = TRUE;

	// 读取微端标志
	LPCWSTR szSmall = TObj<IniLoader>()->Get(L"small", L"client", L"0");
	if( szSmall[0] == L'1' )
		g_bSmallClient = TRUE;

	// 是否输出文件日志
	LPCWSTR szLog = TObj<IniLoader>()->Get(L"filelog", L"client", L"0");
	if( szLog[0] == L'1' )
		g_bFileLog = TRUE;

	// 读出字体路径
	LPCWSTR szFontFile1 = TObj<IniLoader>()->Get(L"font1", L"font", L"");
	StringUtil::StringToToken(g_listFont1, SS::ToA(szFontFile1), _T(';'));

	LPCWSTR szFontFile2 = TObj<IniLoader>()->Get(L"font2", L"font", L"");
	StringUtil::StringToToken(g_listFont2, SS::ToA(szFontFile2), _T(';'));


	WCHAR szGamePath[MAX_PATH];
	PathCanonicalizeW(szGamePath,g_strGamePath.c_str());
	g_strGamePath = szGamePath;
	g_strGamePathA = SS::ToA(szGamePath);

	// 把路径设置到基础路径
	SetDefaultDir();	
	return TRUE;
}


//-----------------------------------------------------------------------------
//! 销毁引擎
//-----------------------------------------------------------------------------
ECORE_API VOID DestroyECore()
{	
	SetErrorMode(0);	// 还原系统错误对话框
	_set_new_handler(0);

	g_pMemCache->SetTerminate();
	SAFE_DEL(g_pObjMgr);

	
}


//-----------------------------------------------------------------------------
// 设置默认路径
//-----------------------------------------------------------------------------
ECORE_API VOID SetDefaultDir()
{
	::SetCurrentDirectoryW(g_strGamePath.c_str());
}


//-----------------------------------------------------------------------------
// 得到向指定物件的指针
//-----------------------------------------------------------------------------
ECORE_API LPVOID GetObj(LPCSTR szName) 
{ 
	// 先将指针类型转为LPVOID,避免多重继承时编译器做有害的ECX寄存器偏移
	return g_pObjMgr->Get(szName); 
}


//-----------------------------------------------------------------------------
// 根据注册过的对象创建对象
//-----------------------------------------------------------------------------
ECORE_API LPCSTR CreateObj(LPCSTR szDestObjName, LPCSTR szSrcObjName ) 
{ 
	ASSERT( g_pObjMgr );	// 引擎没有初始化

	// 根据接口克隆对象,克隆成功就传回克隆出的对象的名字
	if( TRUE == g_pObjMgr->CreateObj(szDestObjName, szSrcObjName) )
		return szDestObjName;
	else
		return NULL;
}


//-----------------------------------------------------------------------------
//!\return 析构是否成功
//-----------------------------------------------------------------------------
ECORE_API BOOL KillObj(LPCSTR szObjName)
{
	ASSERT( g_pObjMgr );	// 引擎没有初始化
	return g_pObjMgr->KillObj(szObjName);
}


//-----------------------------------------------------------------------------
// 结构化异常转换
//-----------------------------------------------------------------------------
ECORE_API VOID SEH_Translate(UINT unCode, EXCEPTION_POINTERS* pExp)
{
	throw Dump(unCode, pExp);
}


//-----------------------------------------------------------------------------
// 结构化异常转换
//-----------------------------------------------------------------------------
ECORE_API VOID SEH_TranslateLog(UINT unCode, EXCEPTION_POINTERS* pExp)
{
#ifndef X64
	throw CrashLog(unCode, pExp);
#endif
}



//-----------------------------------------------------------------------------
// 配合::SetUnhandledExceptionFilter使用的异常处理函数
// 当无法使用try...catch块时可以使用
//-----------------------------------------------------------------------------
ECORE_API LONG WINAPI SEH_Filter(EXCEPTION_POINTERS* pExp)
{
	Dump(0, pExp);
	return EXCEPTION_CONTINUE_SEARCH;
}


//-----------------------------------------------------------------------------
// 配合::SetUnhandledExceptionFilter使用的异常处理函数
// 当无法使用try...catch块时可以使用
//-----------------------------------------------------------------------------
ECORE_API LONG WINAPI SEH_FilterLog(EXCEPTION_POINTERS* pExp)
{
#ifndef X64
	CrashLog(pExp->ExceptionRecord->ExceptionCode, pExp);
#endif
	return EXCEPTION_CONTINUE_SEARCH;
}




} // namespace ECore{

