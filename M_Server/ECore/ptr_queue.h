//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2003-11-30
// Last: 2009-06-11
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {
//-----------------------------------------------------------------------------
// ָ�����
//-----------------------------------------------------------------------------
template<typename POINTER>
class PtrQueue
{
public:
	//-------------------------------------------------------------------------
	// ��ӵ�����β
	//-------------------------------------------------------------------------
	INLINE VOID Add(POINTER pItem)
	{
		if( 0 == m_lItemNum )
			m_pFirst = pItem;
		else
			m_pLast->pNext = pItem;
		m_pLast = pItem;
		++m_lItemNum;
	}

	//-------------------------------------------------------------------------
	// �Ӷ���ͷȡitem������踺���ͷ�
	//-------------------------------------------------------------------------
	INLINE POINTER Get()
	{
		if( m_lItemNum <= 0 )
			return NULL;

		if( m_lItemNum <= 0 )
		{
			return NULL;
		}

		POINTER pItem = m_pFirst;	// ȡ��ITEM_POINTER
		m_pFirst = m_pFirst->pNext;
		--m_lItemNum;
		return pItem;
	}

	//-------------------------------------------------------------------------
	// ��ӵ�������
	//-------------------------------------------------------------------------
	INLINE BOOL AddFront(POINTER pItem)
	{
		if( 0 == m_lItemNum )
			m_pLast = pItem;
		else
			pItem->pNext = m_pFirst;

		m_pFirst = pItem;
		++m_lItemNum;

		return TRUE;
	}
	
	//--------------------------------------------------------------------------
	// �õ������е�items��Ŀ
	//--------------------------------------------------------------------------
	INLINE LONG GetNum()	{ return m_lItemNum; }
	
	INLINE PtrQueue(): m_lItemNum(0),m_pFirst(0), m_pLast(0) {}
	
private:
	LONG VOLATILE	m_lItemNum;			// ���д�С
	POINTER			m_pFirst;			// ͷ��
	POINTER			m_pLast;			// β��
};

} // namespace ECore{

