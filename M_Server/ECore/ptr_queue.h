//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2003-11-30
// Last: 2009-06-11
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {
//-----------------------------------------------------------------------------
// 指针队列
//-----------------------------------------------------------------------------
template<typename POINTER>
class PtrQueue
{
public:
	//-------------------------------------------------------------------------
	// 添加到队列尾
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
	// 从队列头取item，外边需负责释放
	//-------------------------------------------------------------------------
	INLINE POINTER Get()
	{
		if( m_lItemNum <= 0 )
			return NULL;

		if( m_lItemNum <= 0 )
		{
			return NULL;
		}

		POINTER pItem = m_pFirst;	// 取出ITEM_POINTER
		m_pFirst = m_pFirst->pNext;
		--m_lItemNum;
		return pItem;
	}

	//-------------------------------------------------------------------------
	// 添加到队列首
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
	// 得到队列中的items数目
	//--------------------------------------------------------------------------
	INLINE LONG GetNum()	{ return m_lItemNum; }
	
	INLINE PtrQueue(): m_lItemNum(0),m_pFirst(0), m_pLast(0) {}
	
private:
	LONG VOLATILE	m_lItemNum;			// 队列大小
	POINTER			m_pFirst;			// 头部
	POINTER			m_pLast;			// 尾部
};

} // namespace ECore{

