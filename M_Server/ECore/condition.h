//-----------------------------------------------------------------------------
// File: condition.h
// Desc: ��ͬ���¼�ʵ����
// Auth: Lyp
// Date: 2003/11/30
//-----------------------------------------------------------------------------
#pragma once

namespace ECore{

//-----------------------------------------------------------------------------
// ��ͬ���¼�ʵ����
//-----------------------------------------------------------------------------
class Condition
{
public:
	Condition(BOOL bManual, LPCTSTR szName=NULL)	{ m_hEvent = ::CreateEvent(NULL, bManual, FALSE, szName); ASSERT(m_hEvent != NULL); }
	~Condition()									{ ::CloseHandle(m_hEvent); }

	//--------------------------------------------------------------------------
	// ���ã������ã��ȴ�
	//--------------------------------------------------------------------------
	VOID	Set()				{ ::SetEvent(m_hEvent); }
	VOID	ReSet()				{ ::ResetEvent(m_hEvent); }
	VOID	Wait()				{ ::WaitForSingleObject(m_hEvent, INFINITE); }

private:
	HANDLE	m_hEvent;
};
}
