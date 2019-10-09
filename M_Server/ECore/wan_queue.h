//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2003-11-30
// Last: 2009-06-11
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {
namespace Wan{
//-----------------------------------------------------------------------------
// RingQueue: �ṩ�̰߳�ȫ�Ļ��ζ��У�ֻ�ܴ��ָ��
//-----------------------------------------------------------------------------
template<typename POINTER>
class RingQueue
{
public:
	//-------------------------------------------------------------------------
	// ��ӵ�����β
	//-------------------------------------------------------------------------
	INLINE BOOL Push(POINTER pItem)
	{
		m_Lock.Lock();
		if( m_lHead == m_lTail && m_bFull == TRUE )	// ������
		{
			m_Lock.Unlock();
			return FALSE;
		}
		
		m_Array[m_lTail++] = pItem;
		if( m_lTail >= m_lMaxSize )
			m_lTail = 0;

		if( m_lTail == m_lHead )
			m_bFull = TRUE;

		++m_lSize;
		m_Lock.Unlock();
		return TRUE;
	}

	//-------------------------------------------------------------------------
	// �Ӷ���ͷȡitem������踺���ͷ�
	//-------------------------------------------------------------------------
	INLINE POINTER Pop()
	{
		POINTER pItem;

		m_Lock.Lock();
		if( m_lHead == m_lTail && m_bFull == FALSE )	// ���п�
		{
			m_Lock.Unlock();
			return NULL;
		}
		
		pItem = m_Array[m_lHead++];	// ȡ��ITEM_POINTER
		if( m_lHead >= m_lMaxSize )
			m_lHead = 0;

		if( m_lHead == m_lTail )
			m_bFull = FALSE;

		--m_lSize;
		m_Lock.Unlock();
		return pItem;
	}

	INLINE LONG GetNum() { return m_lSize; }
	
	RingQueue(LONG lMaxSize): m_lMaxSize(lMaxSize), m_lSize(0), m_lHead(0), m_lTail(0), m_bFull(0)
	{ 
		m_Array = new POINTER[m_lMaxSize]; 
		ZeroMemory(m_Array, sizeof(m_Array));
	}

	INLINE VOID Clear()
	{
		m_Lock.Lock();
		ZeroMemory(m_Array, sizeof(m_Array));
		m_lSize = 0;
		m_lHead = 0;
		m_lTail = 0; 
		m_bFull = 0;
		m_Lock.Unlock();
	}

	~RingQueue() { SAFE_DEL_ARRAY(m_Array); }
	
private:
	AtomMutex		m_Lock;
	
	LONG			m_lMaxSize;				// ����С
	LONG VOLATILE	m_lSize;				// ��ǰ��С
	POINTER*		m_Array;				// ����
	
	LONG			m_lHead;
	LONG			m_lTail;
	BOOL			m_bFull;
};

}
} // namespace ECore{

