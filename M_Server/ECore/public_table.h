//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2004/3/14
//-----------------------------------------------------------------------------
#pragma once


namespace ECore{
//-----------------------------------------------------------------------------
// 引擎所有常量表定义在此 
//-----------------------------------------------------------------------------
extern ECORE_API std::wstring g_strGamePath;
extern ECORE_API std::string g_strGamePathA;

extern ECORE_API std::wstring g_strLocalPath;
extern ECORE_API std::string g_strLocalPathA;

extern ECORE_API std::wstring g_strOldCurrentPath;
extern ECORE_API std::string g_strOldCurrentPathA;

extern ECORE_API std::string g_strUIPath;

extern ECORE_API BOOL g_bDebug;
extern ECORE_API BOOL g_bSmallClient;//小客户端标示 by 王炜
extern ECORE_API BOOL g_bFileLog;// 是否输出文件日志 by 王炜

extern ECORE_API MemCache<AtomMutex>* g_pMemCache;
extern ECORE_API ObjMgr* g_pObjMgr;
extern ECORE_API HWND g_hWnd;
extern ECORE_API CONST LPCSTR DUMP_FILE_NAME;
extern ECORE_API CONST LPCSTR CRASH_FILE_NAME;
extern ECORE_API const unsigned long g_CrcTable[256];
extern ECORE_API const unsigned int g_SqrtTable[0x10000];
extern ECORE_API DWORD g_dwSVN;
extern ECORE_API list<string>	g_listFont1;
extern ECORE_API list<string>	g_listFont2;
}	// namespace ECore{
