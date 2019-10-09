//-----------------------------------------------------------------------------
// File: local_lock.h
// Desc: 线程锁
// Auth: Sxg
// Date: 2010/01/15
// Desc: 该锁为局部变量，函数中使用时不用考虑unlock
//-----------------------------------------------------------------------------
#pragma once

namespace ECore{

template<typename TLock>
class LocalGuard
{
public:
	LocalGuard(TLock &Lock, BOOL bLock = TRUE): m_Lock(Lock), m_bLock(bLock)
	{
		if(m_bLock)
		{
			m_Lock.Lock();
		}
	}

	~LocalGuard()
	{
		if(m_bLock)
		{
			m_Lock.Unlock();
		}
	}

private:
	TLock	&m_Lock;
	BOOL	m_bLock;
};



} // namespace ECore{