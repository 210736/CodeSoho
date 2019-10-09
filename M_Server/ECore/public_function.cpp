//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2004/3/14
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "public_function.h"

// ʹ����������
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
//! �����ʼ��
//-----------------------------------------------------------------------------
ECORE_API BOOL InitECore()
{
	if( NULL != g_pObjMgr )	// ���治��Ҫ�ظ���ʼ��
		return TRUE;

#ifndef MEM_TRACE
	_CrtSetDebugFillThreshold(0);
#endif

	SetErrorMode(SEM_NOGPFAULTERRORBOX);	// ���ⵯ��ϵͳ�����
	_set_new_handler(NewHandle);			// new ����
	_set_new_mode(1);						// malloc ����

	g_pObjMgr = new ObjMgr;
	if( NULL == g_pObjMgr )
		return FALSE;

	// ע������ӿ�, ��Ϊ����������ʱ���ᰴ��ע��ķ���˳��������
	// ɾ����ע�������, ����������ѻ����Ľӿڷ���ǰ��ע��
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

	// ����ȫ��cache
	g_pMemCache = TObj<MemCache<AtomMutex>>();
	g_pMemCache->SetMaxSize(64*1024*1024);

	// ���浱ǰ·��
	CHAR szPathA[MAX_PATH];
	WCHAR szPath[MAX_PATH];
	GetCurrentDirectoryW(MAX_PATH, szPath);
	g_strOldCurrentPath = szPath;
	GetCurrentDirectoryA(MAX_PATH, szPathA);
	g_strOldCurrentPathA = szPathA;

	// ���ó���Ĭ��·��
	GetModuleFileNameW(NULL, szPath, MAX_PATH);
	WCHAR *pResult = wcsrchr(szPath, L'\\');	// ��ȥ�ļ�����ʣ��·��
	if(pResult)	
		*pResult = 0;
	
	::SetCurrentDirectoryW(szPath);
	g_strGamePath = szPath;

	// �����������
	TObj<IniLoader>()->Load(NULL, L"ecore.ini");
	g_strGamePath += TObj<IniLoader>()->Get(L"base_path", L"path", L"");
	g_strLocalPath = TObj<IniLoader>()->Get(L"local_path", L"path", L"");
	g_strLocalPathA = SS::ToA(g_strLocalPath.c_str());
	LPCWSTR szDebug = TObj<IniLoader>()->Get(L"debug", L"gb_debug", L"0");
	if( szDebug[0] == L'1' )
		g_bDebug = TRUE;

	// ��ȡ΢�˱�־
	LPCWSTR szSmall = TObj<IniLoader>()->Get(L"small", L"client", L"0");
	if( szSmall[0] == L'1' )
		g_bSmallClient = TRUE;

	// �Ƿ�����ļ���־
	LPCWSTR szLog = TObj<IniLoader>()->Get(L"filelog", L"client", L"0");
	if( szLog[0] == L'1' )
		g_bFileLog = TRUE;

	// ��������·��
	LPCWSTR szFontFile1 = TObj<IniLoader>()->Get(L"font1", L"font", L"");
	StringUtil::StringToToken(g_listFont1, SS::ToA(szFontFile1), _T(';'));

	LPCWSTR szFontFile2 = TObj<IniLoader>()->Get(L"font2", L"font", L"");
	StringUtil::StringToToken(g_listFont2, SS::ToA(szFontFile2), _T(';'));


	WCHAR szGamePath[MAX_PATH];
	PathCanonicalizeW(szGamePath,g_strGamePath.c_str());
	g_strGamePath = szGamePath;
	g_strGamePathA = SS::ToA(szGamePath);

	// ��·�����õ�����·��
	SetDefaultDir();	
	return TRUE;
}


//-----------------------------------------------------------------------------
//! ��������
//-----------------------------------------------------------------------------
ECORE_API VOID DestroyECore()
{	
	SetErrorMode(0);	// ��ԭϵͳ����Ի���
	_set_new_handler(0);

	g_pMemCache->SetTerminate();
	SAFE_DEL(g_pObjMgr);

	
}


//-----------------------------------------------------------------------------
// ����Ĭ��·��
//-----------------------------------------------------------------------------
ECORE_API VOID SetDefaultDir()
{
	::SetCurrentDirectoryW(g_strGamePath.c_str());
}


//-----------------------------------------------------------------------------
// �õ���ָ�������ָ��
//-----------------------------------------------------------------------------
ECORE_API LPVOID GetObj(LPCSTR szName) 
{ 
	// �Ƚ�ָ������תΪLPVOID,������ؼ̳�ʱ���������к���ECX�Ĵ���ƫ��
	return g_pObjMgr->Get(szName); 
}


//-----------------------------------------------------------------------------
// ����ע����Ķ��󴴽�����
//-----------------------------------------------------------------------------
ECORE_API LPCSTR CreateObj(LPCSTR szDestObjName, LPCSTR szSrcObjName ) 
{ 
	ASSERT( g_pObjMgr );	// ����û�г�ʼ��

	// ���ݽӿڿ�¡����,��¡�ɹ��ʹ��ؿ�¡���Ķ��������
	if( TRUE == g_pObjMgr->CreateObj(szDestObjName, szSrcObjName) )
		return szDestObjName;
	else
		return NULL;
}


//-----------------------------------------------------------------------------
//!\return �����Ƿ�ɹ�
//-----------------------------------------------------------------------------
ECORE_API BOOL KillObj(LPCSTR szObjName)
{
	ASSERT( g_pObjMgr );	// ����û�г�ʼ��
	return g_pObjMgr->KillObj(szObjName);
}


//-----------------------------------------------------------------------------
// �ṹ���쳣ת��
//-----------------------------------------------------------------------------
ECORE_API VOID SEH_Translate(UINT unCode, EXCEPTION_POINTERS* pExp)
{
	throw Dump(unCode, pExp);
}


//-----------------------------------------------------------------------------
// �ṹ���쳣ת��
//-----------------------------------------------------------------------------
ECORE_API VOID SEH_TranslateLog(UINT unCode, EXCEPTION_POINTERS* pExp)
{
#ifndef X64
	throw CrashLog(unCode, pExp);
#endif
}



//-----------------------------------------------------------------------------
// ���::SetUnhandledExceptionFilterʹ�õ��쳣������
// ���޷�ʹ��try...catch��ʱ����ʹ��
//-----------------------------------------------------------------------------
ECORE_API LONG WINAPI SEH_Filter(EXCEPTION_POINTERS* pExp)
{
	Dump(0, pExp);
	return EXCEPTION_CONTINUE_SEARCH;
}


//-----------------------------------------------------------------------------
// ���::SetUnhandledExceptionFilterʹ�õ��쳣������
// ���޷�ʹ��try...catch��ʱ����ʹ��
//-----------------------------------------------------------------------------
ECORE_API LONG WINAPI SEH_FilterLog(EXCEPTION_POINTERS* pExp)
{
#ifndef X64
	CrashLog(pExp->ExceptionRecord->ExceptionCode, pExp);
#endif
	return EXCEPTION_CONTINUE_SEARCH;
}




} // namespace ECore{

