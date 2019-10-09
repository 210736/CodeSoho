//-----------------------------------------------------------------------------
// Auth: wb
// Date: 2011/5/20
//-----------------------------------------------------------------------------

#pragma once

namespace ECore
{

#ifdef MEM_TRACE

#define DynaArray std::vector

#else if
	/** 
	 * 动态数组
	 *
	 * @remark 添加/删除元素时会自动调用构造/析构函数
	 * @remark 开辟空间时使用memcpy快速拷贝数据，只适用于支持memcpy的数据类型
	 * @remark 效率比较：SimpleVector > DynaArray > std::vector
	 */
	template<typename T>
	class DynaArray
	{
	public:
		/**
		 *	Constructor
		 */
		DynaArray()
			: m_size(0)
			, m_capacity(0)
			, m_pData(0)
		{}

		/**
		 *	Constructor with capacity
		 */
		DynaArray(size_t _capacity,size_t _size=0)
			: m_size(_size)
			, m_capacity(_capacity)
			, m_pData(0)
		{
			ASSERT( _capacity >= _size );
			m_pData = (T*)MCALLOC((DWORD)(sizeof(T)*_capacity));
			::new(m_pData) T[_size];
		}

		/**
		 *	Destructor
		 */
		~DynaArray()
		{	
			for(size_t n=0; n<m_size; ++n)
				m_pData[n].~T();
			MCFREE(m_pData);
		}

		/**
		 *	Copyconstructor
		 */
		DynaArray( const DynaArray& rhs )
			: m_size(0)
			, m_capacity(0)
			, m_pData(0)
		{
			*this = rhs;
		}

		/**
		 *	operator=
		 */
		INLINE const DynaArray& operator=( const DynaArray& rhs )
		{
			if( this == &rhs )
				return *this;

			for(size_t n=0; n<m_size; ++n)
				m_pData[n].~T();
			MCFREE(m_pData);
			m_pData = 0;

			m_capacity = rhs.m_capacity;
			m_size = rhs.m_size;

			if( m_capacity )
			{
				m_pData = (T*)MCALLOC((DWORD)(sizeof(T)*m_capacity));
				if( m_size )
					memcpy( m_pData, rhs.m_pData, m_size * sizeof(T) );
			}

			return *this;
		}

		/** 
		 *	返回存储空间大小
		 */
		INLINE size_t capacity() const { return m_capacity; }

		/** 
		 *	返回元素数量
		 */
		INLINE size_t size() const { return m_size; }

		/**
		 *	返回是否为空
		 */
		INLINE BOOL empty() const { return !m_size; }

		/**
		 *	返回第一个元素引用
		 */
		INLINE T& back() 
		{  
			ASSERT( m_size );
			return m_pData[m_size-1];
		}

		/**
		 *	返回最后一个元素引用
		 */
		INLINE T& front()
		{
			ASSERT( m_size );
			return *m_pData; 
		}

		/**
		 *	返回首地址
		 */
		INLINE T* begin() { return m_pData; }

		/**
		 *	返回首地址常量指针
		 */
		INLINE const T* begin_const() const { return m_pData; }

		/**
		 *	返回尾地址
		 */
		INLINE T* end() 
		{
			if( m_pData )
				return m_pData + m_size; 
			return NULL;
		}

		/**
		 *	括符操作符
		 */
		INLINE T& operator[](size_t index)
		{
			ASSERT(index<m_size);
			return m_pData[index];
		}

		/**
		 *	括符操作符
		 */
		INLINE const T& operator[](size_t index) const
		{
			ASSERT(index<m_size);
			return m_pData[index];
		}

		/**
		 *	尾部添加
		 */
		INLINE void push_back(const T& _val)
		{
			grow();

			ASSERT( m_capacity >= m_size );
			m_pData[m_size-1] = _val;
		}

		/**
		 *	删除尾部元素
		 */
		INLINE void pop_back()
		{
			ASSERT( m_size > 0 );
			--m_size;
			m_pData[m_size].~T();
		}

		/** 
		 *	size加1，不添加数据
		 */
		INLINE void grow()
		{
			if( m_capacity <= m_size )
			{
				size_t new_capacity = EMax( 10, m_capacity * 2 );
				reserve( new_capacity );
			}

			ASSERT( m_capacity > m_size );
			::new(&m_pData[m_size]) T;
			++m_size;
		}

		/** 
		 *	清空所有元素
		 */
		INLINE void clear()
		{
			for(size_t n=0; n<m_size; ++n)
				m_pData[n].~T();
			m_size = 0;
		}

		INLINE void free_all()
		{
			clear();
			MCFREE(m_pData);
			m_pData = 0;
			m_capacity = 0;
		}

		/** 
		 *	设置元素数量
		 */
		INLINE void resize( size_t newsize)
		{
			if( newsize == m_size )
				return;

			if( m_capacity < newsize )
				reserve( newsize );

			if( newsize > m_size )
			{
				for(size_t n=m_size; n<newsize; ++n)
					::new(&m_pData[n]) T;
			}
			else
			{
				for(size_t n=newsize; n<m_size; ++n)
					m_pData[n].~T();				
			}

			m_size = newsize;
		}

		/** 
		 *	快速删除元素，注意只是与最后一个元素交换，然后再删除
		 */
		INLINE void erase( size_t index )
		{
			ASSERT( index < m_size );
			m_pData[index].~T();

			if( index < m_size - 1 )//not is the last element
				m_pData[index] = m_pData[m_size-1];

			--m_size;
		}

		/**
		 *	删除元素，并保持元素顺序，需要拷贝数据，效率较低
		 */
		INLINE void erase_keep_order( size_t index )
		{
			ASSERT( index < m_size );
			m_pData[index].~T();

			if( index < m_size - 1 )//not is the last element
			{
				for( size_t i = index; i < m_size - 1; ++i )
					m_pData[i] = m_pData[i+1];
			}

			--m_size;
		}

		/**
		 *	插入元素，需要拷贝数据、效率较低
		 */
		INLINE void insert( size_t index, const T& _val )
		{
			ASSERT( index <= m_size );

			// size加1
			grow();

			// 腾出位置
			for( size_t i = m_size - 1; i > index; --i )
				m_pData[i] = m_pData[i-1];

			// 插入
			m_pData[index] = _val;
		}

		/** 
		 *	开辟新的空间
		 */
		INLINE void reserve(size_t _capacity)
		{
			//ASSERT( m_capacity == 0 );
			ASSERT( _capacity > m_size );

			// allocate new space
			m_capacity = _capacity;
			
			T* pNewData = (T*)MCALLOC((DWORD)(sizeof(T)*m_capacity));
			
			// copy data if necessary
			if( m_size )
				memcpy( pNewData, m_pData, m_size * sizeof(T) );
				
			MCFREE(m_pData);

			// hold on pointer of new space 
			m_pData = pNewData;
		}

	private:
		T*		m_pData;
		size_t	m_size;
		size_t	m_capacity;
	};
#endif

}//namespace ECore