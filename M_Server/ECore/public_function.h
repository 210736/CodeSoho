//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2004/3/14
//-----------------------------------------------------------------------------
#pragma once

namespace ECore
{
//-----------------------------------------------------------------------------
// 基本操作
//-----------------------------------------------------------------------------
ECORE_API BOOL		InitECore();
ECORE_API VOID		DestroyECore();
ECORE_API VOID		SetDefaultDir();		// 设置默认路径

//-----------------------------------------------------------------------------
// 对象操作
//-----------------------------------------------------------------------------
ECORE_API LPVOID	GetObj(LPCSTR szName);
ECORE_API LPCSTR	CreateObj(LPCSTR szDestObjName, LPCSTR szSrcObjName);
ECORE_API BOOL		KillObj(LPCSTR szObjName);

//-----------------------------------------------------------------------------
// 异常处理
//-----------------------------------------------------------------------------
ECORE_API VOID		SEH_Translate(UINT unCode, EXCEPTION_POINTERS* pExp);
ECORE_API VOID		SEH_TranslateLog(UINT unCode, EXCEPTION_POINTERS* pExp);

ECORE_API VOID		InitDump(LPCTSTR szIP, LPCTSTR szUser, LPCTSTR szPassword, LPCTSTR szVersion=NULL);
ECORE_API LONG WINAPI SEH_Filter(EXCEPTION_POINTERS* pExp);	// 当无法使用try...catch块时可以使用
ECORE_API LONG WINAPI SEH_FilterLog(EXCEPTION_POINTERS* pExp);	// 当无法使用try...catch块时可以使用

//-----------------------------------------------------------------------------
// 注册对象
//-----------------------------------------------------------------------------
template<typename class_type>
VOID Register(LPCSTR szName)
{ ECore::g_pObjMgr->Register<class_type>(szName); }


} // namespace ECore {



