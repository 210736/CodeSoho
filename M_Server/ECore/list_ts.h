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
	// ���Ԫ�ص�����β -- ���ӷ���ֵ�������ⲿ���¼�������
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
	// ���Ԫ�ص������ײ�
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
	// �Ӷ���ͷȡԪ��
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
	// ɾ��ָ��Ԫ��
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
	// ���ָ��Ԫ���Ƿ����
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
	// �������Ԫ��
	//-----------------------------------------------------------------------------
	VOID Clear()
	{
		m_Lock.Lock();

		m_list.clear();
		m_nItemNum = 0;

		m_Lock.Unlock();
	}

	//-----------------------------------------------------------------------------
	// �õ�Ԫ�ظ���
	//-----------------------------------------------------------------------------
	INT	Size()		{ return m_nItemNum; }

	//-----------------------------------------------------------------------------
	// ��ȫ�Ľ����ݵ���һ����ͨstd::list,�������ݸ���
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
	// ���캯��
	//-------------------------------------------------------------------------
	ListTS():m_nItemNum(0) {}
	~ListTS() { Clear(); }

private:
	std::list<Type>		m_list;
	INT					m_nItemNum;
	AtomMutex			m_Lock;
};



}