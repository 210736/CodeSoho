//--------------------------------------------------------------------------------------------------
// File: safe_queue.h
// Desc: ������󣬲��ṩ�¼�֪ͨ���� -- �������첽����
// Time: 2009-12-30
// Author: lyp
//---------------------------------------------------------------------------------------------------
#pragma once

namespace ECore {
//-----------------------------------------------------------------------------
// Queue: �ṩ�̰߳�ȫ���Ƚ��ȳ�ָ����й���ֻ�ܴ��ָ��
//-----------------------------------------------------------------------------
template<typename POINTER>
class SafeTinyQueue
{
public:
	//-------------------------------------------------------------------------
	// ��ӵ�����β
	//-------------------------------------------------------------------------
	INLINE BOOL Add(POINTER pItem)
	{
		m_Lock.Lock();
		if( 0 == m_lItemNum )
			m_pFirst = pItem;
		else
			m_pLast->pNext = pItem;
		m_pLast = pItem;
		++m_lItemNum;
		m_Lock.Unlock();
		return TRUE;
	}

	//-------------------------------------------------------------------------
	// �Ӷ���ͷȡitem������踺���ͷ�
	//-------------------------------------------------------------------------
	INLINE POINTER Get()
	{
		if( m_lItemNum <= 0 )
			return NULL;

		POINTER pItem;

		m_Lock.Lock();
		if( m_lItemNum <= 0 )
		{
			m_Lock.Unlock();
			return NULL;
		}

		pItem = m_pFirst;	// ȡ��ITEM_POINTER
		m_pFirst = m_pFirst->pNext;
		--m_lItemNum;
		m_Lock.Unlock();
		return pItem;
	}

	//-------------------------------------------------------------------------
	// ��ӵ�������
	//-------------------------------------------------------------------------
	INLINE BOOL AddFront(POINTER pItem)
	{
		m_Lock.Lock();
		if( 0 == m_lItemNum )
			m_pLast = pItem;
		else
			pItem->pNext = m_pFirst;

		m_pFirst = pItem;
		++m_lItemNum;

		m_Lock.Unlock();
		return TRUE;
	}

	//--------------------------------------------------------------------------
	// �õ������е�items��Ŀ
	//--------------------------------------------------------------------------
	INLINE LONG GetNum()	{ return m_lItemNum; }

	INLINE SafeTinyQueue(): m_lItemNum(0),m_pFirst(0), m_pLast(0) {}

private:
	Mutex			m_Lock;
	LONG VOLATILE	m_lItemNum;			// ���д�С
	POINTER			m_pFirst;			// ͷ��
	POINTER			m_pLast;			// β��
};

} // namespace ECore
