//-----------------------------------------------------------------------------
// filename: kernel_mutex.h
// author: Sxg
// actor:
// data: 2011-04-06
// last:
// brief: ����ϵͳ�¼��ں˶���(������)��װ
//-----------------------------------------------------------------------------
#pragma once

namespace ECore{

	//-----------------------------------------------------------------------------
	// �ں��¼������װ
	//-----------------------------------------------------------------------------
	class ECORE_API KernelMutex
	{
	public:
		//-------------------------------------------------------------------------
		// ���������
		//-------------------------------------------------------------------------
		KernelMutex(LPCTSTR szName=NULL);
		~KernelMutex();

		//-------------------------------------------------------------------------
		// �������򿪣��ر�
		//-------------------------------------------------------------------------
		BOOL	Create(BOOL bInitialState);
		BOOL	Open();
		VOID	Close();

		//-------------------------------------------------------------------------
		// lock/unlock
		//-------------------------------------------------------------------------
		INLINE VOID	Lock()					{ ASSERT(m_hMutex != NULL); ::WaitForSingleObject(m_hMutex, INFINITE); }
		INLINE VOID	Unlock()				{ ASSERT(m_hMutex != NULL); ::ReleaseMutex(m_hMutex); }
		INLINE BOOL	TryLock(DWORD dwTime)	{ ASSERT(m_hMutex != NULL); return ::WaitForSingleObject(m_hMutex, dwTime) != WAIT_TIMEOUT; }

		//-------------------------------------------------------------------------
		// ��ȡ������
		//-------------------------------------------------------------------------
		DWORD	GetLastError()				{ return m_dwLastError; }

	private:
		BOOL	m_bInitialState;		// ��ʼ������״̬ -- FALSE: ���Ա������̻߳�ȡ��TRUE: �ѱ��̻߳�ȡ
		TCHAR	m_szName[MAX_PATH];		// �¼��ں˶�������

		HANDLE	m_hMutex;				// ���
		DWORD	m_dwLastError;			// ���һ�β���������
	};

} // namespace ECore

