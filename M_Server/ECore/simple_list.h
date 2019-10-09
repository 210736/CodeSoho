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
	// ���Ԫ�ص�ָ��λ��
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
	// ���Ԫ�ص�����β
	//-------------------------------------------------------------------------
	VOID PushBack(Type val)
	{
		m_list.push_back(val);
		++m_nItemNum;
	}

	//-------------------------------------------------------------------------
	// �Ӷ���ͷȡԪ��
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
	// �Ӷ���βȡԪ��
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
	// ���Ԫ�ص�����ͷ
	//-------------------------------------------------------------------------
	VOID PushFront(Type val)
	{
		m_list.push_front(val);
		++m_nItemNum;
	}

	//-------------------------------------------------------------------------
	// �õ���һ��Ԫ�ص�ֵ��������ȡ����
	//-------------------------------------------------------------------------
	Type Front() const
	{
		if( m_nItemNum <= 0 )
			return Type(INVALID);

		return m_list.front();
	}
	
	//-------------------------------------------------------------------------
	// �õ���һ��Ԫ�ص�ֵ��������ȡ����
	//-------------------------------------------------------------------------
	Type Back() const
	{
		if( m_nItemNum <= 0 )
			return Type(INVALID);

		return m_list.back();
	}

	//-------------------------------------------------------------------------
	// ɾ��ָ��Ԫ��
	//-------------------------------------------------------------------------
	BOOL Erase(const Type& val)
	{
		Iterator it;
		for(it = m_list.begin(); it != m_list.end(); ++it)
		{
			if( val == *it )
			{
				m_list.erase(it);
				--m_nItemNum;		// ��Ҫ����nItemNum,���Բ���ֱ����list::remove
				return TRUE;
			}
		}
		return FALSE;
	}

	//--------------------------------------------------------------------------
	// ���ָ��Ԫ���Ƿ����
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
	// �������Ԫ��
	//---------------------------------------------------------------------------
	VOID Clear()
	{
		m_list.clear();
		m_nItemNum = 0;
	}

	//---------------------------------------------------------------------------
	// �������
	//---------------------------------------------------------------------------
	INT		Size() const	{ return m_nItemNum; }
	BOOL	Empty() const	{ return (0 == m_nItemNum); }

	//---------------------------------------------------------------------------
	// ��list���������һ��Ԫ�أ����ƶ��Ƿ�ɾ�����Ԫ��
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
	// ȡ���ڲ��ĵ�����ָ���Ԫ�أ������Ƶ�����
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
	// ȡ���ⲿ�ĵ�����ָ���Ԫ�أ������Ƶ�����
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
	// ȡ���ⲿ�ĵ�����ָ���Ԫ�أ������Ƶ�����
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
	// ���ڲ��ĵ�������ʼ����map�Ŀ�ʼ
	//-----------------------------------------------------------------------------
	VOID ResetIterator()		{ m_it = m_list.begin(); }

	//-----------------------------------------------------------------------------
	// �õ�ָ��list��ʼλ�õĵ�����
	//-----------------------------------------------------------------------------
	Iterator	Begin()			{ return m_list.begin(); }
	CIterator	Begin() const	{ return m_list.begin(); }

	//-----------------------------------------------------------------------------
	// �õ��ڲ���list
	//-----------------------------------------------------------------------------
	std::list<Type>& GetList()	{ return m_list; }


	//-----------------------------------------------------------------------------
	// ��Ȳ�����
	//-----------------------------------------------------------------------------
	VOID operator=(std::list<Type>& list)	{ m_list = list; m_nItemNum = INT(list.size()); m_it = m_list.end(); }
	VOID operator=(TList<Type>& list)		{ m_list = list.GetList(); m_nItemNum = INT(m_list.size()); m_it = m_list.end(); }

	//-----------------------------------------------------------------------------
	// ���캯������������
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