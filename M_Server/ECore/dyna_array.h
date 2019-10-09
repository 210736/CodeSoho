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
	 * ��̬����
	 *
	 * @remark ���/ɾ��Ԫ��ʱ���Զ����ù���/��������
	 * @remark ���ٿռ�ʱʹ��memcpy���ٿ������ݣ�ֻ������֧��memcpy����������
	 * @remark Ч�ʱȽϣ�SimpleVector > DynaArray > std::vector
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
		 *	���ش洢�ռ��С
		 */
		INLINE size_t capacity() const { return m_capacity; }

		/** 
		 *	����Ԫ������
		 */
		INLINE size_t size() const { return m_size; }

		/**
		 *	�����Ƿ�Ϊ��
		 */
		INLINE BOOL empty() const { return !m_size; }

		/**
		 *	���ص�һ��Ԫ������
		 */
		INLINE T& back() 
		{  
			ASSERT( m_size );
			return m_pData[m_size-1];
		}

		/**
		 *	�������һ��Ԫ������
		 */
		INLINE T& front()
		{
			ASSERT( m_size );
			return *m_pData; 
		}

		/**
		 *	�����׵�ַ
		 */
		INLINE T* begin() { return m_pData; }

		/**
		 *	�����׵�ַ����ָ��
		 */
		INLINE const T* begin_const() const { return m_pData; }

		/**
		 *	����β��ַ
		 */
		INLINE T* end() 
		{
			if( m_pData )
				return m_pData + m_size; 
			return NULL;
		}

		/**
		 *	����������
		 */
		INLINE T& operator[](size_t index)
		{
			ASSERT(index<m_size);
			return m_pData[index];
		}

		/**
		 *	����������
		 */
		INLINE const T& operator[](size_t index) const
		{
			ASSERT(index<m_size);
			return m_pData[index];
		}

		/**
		 *	β�����
		 */
		INLINE void push_back(const T& _val)
		{
			grow();

			ASSERT( m_capacity >= m_size );
			m_pData[m_size-1] = _val;
		}

		/**
		 *	ɾ��β��Ԫ��
		 */
		INLINE void pop_back()
		{
			ASSERT( m_size > 0 );
			--m_size;
			m_pData[m_size].~T();
		}

		/** 
		 *	size��1�����������
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
		 *	�������Ԫ��
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
		 *	����Ԫ������
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
		 *	����ɾ��Ԫ�أ�ע��ֻ�������һ��Ԫ�ؽ�����Ȼ����ɾ��
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
		 *	ɾ��Ԫ�أ�������Ԫ��˳����Ҫ�������ݣ�Ч�ʽϵ�
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
		 *	����Ԫ�أ���Ҫ�������ݡ�Ч�ʽϵ�
		 */
		INLINE void insert( size_t index, const T& _val )
		{
			ASSERT( index <= m_size );

			// size��1
			grow();

			// �ڳ�λ��
			for( size_t i = m_size - 1; i > index; --i )
				m_pData[i] = m_pData[i-1];

			// ����
			m_pData[index] = _val;
		}

		/** 
		 *	�����µĿռ�
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