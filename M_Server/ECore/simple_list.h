//-----------------------------------------------------------------------------
// author Lyp
// date 2005-3-2
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename Type> class TList
{
public:
	typedef typename std::list<Type>::iterator			Iterator;
	typedef typename std::list<Type>::const_iterator	CIterator;

public:
	//-------------------------------------------------------------------------
	// 添加元素到指定位置
	//-------------------------------------------------------------------------
	BOOL Insert(Type val, INT nPos)
	{
		if (nPos > m_nItemNum)
			return FALSE;

		Iterator it = m_list.begin();
		for(INT n=1; n<nPos; n++)
			it++;

		m_list.insert(it, val);
		++m_nItemNum;

		return TRUE;
	}

	//-------------------------------------------------------------------------
	// 添加元素到队列尾
	//-------------------------------------------------------------------------
	VOID PushBack(Type val)
	{
		m_list.push_back(val);
		++m_nItemNum;
	}

	//-------------------------------------------------------------------------
	// 从队列头取元素
	//-------------------------------------------------------------------------
	Type PopFront()
	{
		Type val;
		if( m_nItemNum <= 0 )
			return Type(INVALID);

		val = m_list.front();
		m_list.pop_front();
		--m_nItemNum;

		return val;
	}
	
	//-------------------------------------------------------------------------
	// 从队列尾取元素
	//-------------------------------------------------------------------------
	Type PopBack()
	{
		Type val;
		if( m_nItemNum <= 0 )
			return Type(INVALID);

		val = m_list.back();
		m_list.pop_back();
		--m_nItemNum;

		return val;
	}

	//-------------------------------------------------------------------------
	// 添加元素到队列头
	//-------------------------------------------------------------------------
	VOID PushFront(Type val)
	{
		m_list.push_front(val);
		++m_nItemNum;
	}

	//-------------------------------------------------------------------------
	// 得到第一个元素的值，但并不取出来
	//-------------------------------------------------------------------------
	Type Front() const
	{
		if( m_nItemNum <= 0 )
			return Type(INVALID);

		return m_list.front();
	}
	
	//-------------------------------------------------------------------------
	// 得到第一个元素的值，但并不取出来
	//-------------------------------------------------------------------------
	Type Back() const
	{
		if( m_nItemNum <= 0 )
			return Type(INVALID);

		return m_list.back();
	}

	//-------------------------------------------------------------------------
	// 删除指定元素
	//-------------------------------------------------------------------------
	BOOL Erase(const Type& val)
	{
		Iterator it;
		for(it = m_list.begin(); it != m_list.end(); ++it)
		{
			if( val == *it )
			{
				m_list.erase(it);
				--m_nItemNum;		// 需要处理nItemNum,所以不能直接用list::remove
				return TRUE;
			}
		}
		return FALSE;
	}

	//--------------------------------------------------------------------------
	// 检查指定元素是否存在
	//--------------------------------------------------------------------------
	BOOL IsExist(const Type& val) const
	{
		CIterator cit;
		for(cit = m_list.begin(); cit != m_list.end(); ++cit)
		{
			if( val == *cit )
				return TRUE;
		}
		return FALSE;
	}

	//---------------------------------------------------------------------------
	// 清空所有元素
	//---------------------------------------------------------------------------
	VOID Clear()
	{
		m_list.clear();
		m_nItemNum = 0;
	}

	//---------------------------------------------------------------------------
	// 个数相关
	//---------------------------------------------------------------------------
	INT		Size() const	{ return m_nItemNum; }
	BOOL	Empty() const	{ return (0 == m_nItemNum); }

	//---------------------------------------------------------------------------
	// 从list里面随机读一个元素，并制定是否删除这个元素
	//---------------------------------------------------------------------------
	BOOL RandPeek(Type& value, BOOL bPop=FALSE)
	{
		if( m_list.empty() )
			return FALSE;

		INT nRand = rand() % m_nItemNum;

		Iterator it = m_list.begin();
		for(INT n = 0; n < nRand && it != m_list.end(); ++n)
		{
			++it;
		}

		if( it == m_list.end() ) 
			return FALSE;
       
		value = *it;
		if( bPop )
		{
			m_list.erase(it);
			--m_nItemNum;
		}
			
		return TRUE;
	}

	//-----------------------------------------------------------------------------
	// 取出内部的迭代器指向的元素，并后移迭代器
	//-----------------------------------------------------------------------------
	BOOL PeekNext(Type& value) const
	{
		if( m_it == m_list.end() )
			return FALSE;
		value = *m_it;
		++m_it;
		return TRUE;
	}

	//-----------------------------------------------------------------------------
	// 取出外部的迭代器指向的元素，并后移迭代器
	//-----------------------------------------------------------------------------
	BOOL PeekNext(Iterator& it, Type& value) const
	{
		if( it == m_list.end() )
			return FALSE;

		value = *it;
		++it;
		return TRUE;
	}

	//-----------------------------------------------------------------------------
	// 取出外部的迭代器指向的元素，并后移迭代器
	//-----------------------------------------------------------------------------
	BOOL PeekNext(CIterator& it, Type& value) const
	{
		if( it == m_list.end() )
			return FALSE;

		value = *it;
		++it;
		return TRUE;
	}

	//-----------------------------------------------------------------------------
	// 将内部的迭代器初始化到map的开始
	//-----------------------------------------------------------------------------
	VOID ResetIterator()		{ m_it = m_list.begin(); }

	//-----------------------------------------------------------------------------
	// 得到指向list开始位置的迭代器
	//-----------------------------------------------------------------------------
	Iterator	Begin()			{ return m_list.begin(); }
	CIterator	Begin() const	{ return m_list.begin(); }

	//-----------------------------------------------------------------------------
	// 得到内部的list
	//-----------------------------------------------------------------------------
	std::list<Type>& GetList()	{ return m_list; }


	//-----------------------------------------------------------------------------
	// 相等操作符
	//-----------------------------------------------------------------------------
	VOID operator=(std::list<Type>& list)	{ m_list = list; m_nItemNum = INT(list.size()); m_it = m_list.end(); }
	VOID operator=(TList<Type>& list)		{ m_list = list.GetList(); m_nItemNum = INT(m_list.size()); m_it = m_list.end(); }

	//-----------------------------------------------------------------------------
	// 构造函数和析构函数
	TList(): m_nItemNum(0) {}
	TList(std::list<Type>& list)			{ m_list = list; m_nItemNum = (INT)list.size(); m_it = m_list.end(); }
	TList(const TList<Type>& list)			{ m_list = list.m_list; m_nItemNum = (INT)m_list.size(); m_it = m_list.end(); }
	~TList() {}


private:
	std::list<Type>			m_list;
	INT						m_nItemNum;
	mutable CIterator		m_it;
};



}