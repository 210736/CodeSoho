//--------------------------------------------------------------------------------
// author Lyp
// date 2003-03-17
// last 2009-12-16
//--------------------------------------------------------------------------------
#pragma once

namespace ECore {
namespace Wan {
//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------
class ECORE_API Client
{
public:
	BOOL	Init();
	VOID	Destroy();

	//-----------------------------------------------------------------------------
	// �����������
	//-----------------------------------------------------------------------------
	BOOL	TryConnect(LPCSTR szIP, INT nPort);	// ��������
	BOOL	IsTryingConnect();					// �Ƿ�����������
	VOID	Disconnect(); // �Ͽ�����
	INLINE BOOL	IsConnected() { return m_bConnected; }

	INLINE BOOL		Send(LPVOID pMsg, DWORD dwMsgSize);
	INLINE LPBYTE	Recv(DWORD& dwMsgSize);
	INLINE VOID		FreeRecved(LPBYTE pMsg);
	
	INLINE HANDLE GetRecvEvent() { return m_RecvMsgQueue.GetEvent(); } // �а��յ�����������
	//------------------------------------------------------------------------------
	// ��Ϣͳ��
	//------------------------------------------------------------------------------
	INLINE INT		GetUnsend(); // δ���͵İ������������Դ����ж����縺��
	INLINE INT		GetUnreceive();	// δ���ϲ��յİ�����
	INLINE DWORD	GetSend() { return m_dwSendPackNum; }	// �ܷ�����
	INLINE DWORD	GetRecved() { return m_dwRecvPackNum; }	// ���հ���
	INLINE DWORD	GetSendBPS() { return m_dwSendBPS; }	// ������
	INLINE DWORD	GetRecvBPS() { return m_dwRecvBPS; }	// ������
	INLINE LPCSTR	GetServerIP() { return m_strIP.c_str(); }	// �õ�������IP


	Client();
	~Client();

private:
	TObj<Filter>		m_pFilter;
	ThreadMgr*			m_pThreadMgr;

	MsgQueueTS			m_SendMsgQueue;	// ���Ͷ���
	MsgQueueTS			m_RecvMsgQueue;	// ���ն���
	
	//-------------------------------------------------------------------------------
	// ����
	//-------------------------------------------------------------------------------
	std::string			m_strIP;			// ������IP��ַ
	INT					m_nPort;			// �������˿�
	SOCKET				m_Sock;				// �׽���
	BOOL VOLATILE		m_bConnected;		// �Ƿ�������

	//--------------------------------------------------------------------------------
	// �߳����
	//--------------------------------------------------------------------------------
	DWORD VOLATILE 		m_dwTDConnectID;	// �����̵߳��̳߳�ID
	BOOL VOLATILE 		m_bCloseConnect;	// ���������߳�
	BOOL VOLATILE 		m_bCloseSend;		// ���������߳�
	BOOL VOLATILE 		m_bCloseReceive;	// ���������߳�

	//--------------------------------------------------------------------------------
	// ͳ��
	//--------------------------------------------------------------------------------
	DWORD VOLATILE 		m_dwLastStatInTime;	// �ϴ�Inͳ��ʱ��
	DWORD VOLATILE 		m_dwLastStatOutTime;// �ϴ�Outͳ��ʱ��
	DWORD VOLATILE 		m_dwSendBPS;
	DWORD VOLATILE 		m_dwRecvBPS;
	DWORD VOLATILE 		m_dwByteOutCounter;
	DWORD VOLATILE 		m_dwByteInCounter;
	DWORD VOLATILE 		m_dwSendPackNum;	// �ܷ�����(��������ʧ�ܺͻ�δ����ȥ��)
	DWORD VOLATILE 		m_dwRecvPackNum;	// ���հ���(������δ�����)


	//-------------------------------------------------------------------------------
	//
	//-------------------------------------------------------------------------------
	VOID	Clear();
	BOOL	CloseSocket(SOCKET& socket, INT nRetryTimes);
	DWORD	TDConnect();
	DWORD	TDSend();
	DWORD	TDReceive();
	INLINE DWORD	CloseTDConnect()	{ InterlockedExchangeEx((LPLONG)&m_bCloseConnect,	TRUE); return 0; }
	INLINE DWORD	CloseTDSend()		{ InterlockedExchangeEx((LPLONG)&m_bCloseSend,		TRUE); return 0; }
	INLINE DWORD	CloseTDReceive()	{ InterlockedExchangeEx((LPLONG)&m_bCloseReceive,	TRUE); return 0; }
};


//-----------------------------------------------------------------------------
// ����
//-----------------------------------------------------------------------------
INLINE BOOL Client::Send(LPVOID pMsg, DWORD dwMsgSize)
{
	ASSERT(dwMsgSize < 0xFFFFFF);

	// ��������Ϣ��ӵ�������Ϣ����
	if( FALSE == m_bConnected )
		return FALSE;

	return m_SendMsgQueue.AddMsg((LPBYTE)pMsg, dwMsgSize);
}


//-----------------------------------------------------------------------------
// ����
//-----------------------------------------------------------------------------
INLINE LPBYTE Client::Recv(DWORD& dwMsgSize)
{
	// �ӽ��ܻ��������Ϣ
	return m_RecvMsgQueue.GetMsg(dwMsgSize);
}


//-----------------------------------------------------------------------------
// �ͷ��Ѿ��յ�����Ϣ
//-----------------------------------------------------------------------------
INLINE VOID Client::FreeRecved(LPBYTE pMsg) 
{
	m_RecvMsgQueue.FreeMsg(pMsg);
}


//-----------------------------------------------------------------------------
// �õ�δ���Ͱ��������������Դ����ж����縺��
//-----------------------------------------------------------------------------
INLINE INT	Client::GetUnsend()
{
	if( !m_bConnected )
		return 0;

	return m_SendMsgQueue.GetMsgNum();
}


//-----------------------------------------------------------------------------
// �õ��ѽ��ܰ��������������Դ����ж����縺��
//-----------------------------------------------------------------------------
INLINE INT	Client::GetUnreceive()
{
	if( !m_bConnected )
		return 0;

	return m_RecvMsgQueue.GetMsgNum();
}

}
}
