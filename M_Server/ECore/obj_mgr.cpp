//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2003/12/11
// Last: 2004/03/15
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "obj_mgr.h"

// 这里不能使用内存池
#undef new
#undef delete
#undef malloc
#undef calloc
#undef realloc
#undef free

namespace ECore{
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
ObjMgr::~ObjMgr()
{
	std::map<DWORD, tagObj>::iterator it;
	m_Lock.Lock();

	// 按照注册的相反顺序析构对象
	while( !m_listObj.empty() )
	{
		std::string str = m_listObj.back();
		KillObj(str.c_str());
	}
	
	for(it = m_mapObj.begin(); it != m_mapObj.end(); ++it)	// 析构剩余对象
	{
		if( !VALID(it->second.pObj) )
			continue;

		m_AF.Destroy(it->second.pObj, it->second.dwClassID);
		it->second.pObj = NULL;
	}

	m_mapObj.clear();
	m_Lock.Unlock();
}


//-----------------------------------------------------------------------------
// 如果对象还没有创建，则创建
//-----------------------------------------------------------------------------
LPVOID ObjMgr::Get(LPCSTR szName)
{
	DWORD dwID = Crc32(szName);
	LPVOID pObj = NULL;
	std::map<DWORD, tagObj>::iterator it;

	m_Lock.Lock();

	it = m_mapObj.find(dwID);	// 从对象中找
	if( it == m_mapObj.end() )
	{
		m_Lock.Unlock();
		return NULL;
	}

	if( it->second.pObj )
		pObj = it->second.pObj;
	else
	{
		// 对象还没有创建，准备创建
		it->second.pObj = m_AF.Create(dwID);
		pObj = it->second.pObj;
	}

	m_Lock.Unlock();
	return pObj;
}


//-----------------------------------------------------------------------------
// 根据类型名获得对象
//-----------------------------------------------------------------------------
LPVOID ObjMgr::GetByType(LPCSTR szName)
{
	LPVOID pObj = NULL;
	DWORD dwTypeID = Crc32(szName);
	std::map<DWORD, DWORD>::iterator it0;
	std::map<DWORD, tagObj>::iterator it;

	m_Lock.Lock();

	it0 = m_mapTypeObj.find(dwTypeID);	// 从对象中找
	if( it0 == m_mapTypeObj.end() )
	{
		m_Lock.Unlock();
		return NULL;
	}
	
	DWORD dwID = it0->second;
	
	it = m_mapObj.find(dwID);	// 从对象中找
	if( it == m_mapObj.end() )
	{
		m_Lock.Unlock();
		return NULL;
	}

	if( it->second.pObj )
		pObj = it->second.pObj;
	else
	{
		// 对象还没有创建，准备创建
		it->second.pObj = m_AF.Create(dwID);
		pObj = it->second.pObj;
	}

	m_Lock.Unlock();
	return pObj;
}


//-----------------------------------------------------------------------------
// 创建对象
//-----------------------------------------------------------------------------
BOOL ObjMgr::CreateObj(LPCSTR szDestObjName, LPCSTR szSrcObjName)
{
	tagObj objDesc;
	objDesc.dwClassID = Crc32(szSrcObjName);
	objDesc.strObjName.assign(szDestObjName);
	DWORD dwObjID = Crc32(szDestObjName);

	m_Lock.Lock();

	if( m_mapObj.find(dwObjID) != m_mapObj.end() )	// 已经存在
	{
		m_Lock.Unlock();
		return FALSE;
	}

	objDesc.pObj = m_AF.Create(objDesc.dwClassID);	// 创建对象
	if( !VALID(objDesc.pObj) )
	{
		m_Lock.Unlock();
		return FALSE;
	}
	
	m_mapObj.insert(std::pair<DWORD, tagObj>(dwObjID, objDesc));
	m_listObj.push_back(szDestObjName);

	m_Lock.Unlock();
	return TRUE;
}


//-----------------------------------------------------------------------------
// 析构对象
//-----------------------------------------------------------------------------
BOOL ObjMgr::KillObj(LPCSTR szObjName)
{
	ASSERT(szObjName);
	DWORD dwID = Crc32(szObjName);
	std::map<DWORD, tagObj>::iterator it;
	std::list<std::string>::iterator itList;

	m_Lock.Lock();
	it = m_mapObj.find(dwID);	// 从对象中找
	if( it == m_mapObj.end() )
	{
		m_Lock.Unlock();
		return FALSE;	// 最终找不到
	}

	LPVOID pObj = it->second.pObj;
	DWORD dwClassID = it->second.dwClassID;
	
	// 先从物件列表中删除，避免Destory过程中又从m_mapObj查找到自己
	m_mapObj.erase(it);

	// 同时从lis中删除
	for(itList = m_listObj.begin(); itList!=m_listObj.end(); itList++)
	{
		if((*itList) == szObjName)
		{
			m_listObj.erase(itList);
			break;
		}
	}
	
	if( pObj )
		m_AF.Destroy(pObj, dwClassID);
		
	m_Lock.Unlock();
	return TRUE;
}



} // namespace ECore{