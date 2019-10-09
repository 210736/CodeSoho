//-----------------------------------------------------------------------------
// filename: event.h
// author: Sxg
// actor:
// data: 2009-12-21
// last:
// brief: ����ϵͳ�¼��ں˶����װ
//-----------------------------------------------------------------------------
#pragma once

namespace ECore{

//-----------------------------------------------------------------------------
// �ں��¼������װ
//-----------------------------------------------------------------------------
class ECORE_API Event
{
public:
	//-------------------------------------------------------------------------
	// ���������
	//-------------------------------------------------------------------------
	Event(LPCTSTR szName=NULL);
	~Event();

	//-------------------------------------------------------------------------
	// �������򿪣��ر�
	//-------------------------------------------------------------------------
	BOOL	Create(BOOL bManual, BOOL bInitialState);
	BOOL	Open();
	VOID	Close();

	//-------------------------------------------------------------------------
	// ���ã������ã��ȴ�
	//-------------------------------------------------------------------------
	INLINE VOID		Set()						{ ASSERT(m_hEvent != NULL); ::SetEvent(m_hEvent); }
	INLINE VOID		Reset()						{ ASSERT(m_hEvent != NULL); ::ResetEvent(m_hEvent); }
	INLINE DWORD	Wait(DWORD dwTime=INFINITE)	{ ASSERT(m_hEvent != NULL); return ::WaitForSingleObject(m_hEvent, dwTime); }
	INLINE BOOL		IsWaitTimeOut(DWORD dwTime)	{ ASSERT(m_hEvent != NULL); return WAIT_TIMEOUT == ::WaitForSingleObject(m_hEvent, dwTime); }

	//-------------------------------------------------------------------------
	// ��ȡ������
	//-------------------------------------------------------------------------
	INLINE DWORD	GetLastError()				{ return m_dwLastError; }

private:
	BOOL	m_bManual;				// �ֶ����ü���״̬
	BOOL	m_bInitialState;		// ��ʼ������״̬
	TCHAR	m_szName[MAX_PATH];		// �¼��ں˶�������

	HANDLE	m_hEvent;				// ���
	DWORD	m_dwLastError;			// ���һ�β���������
};

} // namespace ECore

