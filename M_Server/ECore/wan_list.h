//-----------------------------------------------------------------------------
// author Lyp
// date 2004-07-07
// last 2009-06-11
//------------------------------------------------------------------------------
#pragma once

namespace ECore {
namespace Wan {

//------------------------------------------------------------------------------
// 将std::list包装成线程安全，采用快速非重入性锁以增加效率，只能对指针和整型操作
//------------------------------------------------------------------------------
template<typename Type>
class List
{
public:
	//--------------------------------------------------------------------------
	// 添加元素到队列尾
	//--------------------------------------------------------------------------
	INLINE VOID PushBack(Type val)
	{
		m_Lock.Lock();
		m_list.push_back(val);
		++m_nItemNum;
		m_Lock.Unlock();
	}

	//--------------------------------------------------------------------------
	// 添加元素到队列尾
	//--------------------------------------------------------------------------
	INLINE VOID PushFront(Type val)
	{
		m_Lock.Lock();
		m_list.push_front(val);
		++m_nItemNum;
		m_Lock.Unlock();
	}

	//-------------------------------------------------------------------------
	// 从队列头取元素
	//-------------------------------------------------------------------------
	INLINE Type PopFront()
	{
		if( m_nItemNum <= 0 )
			return Type(INVALID);

		m_Lock.Lock();
		if( m_nItemNum <= 0 )
		{
			m_Lock.Unlock();
			return Type(INVALID);
		}
		Type val = m_list.front();
		m_list.pop_front();
		--m_nItemNum;
		m_Lock.Unlock();
		return val;
	}
	
	//-------------------------------------------------------------------------
	// 删除指定元素
	//-------------------------------------------------------------------------
	INLINE BOOL Erase(Type& val)
	{
		std::list<Type>::iterator it;

		m_Lock.Lock();
		for(it=m_list.begin(); it!=m_list.end(); ++it)
		{
			if( val == *it )
			{
				m_list.erase(it);
				--m_nItemNum;
				m_Lock.Unlock();
				return TRUE;
			}
		}
		m_Lock.Unlock();

		return FALSE;
	}

	//-------------------------------------------------------------------------
	// 检查指定元素是否存在
	//-------------------------------------------------------------------------
	INLINE BOOL IsExist(Type& val)
	{
		m_Lock.Lock();
		std::list<Type>::iterator it;
		for(it=m_list.begin(); it!=m_list.end(); ++it)
		{
			if( val == *it )
			{
				m_Lock.Unlock();
				return TRUE
			}
		}
		m_Lock.Unlock();
		return FALSE;
	}

	//-------------------------------------------------------------------------
	// 清空所有元素
	//-------------------------------------------------------------------------
	INLINE VOID Clear()
	{
		m_Lock.Lock();
		m_list.clear();
		m_nItemNum=0;
		m_Lock.Unlock();
	}

	//-------------------------------------------------------------------------
	// 得到元素数目
	//-------------------------------------------------------------------------
	INLINE INT Size() { return m_nItemNum;	}

	//-------------------------------------------------------------------------
	// 取出内部的迭代器指向的元素，并后移迭代器,注意要lock后才能使用
	//-------------------------------------------------------------------------
	INLINE BOOL _Peek(Type& value)
	{
		if( m_it == m_list.end() )
			return FALSE;
		value = *m_it;
		return TRUE;
	}

	//-------------------------------------------------------------------------
	// 将内部的迭代器初始化到map的开始,注意要lock后才能使用
	//-------------------------------------------------------------------------
	INLINE VOID _ResetIterator()	{ m_it = m_list.begin(); }
	INLINE VOID _AddIterator()		{ ++m_it; }
	INLINE VOID _EraseCurrent()		{ m_list.erase(m_it++); }

	//-------------------------------------------------------------------------
	// 锁定和解锁,外部要用
	//-------------------------------------------------------------------------
	INLINE VOID Lock()		{ m_Lock.Lock(); }
	INLINE VOID Unlock()	{ m_Lock.Unlock(); }

	INLINE List(): m_nItemNum(0){}
private:
	std::list<Type>						m_list;
	typename std::list<Type>::iterator	m_it;
	
	INT	VOLATILE						m_nItemNum;
	AtomMutex							m_Lock;
};

}
}