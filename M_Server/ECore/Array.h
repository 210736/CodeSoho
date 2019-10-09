#pragma once

namespace ECore
{
	/** \class Array
		\brief 固定长度数组
	*/
	template<typename T,size_t MaxSize>
	class Array
	{
		T m_array[MaxSize];
		size_t m_size;
	public:
		Array()	: m_size(0)
		{}
		Array(size_t _size)	: m_size(_size)
		{
			ASSERT( _size <= MaxSize );
		}

		INLINE size_t max_size() const	{ return MaxSize;		}
		INLINE size_t size() const		{ return m_size;		}
		INLINE bool empty() const		{ return m_size==0;		}

		INLINE T& operator[](size_t index)
		{
			ASSERT(index<m_size);
			return m_array[index];
		}

		INLINE const T& operator[](size_t index) const
		{
			ASSERT(index<m_size);
			return m_array[index];
		}

		INLINE T& Get(size_t index)
		{
			ASSERT(index<m_size);
			return m_array[index];
		}

		INLINE T& back()
		{
			ASSERT(m_size);
			return m_array[m_size-1];
		}

		INLINE T* begin()	{ return m_array; }
		INLINE T* end()	{ return m_array + m_size;	}

		INLINE void push_back(const T& val)
		{
			if( m_size >= MaxSize )
			{
				ERR( _T("Array overwrite!") );
				return;
			}
			m_array[m_size++]=val;
		}

		INLINE void clear(){ m_size=0;}
		INLINE void resize(size_t size)
		{
			ASSERT(size<=MaxSize);
			m_size=size;
		}

		INLINE void grow()
		{
			ASSERT( m_size < MaxSize );

			++m_size;
		}

		INLINE void clone(Array& other) const
		{
			ASSERT( other.max_size() == MaxSize );

			other.m_size=m_size;
			if(m_size>0)
				memcpy(other.m_array,m_array,m_size*sizeof(T));
		}

		INLINE void erase(size_t index)
		{
			ASSERT(index<m_size);
			if(index<m_size-1)//not is the last element
				m_array[index]=m_array[m_size-1];
			--m_size;
		}
	};

}//namespace ECore