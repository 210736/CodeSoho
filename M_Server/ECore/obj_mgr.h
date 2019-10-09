//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2003/12/11
//-----------------------------------------------------------------------------
#pragma once
#include "factory.h"

namespace ECore{
//-----------------------------------------------------------------------------
// 可以管理各种对象
//-----------------------------------------------------------------------------
class ECORE_API ObjMgr
{
public:
	//-------------------------------------------------------------------------
	// 根据字符串获得对象
	//-------------------------------------------------------------------------
	LPVOID	Get(LPCSTR szName);
	LPVOID	GetByType(LPCSTR szName);

	//-------------------------------------------------------------------------
	// 通过字符串创建和删除对象
	//-------------------------------------------------------------------------
	BOOL	CreateObj(LPCSTR szDestObjName, LPCSTR szClassName);
	BOOL	KillObj(LPCSTR szObjName);

	//-------------------------------------------------------------------------
	// 注册对象类
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

	TFactory<VOID>			m_AF;			// 类型工厂
	Mutex					m_Lock;			// 线程锁
};


//-----------------------------------------------------------------------------
// 注册
//-----------------------------------------------------------------------------
template<typename class_type>
VOID ObjMgr::Register(LPCSTR szName)
{
	DWORD dwID		=	Crc32(szName); 
	DWORD dwTypeID	=	Crc32(typeid(class_type*).raw_name());

	ASSERT(m_mapObj.find(dwID) == m_mapObj.end());

	m_Lock.Lock();

	m_AF.Register<class_type>(dwID);	// 向类厂注册

	tagObj obj;
	obj.pObj = NULL;	// 并不实际创建对象
	obj.dwClassID = dwID;
	obj.strObjName.assign(szName);
	m_mapObj.insert(std::pair<DWORD, tagObj>(dwID, obj));
	m_listObj.push_back(szName);

	// 同时注册typeid
	m_mapTypeObj.insert(std::pair<DWORD, DWORD>(dwTypeID, dwID));

	m_Lock.Unlock();
}


}	// namespace ECore{
