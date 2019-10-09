//--------------------------------------------------------------------------------------------------
// File: safe_queue.h
// Desc: 缓存对象，并提供事件通知机制 -- 可用于异步处理
// Time: 2009-12-30
// Author: lyp
//---------------------------------------------------------------------------------------------------
#pragma once

namespace ECore {
//-----------------------------------------------------------------------------
// Queue: 提供线程安全的先进先出指针队列管理，只能存放指针
//-----------------------------------------------------------------------------
template<typename POINTER>
class SafeTinyQueue
{
public:
	//-------------------------------------------------------------------------
	// 添加到队列尾
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
	// 从队列头取item，外边需负责释放
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

		pItem = m_pFirst;	// 取出ITEM_POINTER
		m_pFirst = m_pFirst->pNext;
		--m_lItemNum;
		m_Lock.Unlock();
		return pItem;
	}

	//-------------------------------------------------------------------------
	// 添加到队列首
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
	// 得到队列中的items数目
	//--------------------------------------------------------------------------
	INLINE LONG GetNum()	{ return m_lItemNum; }

	INLINE SafeTinyQueue(): m_lItemNum(0),m_pFirst(0), m_pLast(0) {}

private:
	Mutex			m_Lock;
	LONG VOLATILE	m_lItemNum;			// 队列大小
	POINTER			m_pFirst;			// 头部
	POINTER			m_pLast;			// 尾部
};

} // namespace ECore
