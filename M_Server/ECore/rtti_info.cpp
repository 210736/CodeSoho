#include "StdAfx.h"
#include "rtti_info.h"

namespace ECore{

RTTInfo::RTTInfo(LPCSTR szClassName,RTTInfo *pSuperClass,CreateObj *pFunc): m_szClassName(szClassName),	m_pSuperClass(pSuperClass),	m_pfnCreateObj(pFunc)
{
	m_dwClassID = Crc32(szClassName);
	RTTFactory::Inst()->AddRuntimeClass(this);
}

bool RTTInfo::IsKindOf(const RTTInfo *pInfo)
{
	if( !pInfo )
		return false;

	RTTInfo *pInfoIter = this;
	while( pInfoIter )
	{
		if( pInfoIter == pInfo )
			return true;
		else
			pInfoIter = pInfoIter->m_pSuperClass;
	}

	return false;
}

bool RTTInfo::IsKindOf(LPCSTR szClassName)
{
	if( !szClassName )
		return false;

	DWORD dwID = Crc32(szClassName);

	RTTInfo *pInfoIter=this;
	while( pInfoIter )
	{
		if( dwID == pInfoIter->m_dwClassID )
			return true;
		else
			pInfoIter = pInfoIter->m_pSuperClass;
	}
	return false;
}



RTTIObj* RTTFactory::CreateObj(DWORD dwClassID)
{
	CONST RTTInfo *pInfo = m_map.Peek(dwClassID);
	if( !VALID(pInfo) )
	{
		OutputDebugStringA("class id not found!\r\n");
		return NULL;
	}

	if( !pInfo->m_pfnCreateObj )
	{
		OutputDebugStringA(pInfo->m_szClassName);
		OutputDebugStringA(":can not create class instance!\r\n");
		return NULL;
	}
	return pInfo->m_pfnCreateObj();
}


void RTTFactory::AddRuntimeClass(const RTTInfo *pInfo)
{
	if( !pInfo )
		return;

	BOOL bResult = m_map.Add(pInfo->m_dwClassID, pInfo);
	ASSERT(bResult);

	m_vec.push_back(pInfo);
}


}
