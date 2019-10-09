//-----------------------------------------------------------------------------
//!\file simple_map.h
//!\author Lyp
//!
//!\date 2004-10-27
//! last 2004-10-27
//!
//!\brief �ṩ�򵥵�map����
//!
//!
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {

//-----------------------------------------------------------------------------
// �ṩ�򵥵�map����ValueTypeֻ��Ϊָ�������
//-----------------------------------------------------------------------------
template<class KeyType, class ValueType, class Compare = less<KeyType> >
class TMap
{
public:
	typedef typename std::map<KeyType, ValueType, Compare>::iterator Iterator;
	typedef typename std::map<KeyType, ValueType, Compare>::const_iterator CIterator;

public:
	//-------------------------------------------------------------------------
	// ���Ԫ��
	//-------------------------------------------------------------------------
	BOOL Add(KeyType key, ValueType value)	{ return m_map.insert(std::make_pair(key, value)).second; }

	//------------------------------------------------------------------------
	// �滻ָ��key��Ӧ��ֵ�����key�����ڣ���ֱ����ӣ�Ҫ��ValueType������operator=���أ�
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
	// ��ȡԪ��
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
	// ��ȡԪ��
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
	// �ı�ָ��key��Ӧ��ֵ
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
	// �ۼ�ָ��key��Ӧ��ֵ�����key�����ڣ���ֱ����ӣ�Ҫ��ValueType������operator+��operator=���أ�
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
	// ���ָ��Ԫ���Ƿ����
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
	// ɾ��һ��ָ��Ԫ��
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
	// �������Ԫ��
	//-------------------------------------------------------------------------
	VOID Clear() { m_map.clear(); }

	//-------------------------------------------------------------------------
	// �������
	//-------------------------------------------------------------------------
	INT		Size() const	{ return (INT)m_map.size(); }
	BOOL	Empty() const	{ return m_map.empty(); }

	//-------------------------------------------------------------------------
	// ���ڲ��ĵ�������ʼ����map�Ŀ�ʼ
	//-------------------------------------------------------------------------
	VOID ResetIterator()	{ m_it = m_map.begin(); }

	//-------------------------------------------------------------------------
	// �õ�map��ͷ
	//-------------------------------------------------------------------------
	CIterator Begin() const	{ return m_map.begin(); }
	Iterator  Begin()		{ return m_map.begin(); }

	//-------------------------------------------------------------------------
	// ȡ���ڲ��ĵ�����ָ���Ԫ�أ������Ƶ�����
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
	// ȡ��ָ��������ָ���Ԫ�أ������Ƹõ�����
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
	// ȡ��ָ��������ָ���Ԫ�أ������Ƹõ�����
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
	// ȡ���ڲ��ĵ�����ָ���Ԫ�أ������Ƶ�����
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
	// ȡ��ָ���ĵ�����ָ���Ԫ�أ������Ƶ�����
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
	// ȡ��ָ���ĵ�����ָ���Ԫ�أ������Ƶ�����
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
	// ��map���������ȡһ��Ԫ��
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
	// ������key������һ������std::list
	//--------------------------------------------------------------------------
	VOID ExportAllKey(std::list<KeyType>& listDest) const
	{
		CIterator it;
		for(it = m_map.begin(); it != m_map.end(); ++it)
			listDest.push_back(it->first);
	}
	
	//--------------------------------------------------------------------------
	// ������value������һ������std::list
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