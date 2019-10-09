//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2003/12/11
// Last: 2004/03/15
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "obj_mgr.h"

// ���ﲻ��ʹ���ڴ��
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

	// ����ע����෴˳����������
	while( !m_listObj.empty() )
	{
		std::string str = m_listObj.back();
		KillObj(str.c_str());
	}
	
	for(it = m_mapObj.begin(); it != m_mapObj.end(); ++it)	// ����ʣ�����
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
// �������û�д������򴴽�
//-----------------------------------------------------------------------------
LPVOID ObjMgr::Get(LPCSTR szName)
{
	DWORD dwID = Crc32(szName);
	LPVOID pObj = NULL;
	std::map<DWORD, tagObj>::iterator it;

	m_Lock.Lock();

	it = m_mapObj.find(dwID);	// �Ӷ�������
	if( it == m_mapObj.end() )
	{
		m_Lock.Unlock();
		return NULL;
	}

	if( it->second.pObj )
		pObj = it->second.pObj;
	else
	{
		// ����û�д�����׼������
		it->second.pObj = m_AF.Create(dwID);
		pObj = it->second.pObj;
	}

	m_Lock.Unlock();
	return pObj;
}


//-----------------------------------------------------------------------------
// ������������ö���
//-----------------------------------------------------------------------------
LPVOID ObjMgr::GetByType(LPCSTR szName)
{
	LPVOID pObj = NULL;
	DWORD dwTypeID = Crc32(szName);
	std::map<DWORD, DWORD>::iterator it0;
	std::map<DWORD, tagObj>::iterator it;

	m_Lock.Lock();

	it0 = m_mapTypeObj.find(dwTypeID);	// �Ӷ�������
	if( it0 == m_mapTypeObj.end() )
	{
		m_Lock.Unlock();
		return NULL;
	}
	
	DWORD dwID = it0->second;
	
	it = m_mapObj.find(dwID);	// �Ӷ�������
	if( it == m_mapObj.end() )
	{
		m_Lock.Unlock();
		return NULL;
	}

	if( it->second.pObj )
		pObj = it->second.pObj;
	else
	{
		// ����û�д�����׼������
		it->second.pObj = m_AF.Create(dwID);
		pObj = it->second.pObj;
	}

	m_Lock.Unlock();
	return pObj;
}


//-----------------------------------------------------------------------------
// ��������
//-----------------------------------------------------------------------------
BOOL ObjMgr::CreateObj(LPCSTR szDestObjName, LPCSTR szSrcObjName)
{
	tagObj objDesc;
	objDesc.dwClassID = Crc32(szSrcObjName);
	objDesc.strObjName.assign(szDestObjName);
	DWORD dwObjID = Crc32(szDestObjName);

	m_Lock.Lock();

	if( m_mapObj.find(dwObjID) != m_mapObj.end() )	// �Ѿ�����
	{
		m_Lock.Unlock();
		return FALSE;
	}

	objDesc.pObj = m_AF.Create(objDesc.dwClassID);	// ��������
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
// ��������
//-----------------------------------------------------------------------------
BOOL ObjMgr::KillObj(LPCSTR szObjName)
{
	ASSERT(szObjName);
	DWORD dwID = Crc32(szObjName);
	std::map<DWORD, tagObj>::iterator it;
	std::list<std::string>::iterator itList;

	m_Lock.Lock();
	it = m_mapObj.find(dwID);	// �Ӷ�������
	if( it == m_mapObj.end() )
	{
		m_Lock.Unlock();
		return FALSE;	// �����Ҳ���
	}

	LPVOID pObj = it->second.pObj;
	DWORD dwClassID = it->second.dwClassID;
	
	// �ȴ�����б���ɾ��������Destory�������ִ�m_mapObj���ҵ��Լ�
	m_mapObj.erase(it);

	// ͬʱ��lis��ɾ��
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