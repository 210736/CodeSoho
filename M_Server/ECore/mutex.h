//-----------------------------------------------------------------------------
// File: mutex.h
// Desc: 线程锁
// Auth: Aslan
// Date: 2009/11/30
//
//
// Mutex——CriticalSection的封装
// FastMutex——用原子操作实现的可重入锁
// AtomMutex——原子锁，不可线程重入，最快速度
// DummyMutex——没有锁，主要为了将单线程模块和多线程模块统一成一个模块时使用
//-----------------------------------------------------------------------------
#pragma once

namespace ECore{

//-----------------------------------------------------------------------------
// 线程锁——CriticalSection的封装
//-----------------------------------------------------------------------------
class ECORE_API Mutex
{
public:
	//-------------------------------------------------------------------------
	// 构造和析构
	//-------------------------------------------------------------------------
	Mutex() { if( FALSE == ::InitializeCriticalSectionAndSpinCount(&cs, 4000) ) { abort(); } }
	~Mutex() { ::DeleteCriticalSection(&cs); }

	//-------------------------------------------------------------------------
	// 锁定，解锁，尝试锁定
	//-------------------------------------------------------------------------
	INLINE VOID	Lock()		{ ::EnterCriticalSection(&cs); }
	INLINE VOID	Unlock()	{ ::LeaveCriticalSection(&cs); }
	INLINE BOOL	TryLock()	{ return ::TryEnterCriticalSection(&cs); }

private:
	Mutex(const Mutex& mutex);
	const Mutex& operator=(const Mutex& mutex);

protected:
	CRITICAL_SECTION	cs;
};


//-------------------------------------------------------------------------------------
// 原子锁，提供最大的访问速度，但不可线程冲入
//-------------------------------------------------------------------------------------
class ECORE_API AtomMutex
{
public:
	INLINE AtomMutex() : m_lLock(0) {}

	INLINE VOID Lock()		{ while( ::InterlockedCompareExchange((LPLONG)&m_lLock, 1, 0) != 0 ) ::Sleep(0); }
	INLINE VOID Unlock()	{ InterlockedExchangeEx((&m_lLock), 0); }
	INLINE BOOL TryLock()	{ return ::InterlockedCompareExchange((LPLONG)&m_lLock, 1, 0) == 0; }

private:
#pragma pack(push, 8)
	LONG volatile	m_lLock;				// 原子计数
#pragma pack(pop)
};



//------------------------------------------------------------------------------
// 快速线程锁——用原子操作来实现，不需生成内核对象
//------------------------------------------------------------------------------
class ECORE_API FastMutex
{
public:
	//--------------------------------------------------------------------------
	// 构造函数
	//--------------------------------------------------------------------------
	INLINE FastMutex() : m_lLock(0),m_dwRecursiveCount(0) {}
	INLINE ~FastMutex() {}

	//--------------------------------------------------------------------------
	// 锁定
	//--------------------------------------------------------------------------
	INLINE VOID Lock()
	{
		DWORD dwThreadID = GetCurrentThreadId(), dwOwner;

		// 同一个线程锁定，则增加引用计数，直接返回
		if( dwThreadID == (DWORD)m_lLock )
		{
			++m_dwRecursiveCount;
			return;
		}

		// 循环等待知道目前的m_lLock为0
		for(;;)
		{
			dwOwner = InterlockedCompareExchange((LPLONG)&m_lLock, dwThreadID, 0);
			if( 0 == dwOwner ) break;

			Sleep(0);
		}

		// 获得了锁定，增加一次引用计数
		++m_dwRecursiveCount;
	}

	//-----------------------------------------------------------------------------
	// 尝试锁定
	//-----------------------------------------------------------------------------
	INLINE BOOL TryLock()
	{
		DWORD dwThreadID = GetCurrentThreadId();

		// 同一个线程锁定，则增加引用计数，获得锁定
		if( dwThreadID == (DWORD)m_lLock )
		{
			++m_dwRecursiveCount;
			return TRUE;
		}

		// 尝试一次锁定
		DWORD dwOwner = InterlockedCompareExchange((LPLONG)&m_lLock, dwThreadID, 0);
		if( 0 == dwOwner )
		{
			++m_dwRecursiveCount;
			return TRUE;
		}

		return FALSE;
	}

	//-------------------------------------------------------------------------------
	// 解除锁定
	//-------------------------------------------------------------------------------
	INLINE VOID Unlock()
	{
		if( 0 == m_lLock ) return;

		DWORD dwThreadID = GetCurrentThreadId();

		if( dwThreadID != (DWORD)m_lLock || 0 == m_dwRecursiveCount )
		{
			ASSERT(0);
			return;
		}

		if( (--m_dwRecursiveCount) == 0 )
			InterlockedExchangeEx(&m_lLock, 0);
	}

private:
	FastMutex(const FastMutex& mutex);
	const FastMutex& operator=(const FastMutex& mutex);

private:

#pragma pack(push,8)
	LONG VOLATILE	m_lLock;				// 当前锁定的线程ID
#pragma pack(pop)
	DWORD			m_dwRecursiveCount;		// 重入次数
};


//-------------------------------------------------------------------------------------
// 无锁
//-------------------------------------------------------------------------------------
class ECORE_API DummyMutex
{
public:
	INLINE VOID Lock()		{ }
	INLINE VOID Unlock()	{ }
	INLINE BOOL TryLock()	{ return TRUE; }
};

} // namespace ECore{

