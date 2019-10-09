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
// �ṩ���������̰߳�ȫ��map����ValueTypeֻ��Ϊָ��ͻ�������
//-----------------------------------------------------------------------------
template<typename KeyType, typename ValueType, typename LockType=AtomMutex>
class MapTS
{
public:
	//-------------------------------------------------------------------------
	// ���Ԫ��
	//-------------------------------------------------------------------------
	BOOL Add(KeyType key, ValueType value)
	{
		m_Lock.Lock();
		BOOL bResult = m_map.insert(std::make_pair(key, value)).second;
		m_Lock.Unlock();
		return bResult;
	}

	//--------------------------------------------------------------------------
	// ��ȡԪ��
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
	// �ı�ָ��key��Ӧ��ֵ
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
	// ���ָ��Ԫ���Ƿ����
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
	// ɾ��ָ��Ԫ��
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
	// �������Ԫ��
	//-------------------------------------------------------------------------------
	VOID Clear()
	{
		m_Lock.Lock();
		m_map.clear();
		m_Lock.Unlock();
	}

	//-------------------------------------------------------------------------------
	// �õ�Ԫ�ظ���
	//-------------------------------------------------------------------------------
	INT	Size()
	{
		m_Lock.Lock();
		INT nNum = m_map.size();
		m_Lock.Unlock();

		return nNum;
	}

	//-------------------------------------------------------------------------------
	// ������key������һ������std::list,�����ص����ĸ���
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
	// ������value������һ������std::list,�����ص����ĸ���
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
	//// �ⲿ�����ͽ���
	////--------------------------------------------------------------------------------
	//VOID Lock()			{ m_Lock.Acquire(); }
	//VOID Unlock()		{ m_Lock.Release(); }

private:
	std::map<KeyType, ValueType>			m_map;
	LockType								m_Lock;
};

}

#pragma warning(pop)