#pragma once

namespace ECore
{
	/** \class TSList
		\brief 线程安全List
	*/
	template<typename T>
	class TSList
	{
	public:
		TSList(void)	
		{
			m_iter = m_list.end();
		}
		~TSList(void)	{}

		size_t size() 
		{ 
			size_t sz = 0;
			m_lock.Lock();
			sz = m_list.size();
			m_lock.Unlock();
			return sz;
		}

		void push_back(const T& val)	
		{
			m_lock.Lock();
			m_list.push_back(val);
			m_lock.Unlock();
		}

		void remove(const T& val)
		{
			m_lock.Lock();
			m_list.remove(val);
			m_lock.Unlock();
		}

		bool begin_iterator()
		{
			m_lock.Lock();
			m_iter = m_list.begin();
			return m_iter != m_list.end();
		}

		bool goto_next()
		{
			++m_iter;
			if(m_iter == m_list.end())
				return false;
			else
				return true;
		}

		void get_current(T& val)
		{
			val = (*m_iter);
		}

		void remove_current()
		{
			if(m_iter != m_list.end())
			{
				m_iter = m_list.erase(m_iter);
			}
		}

		void end_iterator()
		{
			m_lock.Unlock();
			m_iter = m_list.end();
		}
		
		void clear()
		{
			m_lock.Lock();
			m_list.clear();
			m_iter = m_list.end();
			m_lock.Unlock();
		}

	private:
		typename std::list<T>			m_list;
		typename std::list<T>::iterator	m_iter;

		AtomMutex		m_lock;
	};
	/** unit test
	//--
	TSList<int> testList;
	for(int i=0;i<10;i++)
	{
	testList.push_back(i);
	}

	testList.remove(2);
	testList.remove(5);

	bool bNext = testList.begin_iterator();
	while(bNext)
	{
	int c;
	testList.get_current(c);
	bNext = testList.goto_next();

	_Trace("test list:%d\r\n",c);
	}

	testList.end_iterator();
	**/
}//namespace ECore