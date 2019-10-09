//-----------------------------------------------------------------------------
// File: mutex.h
// Desc: �߳���
// Auth: Aslan
// Date: 2009/11/30
//
//
// Mutex����CriticalSection�ķ�װ
// FastMutex������ԭ�Ӳ���ʵ�ֵĿ�������
// AtomMutex����ԭ�����������߳����룬����ٶ�
// DummyMutex����û��������ҪΪ�˽����߳�ģ��Ͷ��߳�ģ��ͳһ��һ��ģ��ʱʹ��
//-----------------------------------------------------------------------------
#pragma once

namespace ECore{

//-----------------------------------------------------------------------------
// �߳�������CriticalSection�ķ�װ
//-----------------------------------------------------------------------------
class ECORE_API Mutex
{
public:
	//-------------------------------------------------------------------------
	// ���������
	//-------------------------------------------------------------------------
	Mutex() { if( FALSE == ::InitializeCriticalSectionAndSpinCount(&cs, 4000) ) { abort(); } }
	~Mutex() { ::DeleteCriticalSection(&cs); }

	//-------------------------------------------------------------------------
	// ��������������������
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
// ԭ�������ṩ���ķ����ٶȣ��������̳߳���
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
	LONG volatile	m_lLock;				// ԭ�Ӽ���
#pragma pack(pop)
};



//------------------------------------------------------------------------------
// �����߳���������ԭ�Ӳ�����ʵ�֣����������ں˶���
//------------------------------------------------------------------------------
class ECORE_API FastMutex
{
public:
	//--------------------------------------------------------------------------
	// ���캯��
	//--------------------------------------------------------------------------
	INLINE FastMutex() : m_lLock(0),m_dwRecursiveCount(0) {}
	INLINE ~FastMutex() {}

	//--------------------------------------------------------------------------
	// ����
	//--------------------------------------------------------------------------
	INLINE VOID Lock()
	{
		DWORD dwThreadID = GetCurrentThreadId(), dwOwner;

		// ͬһ���߳����������������ü�����ֱ�ӷ���
		if( dwThreadID == (DWORD)m_lLock )
		{
			++m_dwRecursiveCount;
			return;
		}

		// ѭ���ȴ�֪��Ŀǰ��m_lLockΪ0
		for(;;)
		{
			dwOwner = InterlockedCompareExchange((LPLONG)&m_lLock, dwThreadID, 0);
			if( 0 == dwOwner ) break;

			Sleep(0);
		}

		// ���������������һ�����ü���
		++m_dwRecursiveCount;
	}

	//-----------------------------------------------------------------------------
	// ��������
	//-----------------------------------------------------------------------------
	INLINE BOOL TryLock()
	{
		DWORD dwThreadID = GetCurrentThreadId();

		// ͬһ���߳����������������ü������������
		if( dwThreadID == (DWORD)m_lLock )
		{
			++m_dwRecursiveCount;
			return TRUE;
		}

		// ����һ������
		DWORD dwOwner = InterlockedCompareExchange((LPLONG)&m_lLock, dwThreadID, 0);
		if( 0 == dwOwner )
		{
			++m_dwRecursiveCount;
			return TRUE;
		}

		return FALSE;
	}

	//-------------------------------------------------------------------------------
	// �������
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
	LONG VOLATILE	m_lLock;				// ��ǰ�������߳�ID
#pragma pack(pop)
	DWORD			m_dwRecursiveCount;		// �������
};


//-------------------------------------------------------------------------------------
// ����
//-------------------------------------------------------------------------------------
class ECORE_API DummyMutex
{
public:
	INLINE VOID Lock()		{ }
	INLINE VOID Unlock()	{ }
	INLINE BOOL TryLock()	{ return TRUE; }
};

} // namespace ECore{

