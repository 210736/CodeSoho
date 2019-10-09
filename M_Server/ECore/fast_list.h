#pragma once

namespace ECore
{
	// 使用内存池的List
	template<typename T>
	class FList
	{
		struct Node
		{
			T		data;
			Node*	next;
		};

	public:
		class Iterator
		{
			friend class FList;
		public:

			Iterator(): m_pCurr(NULL), m_pPrev(NULL) {}
			Iterator( Node* pHead ): m_pCurr(pHead), m_pPrev(NULL) {}
			Iterator( const Iterator& rhs )	{ m_pPrev = rhs.m_pPrev; m_pCurr = rhs.m_pCurr;	}

			bool IsEnd() const { return NULL == m_pCurr; }
			T& operator*() { ASSERT( NULL != m_pCurr );	return m_pCurr->data; }
			T* operator->()	{ ASSERT( NULL != m_pCurr ); return &m_pCurr->data;	}
			void operator ++()	{ m_pPrev = m_pCurr; m_pCurr = m_pCurr->next; }
			void operator =( const Iterator& rhs ) { m_pCurr = rhs.m_pCurr; m_pPrev = rhs.m_pPrev; }
			bool operator ==( const Iterator& rhs ) const {	return m_pCurr == rhs.m_pCurr; }
			bool operator !=( const Iterator& rhs ) const { return m_pCurr != rhs.m_pCurr; }

		private:
			Node* m_pCurr;
			Node* m_pPrev;
		};

	public:

		FList( MemCache<DummyMutex>* pCache = NULL ) : m_pCache(pCache), m_pHead(NULL), m_pTail(NULL), m_size(0) {}
		~FList() { Clear(); }

		FList(FList& lst, MemCache<DummyMutex>* pCache) : m_pCache(pCache), m_pHead(NULL), m_pTail(NULL), m_size(0)
		{ 
			Iterator& it = lst.Begin();
			while( it.IsEnd() )
			{
				PushBack(*it);
				++it;
			}
		}

		/** 
		*	SetCache
		*/
		void SetCache(MemCache<DummyMutex>* pCache)
		{
			ASSERT( Empty() );// 设置cache时，list必须为空
			m_pCache = pCache;
		}

		/** 
		*	PushBack
		*/
		void PushBack(const T& val)
		{
			Node* pNode = (Node*)m_pCache->Alloc( sizeof(Node) );
			pNode->data = val;
			pNode->next = NULL;

			if( NULL == m_pHead )
			{
				m_pHead = pNode;
				m_pTail = pNode;
			}
			else
			{
				ASSERT( m_pTail != NULL );
				m_pTail->next = pNode;
				m_pTail = pNode;
			}

			++m_size;
		}

		/** 
		*	PushFront
		*/
		void PushFront(const T& val)
		{
			Node* pNode = (Node*)m_pCache->Alloc( sizeof(Node) );
			pNode->data = val;
			pNode->next = NULL;

			if( NULL == m_pHead )
			{
				m_pHead = pNode;
				m_pTail = pNode;
			}
			else
			{
				pNode->next = m_pHead;
				m_pHead = pNode;
			}

			++m_size;
		}

		/** 
		*	PopFront
		*/
		void PopFront()
		{
			if( NULL == m_pHead )
				return;

			Node* p = m_pHead;

			m_pHead = m_pHead->next;
			if( NULL == m_pHead )
				m_pTail = NULL;

			m_pCache->Free( p );

			--m_size;
		}

		bool Empty() const { return NULL == m_pHead; }
		size_t Size() const { return m_size; }
		
		T& Front() const { ASSERT( m_pHead != NULL ); return m_pHead->data;	}

		void Clear()
		{
			Node* curr = m_pHead;
			while( curr != NULL )
			{
				Node* temp = curr;
				curr = curr->next;
				m_pCache->Free( temp);
			}

			m_pHead = NULL;
			m_pTail = NULL;

			m_size = 0;
		}

		void Remove(const T& val)
		{
			Iterator it = this->Begin();
			while( it.IsEnd() )
			{
				if( *it == val )
				{
					Erase(it);
					break;
				}
				++it;
			}
		}

		// Erase会自动后移Iterator
		void Erase( Iterator& iter )
		{
			ASSERT( iter.m_pCurr );
			if( NULL == iter.m_pPrev )
			{
				PopFront();
				iter.m_pCurr = m_pHead;
			}
			else
			{
				Node* temp = iter.m_pCurr;
				iter.m_pPrev->next = iter.m_pCurr->next;
				if( iter.m_pCurr == m_pTail )
				{
					m_pTail = iter.m_pPrev;
				}
				
				iter.m_pCurr = iter.m_pCurr->next;

				m_pCache->Free( temp);
				--m_size;
			}
		}

		void Insert( const T& val, Iterator& iter )
		{
			ASSERT( iter.m_pCurr != NULL );

			Node* pNode = (Node*)m_pCache->Alloc( sizeof(Node) );
			pNode->data = val;
			pNode->next = iter.m_pCurr;

			if( iter.m_pCurr == m_pHead )
			{
				m_pHead = pNode;
				iter.m_pCurr = pNode;
			}
			else
			{
				ASSERT( iter.m_pPrev != NULL );
				iter.m_pPrev->next = pNode;
			}

			iter.m_pPrev = pNode;
			m_size++;
		}

		Iterator Begin(){ return Iterator( m_pHead ); }

	private:
		
		Node* m_pHead;
		Node* m_pTail;
		size_t m_size;

		MemCache<DummyMutex>* m_pCache;
	};

}//namespace ECore