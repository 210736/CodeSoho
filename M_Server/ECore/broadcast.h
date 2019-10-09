//-----------------------------------------------------------------------------
// author Lyp
// date 2004-08-04
// last 2004-08-04
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
class Broadcast
{
public:
	BOOL	Init(INT nPort, LPCSTR szLocalIP=NULL);
	VOID	Destroy();

	//-------------------------------------------------------------------------
	// �㲥����
	//-------------------------------------------------------------------------
	VOID	Broad(LPBYTE pMsg, DWORD dwMsgSize);

	//-------------------------------------------------------------------------
	// ���չ㲥
	//-------------------------------------------------------------------------
	LPBYTE	RecvBroadcast(DWORD& dwMsgSize);

	//-------------------------------------------------------------------------
	// �黹��ȡ�Ĺ㲥����
	//-------------------------------------------------------------------------
	VOID	FreeRecved(LPBYTE pMsg);


	Broadcast();
	~Broadcast();


private:	
	MsgQueueTS*		m_pSendMsgQueue;				// ���Ͷ���
	MsgQueueTS*		m_pRecvMsgQueue;				// ���ն���

	ThreadMgr*		m_pThreadMgr;					// �̹߳�����

	DWORD			m_dwTDBroadcastID;				// �㲥�̵߳��̳߳�ID
	DWORD			m_dwTDRecvBroadcastID;			// ���չ㲥�̵߳��̳߳�ID
	BOOL VOLATILE 	m_bCloseBroadcast;				// �㲥�߳̽�����־
	BOOL VOLATILE 	m_bCloseRecvBroadcast;			// ���չ㲥�߳̽�����־

	INT VOLATILE 	m_nPort;						// �󶨶˿�
	TCHAR VOLATILE 	m_szLocalIP[LONG_STRING];		// �󶨵�ַ

	//-------------------------------------------------------------------------
	// ��ʼ�㲥/���չ㲥�߳�
	//-------------------------------------------------------------------------
	BOOL	StartBroadcast();
	BOOL	StartRecvBroadcast();

	//-------------------------------------------------------------------------
	// �����㲥/���չ㲥�߳�
	//-------------------------------------------------------------------------
	BOOL	CloseBroadcast();
	BOOL	CloseRecvBroadcast();

	//-------------------------------------------------------------------------
	// �̺߳���
	//-------------------------------------------------------------------------
	DWORD	TDBroadcast();							// �㲥�߳�
	DWORD	TDRecvBroadcast();						// ���չ㲥�߳�

	//-------------------------------------------------------------------------
	// �ر��̺߳���
	//-------------------------------------------------------------------------
	DWORD	CloseTDBroadcast()		{ InterlockedExchangeEx((LPLONG)&m_bCloseBroadcast, TRUE); return 0; }
	DWORD	CloseTDRecvBroadcast()	{ InterlockedExchangeEx((LPLONG)&m_bCloseRecvBroadcast, TRUE); return 0; }
};

} // namespace ECore {
