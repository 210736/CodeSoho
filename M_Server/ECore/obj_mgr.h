//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2003/12/11
//-----------------------------------------------------------------------------
#pragma once
#include "factory.h"

namespace ECore{
//-----------------------------------------------------------------------------
// ���Թ�����ֶ���
//-----------------------------------------------------------------------------
class ECORE_API ObjMgr
{
public:
	//-------------------------------------------------------------------------
	// �����ַ�����ö���
	//-------------------------------------------------------------------------
	LPVOID	Get(LPCSTR szName);
	LPVOID	GetByType(LPCSTR szName);

	//-------------------------------------------------------------------------
	// ͨ���ַ���������ɾ������
	//-------------------------------------------------------------------------
	BOOL	CreateObj(LPCSTR szDestObjName, LPCSTR szClassName);
	BOOL	KillObj(LPCSTR szObjName);

	//-------------------------------------------------------------------------
	// ע�������
	//-------------------------------------------------------------------------
	template<typename class_type>
	VOID Register(LPCSTR szName);

	
	~ObjMgr();

private:
	struct tagObj
	{
		LPVOID			pObj;
		DWORD			dwClassID;
		std::string		strObjName;
	};

	std::map<DWORD, tagObj>		m_mapObj;
	std::map<DWORD, DWORD>		m_mapTypeObj;	// typename -> m_mapObj ID
	std::list<std::string>		m_listObj;

	TFactory<VOID>			m_AF;			// ���͹���
	Mutex					m_Lock;			// �߳���
};


//-----------------------------------------------------------------------------
// ע��
//-----------------------------------------------------------------------------
template<typename class_type>
VOID ObjMgr::Register(LPCSTR szName)
{
	DWORD dwID		=	Crc32(szName); 
	DWORD dwTypeID	=	Crc32(typeid(class_type*).raw_name());

	ASSERT(m_mapObj.find(dwID) == m_mapObj.end());

	m_Lock.Lock();

	m_AF.Register<class_type>(dwID);	// ���೧ע��

	tagObj obj;
	obj.pObj = NULL;	// ����ʵ�ʴ�������
	obj.dwClassID = dwID;
	obj.strObjName.assign(szName);
	m_mapObj.insert(std::pair<DWORD, tagObj>(dwID, obj));
	m_listObj.push_back(szName);

	// ͬʱע��typeid
	m_mapTypeObj.insert(std::pair<DWORD, DWORD>(dwTypeID, dwID));

	m_Lock.Unlock();
}


}	// namespace ECore{
