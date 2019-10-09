//-----------------------------------------------------------------------------
// author Lyp
// date 2004-07-07
// last 2004-07-12
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename Type>
class ListTS
{
public:
	//-------------------------------------------------------------------------
	// 添加元素到队列尾 -- 增加返回值，方便外部做事件处理用
	//-------------------------------------------------------------------------
	INT PushBack(Type val)
	{
		m_Lock.Lock();

		m_list.push_back(val);
		++m_nItemNum;

		INT nRet = m_nItemNum;

		m_Lock.Unlock();

		return nRet;
	}

	//-------------------------------------------------------------------------
	// 添加元素到队列首部
	//-------------------------------------------------------------------------
	INT PushFront(Type val)
	{
		m_Lock.Lock();

		m_list.push_front(val);
		++m_nItemNum;

		INT nRet = m_nItemNum;

		m_Lock.Unlock();

		return nRet;
	}

	//-------------------------------------------------------------------------
	// 从队列头取元素
	//-------------------------------------------------------------------------
	Type PopFront()
	{
		Type val;
		if( m_nItemNum <= 0 )
			return Type(INVALID);

		m_Lock.Lock();
		if( m_nItemNum <= 0 )
		{
			m_Lock.Unlock();
			return Type(INVALID);
		}

		val = m_list.front();
		m_list.pop_front();
		--m_nItemNum;
		
		m_Lock.Unlock();
		return val;
	}

	//--------------------------------------------------------------------------
	// 删除指定元素
	//--------------------------------------------------------------------------
	BOOL Erase(Type& val)
	{
		std::list<Type>::iterator it;

		m_Lock.Lock();

		for(it = m_list.begin(); it != m_list.end(); ++it)
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

	//---------------------------------------------------------------------------
	// 检查指定元素是否存在
	//---------------------------------------------------------------------------
	BOOL IsExist(Type& val)
	{
		BOOL bFound = FALSE;
		std::list<Type>::iterator it;

		m_Lock.Lock();

		for(it = m_list.begin(); it != m_list.end(); ++it)
		{
			if( val == *it )
			{
				bFound = TRUE;
				break;
			}
		}

		m_Lock.Unlock();
		return bFound;
	}

	//-----------------------------------------------------------------------------
	// 清空所有元素
	//-----------------------------------------------------------------------------
	VOID Clear()
	{
		m_Lock.Lock();

		m_list.clear();
		m_nItemNum = 0;

		m_Lock.Unlock();
	}

	//-----------------------------------------------------------------------------
	// 得到元素个数
	//-----------------------------------------------------------------------------
	INT	Size()		{ return m_nItemNum; }

	//-----------------------------------------------------------------------------
	// 安全的将数据导入一个普通std::list,返回数据个数
	//-----------------------------------------------------------------------------
	INT Export(std::list<Type>& listDest)
	{
		INT n = 0;
		std::list<Type>::iterator it;

		m_Lock.Lock();

		for(it = m_list.begin(); it != m_list.end(); ++it, ++n)
			listDest.push_back(*it);
            
		m_Lock.Unlock();
		return n;
	}

	//-------------------------------------------------------------------------
	// 构造函数
	//-------------------------------------------------------------------------
	ListTS():m_nItemNum(0) {}
	~ListTS() { Clear(); }

private:
	std::list<Type>		m_list;
	INT					m_nItemNum;
	AtomMutex			m_Lock;
};



}