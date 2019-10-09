//-----------------------------------------------------------------------------
//!\file simple_map.h
//!\author Lyp
//!
//!\date 2004-10-27
//! last 2004-10-27
//!
//!\brief 提供简单的map管理
//!
//!
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {

//-----------------------------------------------------------------------------
// 提供简单的map管理，ValueType只能为指针和整型
//-----------------------------------------------------------------------------
template<class KeyType, class ValueType, class Compare = less<KeyType> >
class TMap
{
public:
	typedef typename std::map<KeyType, ValueType, Compare>::iterator Iterator;
	typedef typename std::map<KeyType, ValueType, Compare>::const_iterator CIterator;

public:
	//-------------------------------------------------------------------------
	// 添加元素
	//-------------------------------------------------------------------------
	BOOL Add(KeyType key, ValueType value)	{ return m_map.insert(std::make_pair(key, value)).second; }

	//------------------------------------------------------------------------
	// 替换指定key对应的值，如果key不存在，则直接添加（要求ValueType必须有operator=重载）
	//------------------------------------------------------------------------
	VOID Replace(KeyType key, ValueType rep_value)
	{
		Iterator it = m_map.find(key);

		if( it == m_map.end() )
		{
			m_map.insert(std::make_pair(key, rep_value));
		}
		else
		{
			it->second = rep_value;
		}
	}

	//-------------------------------------------------------------------------
	// 读取元素
	//-------------------------------------------------------------------------
	ValueType Peek(KeyType key) const
	{
		CIterator it = m_map.find(key);

		if( it == m_map.end() )
			return ValueType(INVALID);
		else
			return it->second;
	}

	//-------------------------------------------------------------------------
	// 读取元素
	//-------------------------------------------------------------------------
	BOOL Peek(CIterator &iter, KeyType key) const
	{
		iter = m_map.find(key);

		if( iter == m_map.end() )
			return FALSE;
		else
			return TRUE;
	}
	
	//------------------------------------------------------------------------
	// 改变指定key对应的值
	//------------------------------------------------------------------------
	BOOL ChangeValue(KeyType key, ValueType new_value)
	{
		Iterator it = m_map.find(key);

		if( it == m_map.end() )
			return FALSE;

		it->second = new_value;
		return TRUE;
	}

	//------------------------------------------------------------------------
	// 累加指定key对应的值，如果key不存在，则直接添加（要求ValueType必须有operator+和operator=重载）
	//------------------------------------------------------------------------
	VOID ModifyValue(KeyType key, ValueType mod_value)
	{
		Iterator it = m_map.find(key);

		if( it == m_map.end() )
		{
			m_map.insert(std::make_pair(key, mod_value));
		}
		else
		{
			it->second = it->second + mod_value;
		}
	}

	//-------------------------------------------------------------------------
	// 检查指定元素是否存在
	//-------------------------------------------------------------------------
	BOOL IsExist(KeyType key) const
	{
		CIterator it = m_map.find(key);
		if( it == m_map.end() )
			return FALSE;
		else
			return TRUE;
	}

	//-------------------------------------------------------------------------
	// 删除一个指定元素
	//-------------------------------------------------------------------------
	BOOL Erase(KeyType key)
	{ 
		Iterator it = m_map.find(key);
		if( it == m_map.end() )
			return FALSE;

        m_map.erase(it);
		return TRUE;
	}

	//-------------------------------------------------------------------------
	// 清空所有元素
	//-------------------------------------------------------------------------
	VOID Clear() { m_map.clear(); }

	//-------------------------------------------------------------------------
	// 个数相关
	//-------------------------------------------------------------------------
	INT		Size() const	{ return (INT)m_map.size(); }
	BOOL	Empty() const	{ return m_map.empty(); }

	//-------------------------------------------------------------------------
	// 将内部的迭代器初始化到map的开始
	//-------------------------------------------------------------------------
	VOID ResetIterator()	{ m_it = m_map.begin(); }

	//-------------------------------------------------------------------------
	// 得到map的头
	//-------------------------------------------------------------------------
	CIterator Begin() const	{ return m_map.begin(); }
	Iterator  Begin()		{ return m_map.begin(); }

	//-------------------------------------------------------------------------
	// 取出内部的迭代器指向的元素，并后移迭代器
	//-------------------------------------------------------------------------
	BOOL PeekNext(ValueType& value) const
	{
		if( m_it == m_map.end() )
			return FALSE;

		value = m_it->second;
		++m_it;
		return TRUE;
	}

	//-------------------------------------------------------------------------
	// 取出指定迭代器指向的元素，并后移该迭代器
	//-------------------------------------------------------------------------
	BOOL PeekNext(Iterator& it, ValueType& value) const
	{
		if( it == m_map.end() )
			return FALSE;

		value = it->second;
		++it;
		return TRUE;
	}

	//-------------------------------------------------------------------------
	// 取出指定迭代器指向的元素，并后移该迭代器
	//-------------------------------------------------------------------------
	BOOL PeekNext(CIterator& it, ValueType& value) const
	{
		if( it == m_map.end() )
			return FALSE;

		value = it->second;
		++it;
		return TRUE;
	}

	//-------------------------------------------------------------------------
	// 取出内部的迭代器指向的元素，并后移迭代器
	//-------------------------------------------------------------------------
	BOOL PeekNext(KeyType& key, ValueType& value) const
	{
		if( m_it == m_map.end() )
			return FALSE;

		key = m_it->first;
		value = m_it->second;
		++m_it;
		return TRUE;
	}

	//-------------------------------------------------------------------------
	// 取出指定的迭代器指向的元素，并后移迭代器
	//-------------------------------------------------------------------------
	BOOL PeekNext(Iterator& it, KeyType& key, ValueType& value) const
	{
		if( it == m_map.end() )
			return FALSE;

		key = it->first;
		value = it->second;
		++it;
		return TRUE;
	}

	//-------------------------------------------------------------------------
	// 取出指定的迭代器指向的元素，并后移迭代器
	//-------------------------------------------------------------------------
	BOOL PeekNext(CIterator& it, KeyType& key, ValueType& value) const
	{
		if( it == m_map.end() )
			return FALSE;

		key = it->first;
		value = it->second;
		++it;
		return TRUE;
	}

	//-------------------------------------------------------------------------
	// 从map里面随机抽取一个元素
	//-------------------------------------------------------------------------
	BOOL RandPeek(KeyType& key, ValueType& value) const
	{
		std::map<KeyType, ValueType, Compare>::size_type nSize = m_map.size();
		if( nSize <= 0 ) return FALSE;

		INT nRand = rand() % ((INT)nSize);

		CIterator it = m_map.begin();
		for(INT n = 0; n < nRand && it != m_map.end(); ++n)
			++it;

		if( it == m_map.end() ) return FALSE;

		key = it->first;
		value = it->second;
		return TRUE;
	}

	//--------------------------------------------------------------------------
	// 将所有key导出到一个给定std::list
	//--------------------------------------------------------------------------
	VOID ExportAllKey(std::list<KeyType>& listDest) const
	{
		CIterator it;
		for(it = m_map.begin(); it != m_map.end(); ++it)
			listDest.push_back(it->first);
	}
	
	//--------------------------------------------------------------------------
	// 将所有value导出到一个给定std::list
	//--------------------------------------------------------------------------
	VOID ExportAllValue(std::list<ValueType>& listDest) const
	{
		CIterator it;
		for(it = m_map.begin(); it != m_map.end(); ++it)
			listDest.push_back(it->second);
	}


private:
	std::map<KeyType, ValueType, Compare>		m_map;
	mutable CIterator							m_it;
};


} // namespace ECore