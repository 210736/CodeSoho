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
	INLINE VOID		SetSelfGC(BOOL bGC) { m_bSelfGC = bGC; }	// 是否内部自动GC,默认为是
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
	// 尝试垃圾收集
	//---------------------------------------------------------------------------
	INLINE VOID		TryGC(DWORD dwExpectSize);

private:

	//---------------------------------------------------------------------------
	// 垃圾收集
	//---------------------------------------------------------------------------
	INLINE VOID		GC(DWORD dwExpectSize, DWORD dwUseTime);

	//---------------------------------------------------------------------------
	// 返回最匹配的大小
	//---------------------------------------------------------------------------
	INLINE INT		GetIndex(DWORD dwSize, DWORD& dwRealSize);


	struct tagNode	// 内存块头描述
	{
		tagNode*	pNext;
		tagNode*	pPrev;
		INT			nIndex;
		DWORD		dwSize;
		DWORD		dwUseTime;
		DWORD		dwFreeTime;		// 检测用，FreeTime应该等于dwUseTime,防止外部多次Free
		LPVOID		pMem[1];		// 实际内存空间
	};

	struct
	{
		SIZE_T		stNodeNum;
		SIZE_T		stAlloc;

		tagNode*	pFirst;
		tagNode*	pLast;
	} m_Pool[16];

	TObj<Log>		m_pLog;

	DWORD			m_dwMaxSize;				// 外部设定的最大允许空闲内存
	BOOL			m_bSelfGC;					// 是否自动GC

	DWORD VOLATILE 	m_dwCurrentSize;			// 内存池中空闲内存总数
	MutexType		m_Lock;						// 锁定
	BOOL VOLATILE	m_bTerminate;				// 结束标志,结束时为了加速不调用GC

	LONG VOLATILE	m_lTrueMalloc;				// 统计用，实际Malloc次数
	LONG VOLATILE	m_lGCTimes;					// 统计用，垃圾收集次数
	LONG VOLATILE	m_lLeak;					// 统计用，分配次数减去释放次数
};


//-----------------------------------------------------------------------------
// 构造和析构
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
// 分配
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
		if( m_Pool[nIndex].pFirst )	// 提前尝试
		{

			m_Lock.Lock();
			if( m_Pool[nIndex].pFirst )	// 池里有，就从池里分配
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
	pNode->dwFreeTime = 0;	// // 检测用，FreeTime应该等于dwUseTime,防止外部多次Free
	*(DWORD*)((LPBYTE)pNode->pMem + dwRealSize) = 0xDeadBeef;
	InterlockedIncrementEx(&m_lLeak);
	return pNode->pMem;	// 从实际内存中分配
}


//-----------------------------------------------------------------------------
// 释放
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

	if( m_bTerminate )	// 结束时，直接归还
	{
		free(pNode);
		return;
	}

	if( INVALID != pNode->nIndex )
	{
		if( m_bSelfGC && (pNode->dwSize + m_dwCurrentSize > m_dwMaxSize) )
			GC(pNode->dwSize*2, pNode->dwUseTime);	// 垃圾收集

		if( pNode->dwSize + m_dwCurrentSize <= m_dwMaxSize ) // 内存池可以容纳
		{
			if( *(DWORD*)((LPBYTE)pNode->pMem + pNode->dwSize) != 0xDeadBeef )
			{
				// 记录到LOG文件
				m_pLog->Write(_T("MemCache node corruption! %08x, %08x， %08x, %08x， %08x, %08x， %08x, %08x\r\n"),
					*(DWORD*)(pNode->pMem), *(((DWORD*)(pNode->pMem))+1),
					*(((DWORD*)(pNode->pMem))+2),*(((DWORD*)(pNode->pMem))+3),
					*(((DWORD*)(pNode->pMem))+4),*(((DWORD*)(pNode->pMem))+5),
					*(((DWORD*)(pNode->pMem))+6),*(((DWORD*)(pNode->pMem))+7));
				m_pLog->Flush();

				// 打印pNode前几个字节
				ERR(_T("MemCache node corruption! %08x, %08x， %08x, %08x， %08x, %08x， %08x, %08x"),
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
				DebugBreak(); // 重复释放
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
			++pNode->dwFreeTime;	// 检测用，FreeTime应该等于dwUseTime,防止外部多次Free

			// ------------------------------------
			m_Lock.Unlock();	
			return;
		}
	}

	free(pNode);
}


//-----------------------------------------------------------------------------
// 再分配
//-----------------------------------------------------------------------------
template<typename MutexType>
LPVOID MemCache<MutexType>::ReAlloc(LPVOID pMem, DWORD dwNewBytes)
{
#ifdef NO_MEM_CACHE
	return realloc(pMem, dwNewBytes);
#endif

	if( pMem && dwNewBytes )
	{
		// 分配新内存
		LPVOID pNew = Alloc(dwNewBytes);
		if( !pNew )
			return NULL;

		// 取得原大小并拷贝
		tagNode* pNode = (tagNode*)(((LPBYTE)pMem) - sizeof(tagNode) + sizeof(LPVOID));
		memcpy( pNew, pMem, EMin(pNode->dwSize, dwNewBytes));

		// 释放原内存
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
// 分配
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

		if( m_Pool[nIndex].pFirst )	// 池里有，就从池里分配
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
	pNode->dwFreeTime = 0;	// // 检测用，FreeTime应该等于dwUseTime,防止外部多次Free
	*(DWORD*)((LPBYTE)pNode->pMem + dwRealSize) = 0xDeadBeef;
	InterlockedIncrementEx(&m_lLeak);
	return pNode->pMem;	// 从实际内存中分配
}


//-----------------------------------------------------------------------------
// 释放
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

	if( m_bTerminate )	// 结束时，直接归还
	{
		free(pNode);
		InterlockedDecrementEx(&m_lLeak);
		return TRUE;
	}

	if( INVALID != pNode->nIndex )
	{
		if( m_bSelfGC && (pNode->dwSize + m_dwCurrentSize > m_dwMaxSize) )
			GC(pNode->dwSize*2, pNode->dwUseTime);	// 垃圾收集

		if( pNode->dwSize + m_dwCurrentSize <= m_dwMaxSize ) // 内存池可以容纳
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
				DebugBreak(); // 重复释放
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
			++pNode->dwFreeTime;	// 检测用，FreeTime应该等于dwUseTime,防止外部多次Free

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
// 垃圾收集
//-----------------------------------------------------------------------------
template<typename MutexType>
VOID MemCache<MutexType>::GC(DWORD dwExpectSize, DWORD dwUseTime)
{
	STATIC CONST DWORD MAX_FREE = 32;
	tagNode* free_array[MAX_FREE];
	DWORD dwFreeTime = 0;

	if( dwExpectSize > m_dwMaxSize / 64 )
		dwExpectSize = m_dwMaxSize / 64;	// 一次不要释放太多

	InterlockedIncrementEx(&m_lGCTimes);
	DWORD dwFreeSize = 0;

	m_Lock.Lock();
	for(INT n = 15; n >= 0; --n)	// 从最大的开始回收
	{
		if( !m_Pool[n].pFirst )
			continue;

		tagNode* pNode = m_Pool[n].pLast; // 从最后开始释放，因为后面的Node使用次数少
		while( pNode )
		{
			tagNode* pTempNode = pNode;
			pNode = pNode->pPrev;
			
			if( pTempNode->dwUseTime >= dwUseTime )	
				break;	// 再往前已经没有适合释放的节点了，跳出看看其他型号
			
			// 释放此节点
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

			// 记录到数组，等会退出临界区再释放
			free_array[dwFreeTime++] = pTempNode;

			if( dwFreeSize >= dwExpectSize || dwFreeTime >= MAX_FREE )	// 每次GC不要调用太多Free
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
// 匹配大小
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
// 垃圾收集
//-----------------------------------------------------------------------------
template<typename MutexType>
VOID MemCache<MutexType>::TryGC(DWORD dwExpectSize)
{
	STATIC CONST DWORD MAX_FREE = 32;
	tagNode* free_array[MAX_FREE];
	DWORD dwFreeTime = 0;

	if( dwExpectSize > m_dwMaxSize / 64 )
		dwExpectSize = m_dwMaxSize / 64;	// 一次不要释放太多
	
	DWORD dwFreeSize = 0;

	if( !m_Lock.TryLock() )
		return;

	InterlockedIncrementEx(&m_lGCTimes);
	for(INT n = 15; n >= 0; --n)	// 从最大的开始回收
	{
		if( !m_Pool[n].pFirst )
			continue;

		tagNode* pNode = m_Pool[n].pLast; // 从最后开始释放，因为后面的Node使用次数少
		while( pNode )
		{
			tagNode* pTempNode = pNode;
			pNode = pNode->pPrev;

			// 释放此节点
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

			// 记录到数组，等会退出临界区再释放
			free_array[dwFreeTime++] = pTempNode;

			if( dwFreeSize >= dwExpectSize || dwFreeTime >= MAX_FREE )	// 每次GC不要调用太多Free
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