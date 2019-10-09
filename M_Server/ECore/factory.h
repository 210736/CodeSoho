//-----------------------------------------------------------------------------
// author Lyp
// date 2004/3/11
// last 2004/3/13
//-----------------------------------------------------------------------------
#pragma once
#include "binder.h"

namespace ECore {
//-----------------------------------------------------------------------------
// ע��: Register<YOUR_CLASS>( DWORD dwID )
// ����: Create( DWORD dwID )
//-----------------------------------------------------------------------------
template<typename abstract_class>	// ����ΪVOID������ָ���κζ���
class TFactory
{
public:
	//-------------------------------------------------------------------------
	// ע�������
	//-------------------------------------------------------------------------
	template<typename concrete_class>
	BOOL Register(DWORD dwID)
	{ 
		TBinder<concrete_class>* pBinder = new TBinder<concrete_class>;
		if( !pBinder ) return FALSE;

		ASSERT( m_map.Peek(dwID) == (BinderAbstract*)INVALID );	// �����ظ�ע��
		m_map.Add(dwID, pBinder);
		return TRUE;
	}

	//--------------------------------------------------------------------------
	// ��ע��
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
	// ��������
	//---------------------------------------------------------------------------
	abstract_class* Create(DWORD dwID)
	{
		BinderAbstract* pAbstract = m_map.Peek(dwID);
		if( !VALID(pAbstract) )
			return NULL;

		return (abstract_class*)pAbstract->Create();
	}

	//----------------------------------------------------------------------------
	// ɾ������
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
	TMap<DWORD, BinderAbstract*>	m_map;			// ���Ͱ�����
};


} // namespace ECore {