//-----------------------------------------------------------------------------
// author Lyp
// date 2004-07-07
// last 2009-06-11
//------------------------------------------------------------------------------
#pragma once

namespace ECore {
namespace Wan {

//------------------------------------------------------------------------------
// ��std::list��װ���̰߳�ȫ�����ÿ��ٷ���������������Ч�ʣ�ֻ�ܶ�ָ������Ͳ���
//------------------------------------------------------------------------------
template<typename Type>
class List
{
public:
	//--------------------------------------------------------------------------
	// ���Ԫ�ص�����β
	//--------------------------------------------------------------------------
	INLINE VOID PushBack(Type val)
	{
		m_Lock.Lock();
		m_list.push_back(val);
		++m_nItemNum;
		m_Lock.Unlock();
	}

	//--------------------------------------------------------------------------
	// ���Ԫ�ص�����β
	//--------------------------------------------------------------------------
	INLINE VOID PushFront(Type val)
	{
		m_Lock.Lock();
		m_list.push_front(val);
		++m_nItemNum;
		m_Lock.Unlock();
	}

	//-------------------------------------------------------------------------
	// �Ӷ���ͷȡԪ��
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
	// ɾ��ָ��Ԫ��
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
	// ���ָ��Ԫ���Ƿ����
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
	// �������Ԫ��
	//-------------------------------------------------------------------------
	INLINE VOID Clear()
	{
		m_Lock.Lock();
		m_list.clear();
		m_nItemNum=0;
		m_Lock.Unlock();
	}

	//-------------------------------------------------------------------------
	// �õ�Ԫ����Ŀ
	//-------------------------------------------------------------------------
	INLINE INT Size() { return m_nItemNum;	}

	//-------------------------------------------------------------------------
	// ȡ���ڲ��ĵ�����ָ���Ԫ�أ������Ƶ�����,ע��Ҫlock�����ʹ��
	//-------------------------------------------------------------------------
	INLINE BOOL _Peek(Type& value)
	{
		if( m_it == m_list.end() )
			return FALSE;
		value = *m_it;
		return TRUE;
	}

	//-------------------------------------------------------------------------
	// ���ڲ��ĵ�������ʼ����map�Ŀ�ʼ,ע��Ҫlock�����ʹ��
	//-------------------------------------------------------------------------
	INLINE VOID _ResetIterator()	{ m_it = m_list.begin(); }
	INLINE VOID _AddIterator()		{ ++m_it; }
	INLINE VOID _EraseCurrent()		{ m_list.erase(m_it++); }

	//-------------------------------------------------------------------------
	// �����ͽ���,�ⲿҪ��
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