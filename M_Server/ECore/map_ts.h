//-----------------------------------------------------------------------------
// author Lyp
// date 2004-07-09
// last 2004-08-16
//-----------------------------------------------------------------------------
#pragma once

#pragma warning(push)
#pragma warning(disable: 4267)

namespace ECore {

//-----------------------------------------------------------------------------
// 提供轻量级的线程安全的map管理，ValueType只能为指针和基本类型
//-----------------------------------------------------------------------------
template<typename KeyType, typename ValueType, typename LockType=AtomMutex>
class MapTS
{
public:
	//-------------------------------------------------------------------------
	// 添加元素
	//-------------------------------------------------------------------------
	BOOL Add(KeyType key, ValueType value)
	{
		m_Lock.Lock();
		BOOL bResult = m_map.insert(std::make_pair(key, value)).second;
		m_Lock.Unlock();
		return bResult;
	}

	//--------------------------------------------------------------------------
	// 读取元素
	//--------------------------------------------------------------------------
	ValueType Peek(KeyType key)
	{
		ValueType temp;
		std::map<KeyType, ValueType>::iterator it;

		m_Lock.Lock();

		it = m_map.find(key);
		if( it == m_map.end() )
			temp = ValueType(INVALID);
		else
			temp = it->second;

		m_Lock.Unlock();
		return temp;
	}

	//---------------------------------------------------------------------------
	// 改变指定key对应的值
	//---------------------------------------------------------------------------
	BOOL ChangeValue(KeyType key, ValueType new_value)
	{
		std::map<KeyType, ValueType>::iterator it;

		m_Lock.Lock();

		it = m_map.find(key);
		if( it == m_map.end() )
		{
			m_Lock.Unlock();
			return FALSE;
		}
		else
		{
			it->second = new_value;
			m_Lock.Unlock();
			return TRUE;
		}
	}

	//----------------------------------------------------------------------------
	// 检查指定元素是否存在
	//----------------------------------------------------------------------------
	BOOL IsExist(KeyType key)
	{
		BOOL bResult = FALSE;
		std::map<KeyType, ValueType>::iterator it;

		m_Lock.Lock();

		it = m_map.find(key);
		if( it == m_map.end() )
			bResult = FALSE;
		else
			bResult = TRUE;

		m_Lock.Unlock();
		return bResult;
	}

	//-----------------------------------------------------------------------------
	// 删除指定元素
	//-----------------------------------------------------------------------------
	BOOL Erase(KeyType key)
	{
		BOOL bResult = FALSE;
		std::map<KeyType, ValueType>::iterator it;
		
		m_Lock.Lock();

		it = m_map.find(key);
		if( it == m_map.end() )
		{
			bResult = FALSE;
		}
		else
		{
			m_map.erase(it);
			bResult = TRUE;
		}

		m_Lock.Unlock();
		return bResult;
	}

	//-------------------------------------------------------------------------------
	// 清空所有元素
	//-------------------------------------------------------------------------------
	VOID Clear()
	{
		m_Lock.Lock();
		m_map.clear();
		m_Lock.Unlock();
	}

	//-------------------------------------------------------------------------------
	// 得到元素个数
	//-------------------------------------------------------------------------------
	INT	Size()
	{
		m_Lock.Lock();
		INT nNum = m_map.size();
		m_Lock.Unlock();

		return nNum;
	}

	//-------------------------------------------------------------------------------
	// 将所有key导出到一个给定std::list,并返回导出的个数
	//-------------------------------------------------------------------------------
	INT ExportAllKey(std::list<KeyType>& listDest)
	{
		INT n = 0;
		std::map<KeyType, ValueType>::iterator it;

		m_Lock.Lock();

		for(it = m_map.begin(); it != m_map.end(); ++it, ++n)
			listDest.push_back(it->first);

		m_Lock.Unlock();
		return n;
	}

	//-------------------------------------------------------------------------------
	// 将所有value导出到一个给定std::list,并返回导出的个数
	//-------------------------------------------------------------------------------
	INT ExportAllValue(std::list<ValueType>& listDest)
	{
		INT n = 0;
		std::map<KeyType, ValueType>::iterator it;

		m_Lock.Lock();

		for(it = m_map.begin(); it != m_map.end(); ++it, ++n)
			listDest.push_back(it->second);

		m_Lock.Unlock();
		return n;
	}

	////--------------------------------------------------------------------------------
	//// 外部锁定和解锁
	////--------------------------------------------------------------------------------
	//VOID Lock()			{ m_Lock.Acquire(); }
	//VOID Unlock()		{ m_Lock.Release(); }

private:
	std::map<KeyType, ValueType>			m_map;
	LockType								m_Lock;
};

}

#pragma warning(pop)