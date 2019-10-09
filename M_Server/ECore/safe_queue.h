//--------------------------------------------------------------------------------------------------
// File: safe_queue.h
// Desc: 缓存对象，并提供事件通知机制 -- 可用于异步处理
// Time: 2009-12-30
// Author: Sxg
//---------------------------------------------------------------------------------------------------
#pragma once

#include "event.h"

namespace ECore {

//-----------------------------------------------------------------------------------------------------
template<typename T>
class TSafeQueue
{
public:
	TSafeQueue(): m_Event(NULL)
	{
		m_Event.Create(FALSE, TRUE);
	}

	virtual ~TSafeQueue()
	{
		m_Event.Close();
	}

private:
	TSafeQueue(const TSafeQueue<T>&);
	TSafeQueue& operator = (const TSafeQueue<T>&);

public:
	VOID	Push(T obj)			{ if(1 == m_lstObjs.PushBack(obj)) { m_Event.Set(); } }
	T		Pop()				{ return m_lstObjs.PopFront(); }

	INT		GetNum() const		{ return m_lstObjs.Size(); }

	DWORD	Wait(DWORD dwTime)	{ return m_Event.Wait(dwTime); }

private:
	Event				m_Event;		// 发送通知事件用
	mutable ListTS<T>	m_lstObjs;		// 队列 -- 此处按先进先出操作
};

} // namespace ECore