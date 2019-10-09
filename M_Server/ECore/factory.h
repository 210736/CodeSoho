//-----------------------------------------------------------------------------
// author Lyp
// date 2004/3/11
// last 2004/3/13
//-----------------------------------------------------------------------------
#pragma once
#include "binder.h"

namespace ECore {
//-----------------------------------------------------------------------------
// 注册: Register<YOUR_CLASS>( DWORD dwID )
// 创建: Create( DWORD dwID )
//-----------------------------------------------------------------------------
template<typename abstract_class>	// 可以为VOID，用来指向任何对象
class TFactory
{
public:
	//-------------------------------------------------------------------------
	// 注册各种类
	//-------------------------------------------------------------------------
	template<typename concrete_class>
	BOOL Register(DWORD dwID)
	{ 
		TBinder<concrete_class>* pBinder = new TBinder<concrete_class>;
		if( !pBinder ) return FALSE;

		ASSERT( m_map.Peek(dwID) == (BinderAbstract*)INVALID );	// 不能重复注册
		m_map.Add(dwID, pBinder);
		return TRUE;
	}

	//--------------------------------------------------------------------------
	// 反注册
	//--------------------------------------------------------------------------
	template<typename concrete_class>
	BOOL Unregister(DWORD dwID)
	{
		BinderAbstract* pAbstract = m_map.Peek(dwID);
		if( !VALID(pAbstract) )
			return FALSE;

		delete((TBinder<concrete_class>*)(pAbstract));
		m_map.Erase(dwID);
		return TRUE;
	}


	//---------------------------------------------------------------------------
	// 创建对象
	//---------------------------------------------------------------------------
	abstract_class* Create(DWORD dwID)
	{
		BinderAbstract* pAbstract = m_map.Peek(dwID);
		if( !VALID(pAbstract) )
			return NULL;

		return (abstract_class*)pAbstract->Create();
	}

	//----------------------------------------------------------------------------
	// 删除对象
	//----------------------------------------------------------------------------
	VOID Destroy(LPVOID p, DWORD dwID)
	{
		BinderAbstract* pAbstract = m_map.Peek(dwID);
		if( VALID(pAbstract) )
			pAbstract->Destroy(p);
	}

	~TFactory() 
	{	
		BinderAbstract* p;
		m_map.ResetIterator();
		while(m_map.PeekNext(p))
			delete(p);
	}

private:
	TMap<DWORD, BinderAbstract*>	m_map;			// 类型绑定容器
};


} // namespace ECore {