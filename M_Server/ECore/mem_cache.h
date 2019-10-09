//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2009-1-8	
// Last: 2010-6-4
//-----------------------------------------------------------------------------
#pragma once


#ifdef MEM_TRACE
#	define NO_MEM_CACHE
#endif

//#define NO_MEM_CACHE

#ifdef NO_MEM_CACHE
#	define MCALLOC(dw)		malloc(dw)
#	define MCREALLOC(p,dw)	realloc(p,dw)
#	define MCFREE(p)		free(p)
#else
#	define MCALLOC(dw)		g_pMemCache->Alloc(dw)
#	define MCREALLOC(p,dw)	g_pMemCache->ReAlloc(p,dw)
#	define MCFREE(p)		g_pMemCache->Free(p)
#endif

#ifndef SAFE_MCFREE
#	define SAFE_MCFREE(p)	{ if(p) { MCFREE(p); (p) = NULL; } }
#endif

namespace ECore {
template<typename MutexType = DummyMutex>
class MemCache
{
public:
	INLINE VOID		SetMaxSize(DWORD dwSize) { m_dwMaxSize = dwSize; }
	INLINE VOID		SetSelfGC(BOOL bGC) { m_bSelfGC = bGC; }	// �Ƿ��ڲ��Զ�GC,Ĭ��Ϊ��
	INLINE VOID		SetTerminate() { m_bTerminate = TRUE; }


	INLINE LPVOID	Alloc(DWORD dwBytes);
	INLINE VOID		Free(LPVOID pMem);
	INLINE LPVOID	ReAlloc(LPVOID pMem, DWORD dwNewBytes);
	INLINE LPVOID	TryAlloc(DWORD dwBytes);
	INLINE BOOL		TryFree(LPVOID pMem);

	INLINE DWORD	GetMaxSize()	{ return m_dwMaxSize; }
	INLINE DWORD	GetSize()		{ return m_dwCurrentSize; }
	INLINE LONG		GetMalloc()		{ return m_lTrueMalloc; }
	INLINE LONG		GetLeak()		{ return m_lLeak; }
	INLINE LONG		GetGC()			{ return m_lGCTimes; }

	
	MemCache(DWORD dwMaxSize=16*1024*1024);
	~MemCache();


	//---------------------------------------------------------------------------
	// ���������ռ�
	//---------------------------------------------------------------------------
	INLINE VOID		TryGC(DWORD dwExpectSize);

private:

	//---------------------------------------------------------------------------
	// �����ռ�
	//---------------------------------------------------------------------------
	INLINE VOID		GC(DWORD dwExpectSize, DWORD dwUseTime);

	//---------------------------------------------------------------------------
	// ������ƥ��Ĵ�С
	//---------------------------------------------------------------------------
	INLINE INT		GetIndex(DWORD dwSize, DWORD& dwRealSize);


	struct tagNode	// �ڴ��ͷ����
	{
		tagNode*	pNext;
		tagNode*	pPrev;
		INT			nIndex;
		DWORD		dwSize;
		DWORD		dwUseTime;
		DWORD		dwFreeTime;		// ����ã�FreeTimeӦ�õ���dwUseTime,��ֹ�ⲿ���Free
		LPVOID		pMem[1];		// ʵ���ڴ�ռ�
	};

	struct
	{
		SIZE_T		stNodeNum;
		SIZE_T		stAlloc;

		tagNode*	pFirst;
		tagNode*	pLast;
	} m_Pool[16];

	TObj<Log>		m_pLog;

	DWORD			m_dwMaxSize;				// �ⲿ�趨�������������ڴ�
	BOOL			m_bSelfGC;					// �Ƿ��Զ�GC

	DWORD VOLATILE 	m_dwCurrentSize;			// �ڴ���п����ڴ�����
	MutexType		m_Lock;						// ����
	BOOL VOLATILE	m_bTerminate;				// ������־,����ʱΪ�˼��ٲ�����GC

	LONG VOLATILE	m_lTrueMalloc;				// ͳ���ã�ʵ��Malloc����
	LONG VOLATILE	m_lGCTimes;					// ͳ���ã������ռ�����
	LONG VOLATILE	m_lLeak;					// ͳ���ã����������ȥ�ͷŴ���
};


//-----------------------------------------------------------------------------
// ���������
//-----------------------------------------------------------------------------
template<typename MutexType>
MemCache<MutexType>::MemCache(DWORD dwMaxSize):
m_dwMaxSize(dwMaxSize), 
m_bSelfGC(TRUE),
m_dwCurrentSize(0), 
m_bTerminate(0),
m_lTrueMalloc(0),
m_lGCTimes(0),
m_lLeak(0)
{
	ZeroMemory(m_Pool, sizeof(m_Pool));
}

template<typename MutexType>
MemCache<MutexType>::~MemCache()
{
	for( int n=0; n<16; n++ )
	{
		while( m_Pool[n].pFirst )
		{
			tagNode* pNode = m_Pool[n].pFirst;
			m_Pool[n].pFirst = m_Pool[n].pFirst->pNext;
			free(pNode);
		}
	}
}


//-----------------------------------------------------------------------------
// ����
//-----------------------------------------------------------------------------
template<typename MutexType>
LPVOID MemCache<MutexType>::Alloc(DWORD dwBytes)
{

#ifdef NO_MEM_CACHE
	InterlockedIncrementEx(&m_lLeak);
	return malloc(dwBytes);
#endif

	DWORD dwRealSize = 0;

	INT nIndex = GetIndex(dwBytes, dwRealSize);
	if( INVALID != nIndex )
	{
		if( m_Pool[nIndex].pFirst )	// ��ǰ����
		{

			m_Lock.Lock();
			if( m_Pool[nIndex].pFirst )	// �����У��ʹӳ������
			{
				tagNode* pNode = m_Pool[nIndex].pFirst;
				m_Pool[nIndex].pFirst = m_Pool[nIndex].pFirst->pNext;
				if( m_Pool[nIndex].pFirst )
					m_Pool[nIndex].pFirst->pPrev = NULL;
				else
					m_Pool[nIndex].pLast = NULL;
				m_dwCurrentSize -= dwRealSize;
				++pNode->dwUseTime;
				--m_Pool[nIndex].stNodeNum;
				++m_Pool[nIndex].stAlloc;
				m_Lock.Unlock();
				InterlockedIncrementEx(&m_lLeak);
				return pNode->pMem;	
			}
			m_Lock.Unlock();
		}
	}

	
	tagNode* pNode = (tagNode*)malloc(dwRealSize + sizeof(tagNode));
	if( pNode )
	{
		InterlockedIncrementEx(&m_lTrueMalloc);
	}
	else
	{
		return NULL;
	}

	pNode->nIndex = nIndex;
	pNode->dwSize = dwRealSize;
	pNode->dwUseTime = 0;
	pNode->dwFreeTime = 0;	// // ����ã�FreeTimeӦ�õ���dwUseTime,��ֹ�ⲿ���Free
	*(DWORD*)((LPBYTE)pNode->pMem + dwRealSize) = 0xDeadBeef;
	InterlockedIncrementEx(&m_lLeak);
	return pNode->pMem;	// ��ʵ���ڴ��з���
}


//-----------------------------------------------------------------------------
// �ͷ�
//-----------------------------------------------------------------------------
template<typename MutexType>
VOID MemCache<MutexType>::Free(LPVOID pMem)
{
	if( !pMem )
		return;

#ifdef NO_MEM_CACHE
	InterlockedDecrementEx(&m_lLeak);
	free(pMem);
	return;
#endif

	InterlockedDecrementEx(&m_lLeak);

	tagNode* pNode = (tagNode*)(((LPBYTE)pMem) - sizeof(tagNode) + sizeof(LPVOID));

	if( m_bTerminate )	// ����ʱ��ֱ�ӹ黹
	{
		free(pNode);
		return;
	}

	if( INVALID != pNode->nIndex )
	{
		if( m_bSelfGC && (pNode->dwSize + m_dwCurrentSize > m_dwMaxSize) )
			GC(pNode->dwSize*2, pNode->dwUseTime);	// �����ռ�

		if( pNode->dwSize + m_dwCurrentSize <= m_dwMaxSize ) // �ڴ�ؿ�������
		{
			if( *(DWORD*)((LPBYTE)pNode->pMem + pNode->dwSize) != 0xDeadBeef )
			{
				// ��¼��LOG�ļ�
				m_pLog->Write(_T("MemCache node corruption! %08x, %08x�� %08x, %08x�� %08x, %08x�� %08x, %08x\r\n"),
					*(DWORD*)(pNode->pMem), *(((DWORD*)(pNode->pMem))+1),
					*(((DWORD*)(pNode->pMem))+2),*(((DWORD*)(pNode->pMem))+3),
					*(((DWORD*)(pNode->pMem))+4),*(((DWORD*)(pNode->pMem))+5),
					*(((DWORD*)(pNode->pMem))+6),*(((DWORD*)(pNode->pMem))+7));
				m_pLog->Flush();

				// ��ӡpNodeǰ�����ֽ�
				ERR(_T("MemCache node corruption! %08x, %08x�� %08x, %08x�� %08x, %08x�� %08x, %08x"),
					*(DWORD*)(pNode->pMem), *(((DWORD*)(pNode->pMem))+1),
					*(((DWORD*)(pNode->pMem))+2),*(((DWORD*)(pNode->pMem))+3),
					*(((DWORD*)(pNode->pMem))+4),*(((DWORD*)(pNode->pMem))+5),
					*(((DWORD*)(pNode->pMem))+6),*(((DWORD*)(pNode->pMem))+7)
					);

				DebugBreak();
			}

			m_Lock.Lock(); 
			// ------------------------------------

			pNode->pPrev = NULL;
			pNode->pNext = m_Pool[pNode->nIndex].pFirst;
			if(pNode->pNext == pNode)
			{
				ERR(_T("MemCache Free more than once!"));
				DebugBreak(); // �ظ��ͷ�
			}

			if( m_Pool[pNode->nIndex].pFirst )
				m_Pool[pNode->nIndex].pFirst->pPrev = pNode;
			else
				m_Pool[pNode->nIndex].pLast = pNode;

			m_Pool[pNode->nIndex].pFirst = pNode;
			++m_Pool[pNode->nIndex].stNodeNum;
			m_dwCurrentSize += pNode->dwSize;

			if(pNode->dwFreeTime != pNode->dwUseTime)
			{
				ERR(_T("MemCache Free more than once!"));
				DebugBreak();
			}
			++pNode->dwFreeTime;	// ����ã�FreeTimeӦ�õ���dwUseTime,��ֹ�ⲿ���Free

			// ------------------------------------
			m_Lock.Unlock();	
			return;
		}
	}

	free(pNode);
}


//-----------------------------------------------------------------------------
// �ٷ���
//-----------------------------------------------------------------------------
template<typename MutexType>
LPVOID MemCache<MutexType>::ReAlloc(LPVOID pMem, DWORD dwNewBytes)
{
#ifdef NO_MEM_CACHE
	return realloc(pMem, dwNewBytes);
#endif

	if( pMem && dwNewBytes )
	{
		// �������ڴ�
		LPVOID pNew = Alloc(dwNewBytes);
		if( !pNew )
			return NULL;

		// ȡ��ԭ��С������
		tagNode* pNode = (tagNode*)(((LPBYTE)pMem) - sizeof(tagNode) + sizeof(LPVOID));
		memcpy( pNew, pMem, EMin(pNode->dwSize, dwNewBytes));

		// �ͷ�ԭ�ڴ�
		Free(pMem);
		return pNew;
	}
	else if( pMem )
	{
		Free(pMem);
		return NULL;
	}
	else if( dwNewBytes )
	{
		return Alloc(dwNewBytes);
	}
	else
	{
		return NULL;
	}
}


//-----------------------------------------------------------------------------
// ����
//-----------------------------------------------------------------------------
template<typename MutexType>
LPVOID MemCache<MutexType>::TryAlloc(DWORD dwBytes)
{
#ifdef NO_MEM_CACHE
	InterlockedIncrementEx(&m_lLeak);
	return malloc(dwBytes);
#endif

	DWORD dwRealSize = 0;
	INT nIndex = GetIndex(dwBytes, dwRealSize);
	if( INVALID != nIndex )
	{
		if( !m_Lock.TryLock() )
			return NULL;

		if( m_Pool[nIndex].pFirst )	// �����У��ʹӳ������
		{
			tagNode* pNode = m_Pool[nIndex].pFirst;
			m_Pool[nIndex].pFirst = m_Pool[nIndex].pFirst->pNext;
			if( m_Pool[nIndex].pFirst )
				m_Pool[nIndex].pFirst->pPrev = NULL;
			else
				m_Pool[nIndex].pLast = NULL;
			m_dwCurrentSize -= dwRealSize;
			++pNode->dwUseTime;
			--m_Pool[nIndex].stNodeNum;
			++m_Pool[nIndex].stAlloc;
			m_Lock.Unlock();
			InterlockedIncrementEx(&m_lLeak);
			return pNode->pMem;	
		}
		m_Lock.Unlock();
	}

	tagNode* pNode = (tagNode*)malloc(dwRealSize + sizeof(tagNode));
	if( pNode )
	{
		InterlockedIncrementEx(&m_lTrueMalloc);
	}
	else
	{
		return NULL;
	}

	pNode->nIndex = nIndex;
	pNode->dwSize = dwRealSize;
	pNode->dwUseTime = 0;
	pNode->dwFreeTime = 0;	// // ����ã�FreeTimeӦ�õ���dwUseTime,��ֹ�ⲿ���Free
	*(DWORD*)((LPBYTE)pNode->pMem + dwRealSize) = 0xDeadBeef;
	InterlockedIncrementEx(&m_lLeak);
	return pNode->pMem;	// ��ʵ���ڴ��з���
}


//-----------------------------------------------------------------------------
// �ͷ�
//-----------------------------------------------------------------------------
template<typename MutexType>
BOOL MemCache<MutexType>::TryFree(LPVOID pMem)
{
	if( !pMem )
		return TRUE;

#ifdef NO_MEM_CACHE
	InterlockedDecrementEx(&m_lLeak);
	free(pMem);
	return TRUE;
#endif

	tagNode* pNode = (tagNode*)(((LPBYTE)pMem) - sizeof(tagNode) + sizeof(LPVOID));

	if( m_bTerminate )	// ����ʱ��ֱ�ӹ黹
	{
		free(pNode);
		InterlockedDecrementEx(&m_lLeak);
		return TRUE;
	}

	if( INVALID != pNode->nIndex )
	{
		if( m_bSelfGC && (pNode->dwSize + m_dwCurrentSize > m_dwMaxSize) )
			GC(pNode->dwSize*2, pNode->dwUseTime);	// �����ռ�

		if( pNode->dwSize + m_dwCurrentSize <= m_dwMaxSize ) // �ڴ�ؿ�������
		{
			if( *(DWORD*)((LPBYTE)pNode->pMem + pNode->dwSize) != 0xDeadBeef )
			{
				ERR(_T("MemCache node corruption!"));
				DebugBreak();
			}

			if( !m_Lock.TryLock() )
				return FALSE;

			pNode->pPrev = NULL;
			pNode->pNext = m_Pool[pNode->nIndex].pFirst;

			if(pNode->pNext == pNode)
			{
				ERR(_T("MemCache Free more than once!"));
				DebugBreak(); // �ظ��ͷ�
			}

			if( m_Pool[pNode->nIndex].pFirst )
				m_Pool[pNode->nIndex].pFirst->pPrev = pNode;
			else
				m_Pool[pNode->nIndex].pLast = pNode;

			m_Pool[pNode->nIndex].pFirst = pNode;
			m_dwCurrentSize += pNode->dwSize;
			++m_Pool[pNode->nIndex].stNodeNum;

			if(pNode->dwFreeTime != pNode->dwUseTime)
			{
				ERR(_T("MemCache Free more than once!"));
				DebugBreak();
			}
			++pNode->dwFreeTime;	// ����ã�FreeTimeӦ�õ���dwUseTime,��ֹ�ⲿ���Free

			m_Lock.Unlock();
			InterlockedDecrementEx(&m_lLeak);
			return TRUE;
		}
	}
	
	free(pNode);
	InterlockedDecrementEx(&m_lLeak);
	return TRUE;
}



//-----------------------------------------------------------------------------
// �����ռ�
//-----------------------------------------------------------------------------
template<typename MutexType>
VOID MemCache<MutexType>::GC(DWORD dwExpectSize, DWORD dwUseTime)
{
	STATIC CONST DWORD MAX_FREE = 32;
	tagNode* free_array[MAX_FREE];
	DWORD dwFreeTime = 0;

	if( dwExpectSize > m_dwMaxSize / 64 )
		dwExpectSize = m_dwMaxSize / 64;	// һ�β�Ҫ�ͷ�̫��

	InterlockedIncrementEx(&m_lGCTimes);
	DWORD dwFreeSize = 0;

	m_Lock.Lock();
	for(INT n = 15; n >= 0; --n)	// �����Ŀ�ʼ����
	{
		if( !m_Pool[n].pFirst )
			continue;

		tagNode* pNode = m_Pool[n].pLast; // �����ʼ�ͷţ���Ϊ�����Nodeʹ�ô�����
		while( pNode )
		{
			tagNode* pTempNode = pNode;
			pNode = pNode->pPrev;
			
			if( pTempNode->dwUseTime >= dwUseTime )	
				break;	// ����ǰ�Ѿ�û���ʺ��ͷŵĽڵ��ˣ��������������ͺ�
			
			// �ͷŴ˽ڵ�
			if( pNode )
				pNode->pNext = pTempNode->pNext;

			if( pTempNode->pNext )
				pTempNode->pNext->pPrev = pNode;

			if( m_Pool[n].pLast == pTempNode )
				m_Pool[n].pLast = pNode;

			if( m_Pool[n].pFirst == pTempNode )
				m_Pool[n].pFirst = pTempNode->pNext;

			m_dwCurrentSize -= pTempNode->dwSize;
			--m_Pool[n].stNodeNum;
			dwFreeSize += pTempNode->dwSize;

			// ��¼�����飬�Ȼ��˳��ٽ������ͷ�
			free_array[dwFreeTime++] = pTempNode;

			if( dwFreeSize >= dwExpectSize || dwFreeTime >= MAX_FREE )	// ÿ��GC��Ҫ����̫��Free
			{
				goto __out_gc;
			}
		}
	}

__out_gc:
	m_Lock.Unlock();

	for(DWORD n=0; n<dwFreeTime; ++n)
		free(free_array[n]);
}


//-----------------------------------------------------------------------------
// ƥ���С
//-----------------------------------------------------------------------------
template<typename MutexType>
INT MemCache<MutexType>::GetIndex(DWORD dwSize, DWORD& dwRealSize)
{
	if( dwSize<=32 )		{ dwRealSize = 32;			return 0; }
	if( dwSize<=64 )		{ dwRealSize = 64;			return 1; }
	if( dwSize<=128 )		{ dwRealSize = 128;			return 2; }
	if( dwSize<=256 )		{ dwRealSize = 256;			return 3; }
	if( dwSize<=512 )		{ dwRealSize = 512;			return 4; }
	if( dwSize<=1024 )		{ dwRealSize = 1024;		return 5; }
	if( dwSize<=2*1024 )	{ dwRealSize = 2*1024;		return 6; }
	if( dwSize<=4*1024 )	{ dwRealSize = 4*1024;		return 7; }
	if( dwSize<=8*1024 )	{ dwRealSize = 8*1024;		return 8; }
	if( dwSize<=16*1024 )	{ dwRealSize = 16*1024;		return 9; }
	if( dwSize<=32*1024 )	{ dwRealSize = 32*1024;		return 10; }
	if( dwSize<=64*1024 )	{ dwRealSize = 64*1024;		return 11; }
	if( dwSize<=128*1024 )	{ dwRealSize = 128*1024;	return 12; }
	if( dwSize<=256*1024 )	{ dwRealSize = 256*1024;	return 13; }
	if( dwSize<=512*1024 )	{ dwRealSize = 512*1024;	return 14; }
	if( dwSize<=1024*1024 )	{ dwRealSize = 1024*1024;	return 15; }
	dwRealSize = dwSize;
	return INVALID;
}



//-----------------------------------------------------------------------------
// �����ռ�
//-----------------------------------------------------------------------------
template<typename MutexType>
VOID MemCache<MutexType>::TryGC(DWORD dwExpectSize)
{
	STATIC CONST DWORD MAX_FREE = 32;
	tagNode* free_array[MAX_FREE];
	DWORD dwFreeTime = 0;

	if( dwExpectSize > m_dwMaxSize / 64 )
		dwExpectSize = m_dwMaxSize / 64;	// һ�β�Ҫ�ͷ�̫��
	
	DWORD dwFreeSize = 0;

	if( !m_Lock.TryLock() )
		return;

	InterlockedIncrementEx(&m_lGCTimes);
	for(INT n = 15; n >= 0; --n)	// �����Ŀ�ʼ����
	{
		if( !m_Pool[n].pFirst )
			continue;

		tagNode* pNode = m_Pool[n].pLast; // �����ʼ�ͷţ���Ϊ�����Nodeʹ�ô�����
		while( pNode )
		{
			tagNode* pTempNode = pNode;
			pNode = pNode->pPrev;

			// �ͷŴ˽ڵ�
			if( pNode )
				pNode->pNext = pTempNode->pNext;

			if( pTempNode->pNext )
				pTempNode->pNext->pPrev = pNode;

			if( m_Pool[n].pLast == pTempNode )
				m_Pool[n].pLast = pNode;

			if( m_Pool[n].pFirst == pTempNode )
				m_Pool[n].pFirst = pTempNode->pNext;

			m_dwCurrentSize -= pTempNode->dwSize;
			--m_Pool[n].stNodeNum;
			dwFreeSize += pTempNode->dwSize;

			// ��¼�����飬�Ȼ��˳��ٽ������ͷ�
			free_array[dwFreeTime++] = pTempNode;

			if( dwFreeSize >= dwExpectSize || dwFreeTime >= MAX_FREE )	// ÿ��GC��Ҫ����̫��Free
			{
				goto __out_gc;
			}
		}
	}

__out_gc:
	m_Lock.Unlock();

	for(DWORD n=0; n<dwFreeTime; ++n)
		free(free_array[n]);
}









} // namespace ECore {