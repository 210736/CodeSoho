//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2004/3/14
//-----------------------------------------------------------------------------
#pragma once

namespace ECore
{
//-----------------------------------------------------------------------------
// ��������
//-----------------------------------------------------------------------------
ECORE_API BOOL		InitECore();
ECORE_API VOID		DestroyECore();
ECORE_API VOID		SetDefaultDir();		// ����Ĭ��·��

//-----------------------------------------------------------------------------
// �������
//-----------------------------------------------------------------------------
ECORE_API LPVOID	GetObj(LPCSTR szName);
ECORE_API LPCSTR	CreateObj(LPCSTR szDestObjName, LPCSTR szSrcObjName);
ECORE_API BOOL		KillObj(LPCSTR szObjName);

//-----------------------------------------------------------------------------
// �쳣����
//-----------------------------------------------------------------------------
ECORE_API VOID		SEH_Translate(UINT unCode, EXCEPTION_POINTERS* pExp);
ECORE_API VOID		SEH_TranslateLog(UINT unCode, EXCEPTION_POINTERS* pExp);

ECORE_API VOID		InitDump(LPCTSTR szIP, LPCTSTR szUser, LPCTSTR szPassword, LPCTSTR szVersion=NULL);
ECORE_API LONG WINAPI SEH_Filter(EXCEPTION_POINTERS* pExp);	// ���޷�ʹ��try...catch��ʱ����ʹ��
ECORE_API LONG WINAPI SEH_FilterLog(EXCEPTION_POINTERS* pExp);	// ���޷�ʹ��try...catch��ʱ����ʹ��

//-----------------------------------------------------------------------------
// ע�����
//-----------------------------------------------------------------------------
template<typename class_type>
VOID Register(LPCSTR szName)
{ ECore::g_pObjMgr->Register<class_type>(szName); }


} // namespace ECore {



