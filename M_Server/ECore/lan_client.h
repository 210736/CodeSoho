//-----------------------------------------------------------------------------
// author Lyp
// date 2000-01-17
// last 2010-01-13
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {
namespace Lan {

// ֧���ı�ģʽ����ģʽ������ͬһ�����¶�̬�л����ʺ�HTTP����
enum EClientMode
{
	ECM_Package = 0,
	ECM_Stream = 1,
	ECM_Text = 2
};

//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------
class ECORE_API Client
{
public:
	//-----------------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------------
	BOOL	Init(EClientMode eMode=ECM_Package);
	VOID	Destroy();

	//-----------------------------------------------------------------------------
	// �����������
	//-----------------------------------------------------------------------------
	VOID	TryConnect(LPCSTR szIP, INT nPort=0);	// ��������ָ����ַ
	BOOL	IsTryingConnect(); // �Ƿ����ڳ�������
	
	INLINE BOOL	IsConnected()			{ return m_bConnected; }
	VOID	Disconnect();


	//------------------------------------------------------------------------------
	// ���ͣ��������FALSE,����û�����ӻ�û���ڴ�ռ���������msg
	//------------------------------------------------------------------------------
	INLINE BOOL	Send(LPVOID pMsg, DWORD dwMsgSize);

	//------------------------------------------------------------------------------
	// ���գ��������NULL��ʾû�����ӻ�δ�յ�����
	//------------------------------------------------------------------------------
	INLINE LPBYTE	Recv(DWORD& dwMsgSize) { return m_RecvMsgQueue.GetMsg(dwMsgSize); }

	//------------------------------------------------------------------------------
	// �ͷŽ��յ��ڴ棬��Ӧ��Recv(),��Ϊ�ڴ����ڲ�����ģ�����Ӧ�ý����ڲ����ͷ�
	//------------------------------------------------------------------------------
	INLINE VOID	FreeRecved(LPBYTE pMsg) { m_RecvMsgQueue.FreeMsg(pMsg); }

	//-----------------------------------------------------------------------------
	// ��Ϣͳ��
	//-----------------------------------------------------------------------------
	INLINE INT		GetUnsend();									// �õ�δ�����İ�����
	INLINE INT		GetUnreceive();									// �õ�δ���ϲ���յİ�����
	INLINE DWORD	GetSend()		{ return m_dwSendPackNum; }		// ��ȡ�ܷ�����(��δ�����Ļ���ʧ�ܵ�)
	INLINE DWORD	GetRecved()		{ return m_dwRecvPackNum; }		// ��ȡ���հ���
	INLINE DWORD	GetSendBPS()	{ return m_dwSendBPS; }			// �õ�ÿ��������
	INLINE DWORD	GetRecvBPS()	{ return m_dwRecvBPS; }			// �õ�ÿ��������

	INLINE LPCSTR	GetServerIP()	{ return m_strIP.c_str(); }		// �õ�������IP

	BOOL	SwitchMode(EClientMode eMode);		// �ı�����ģʽ֮��ʵʱ�л����ʺ�HTTP����
	BOOL	SetEndian(BOOL bBigEndian);			// ��ģʽ��SIZEͷ���ֽ���
	

	INT32  Connect(LPCSTR strIp, INT nport);


	Client();
	~Client();

private:
	//-------------------------------------------------------------------------------
	// �ڲ�����
	//-------------------------------------------------------------------------------
	VOID		Clear();
	BOOL		CloseSocket(SOCKET& socket, INT nRetryTimes);


private:
	//-------------------------------------------------------------------------------
	// �̹߳�����
	//-------------------------------------------------------------------------------
	ThreadMgr*				m_pThreadMgr;

	//-------------------------------------------------------------------------------
	// ������Ϣ���
	//-------------------------------------------------------------------------------
	MsgQueueTS				m_SendMsgQueue;			// ���Ͷ���
	MsgQueueTS				m_RecvMsgQueue;			// ���ն���
	
	//-------------------------------------------------------------------------------
	// ����
	//-------------------------------------------------------------------------------
	std::string				m_strIP;					// ������IP��ַ
	INT						m_nPort;					// �������˿�
	SOCKET					m_Sock;	
	EClientMode				m_eMode;					// ����ģʽ
	BOOL					m_bBigEndian;				// ��ͷ�ֽ���

	//--------------------------------------------------------------------------------
	// ����״̬
	//--------------------------------------------------------------------------------
	BOOL VOLATILE 			m_bConnected;

	//--------------------------------------------------------------------------------
	// �����̵߳��߳�ID
	//--------------------------------------------------------------------------------
	DWORD VOLATILE 			m_dwConnectID;

	//--------------------------------------------------------------------------------
	// �߳����
	//--------------------------------------------------------------------------------
	BOOL VOLATILE 			m_bCloseConnect;			
	BOOL VOLATILE			m_bCloseSend;				
	BOOL VOLATILE 			m_bCloseReceive;			

	//--------------------------------------------------------------------------------
	// ͳ��
	//--------------------------------------------------------------------------------
	DWORD VOLATILE 			m_dwLastStatInTime;			// Inͳ�ƿ�ʼʱ��
	DWORD VOLATILE 			m_dwLastStatOutTime;		// Outͳ�ƿ�ʼʱ��
	DWORD VOLATILE	 		m_dwSendBPS;
	DWORD VOLATILE			m_dwRecvBPS;
	DWORD VOLATILE			m_dwByteOutCounter;
	DWORD VOLATILE 			m_dwByteInCounter;
	DWORD VOLATILE			m_dwSendPackNum;
	DWORD VOLATILE 			m_dwRecvPackNum;

	//------------------------------------------------------------------------------
	// �ڲ��߳�
	//------------------------------------------------------------------------------
	DWORD Connect();		// �����̺߳���
	DWORD Send();			// �����̺߳���
	DWORD Receive();		// �����̺߳���
	DWORD StreamReceive();	// �������̺߳���

	//-------------------------------------------------------------------------------
	// �̹߳رշ���
	//-------------------------------------------------------------------------------
	INLINE DWORD CloseConnect() { InterlockedExchangeEx((LPLONG)&m_bCloseConnect,	TRUE); return 0; }
	INLINE DWORD CloseSend()	 { InterlockedExchangeEx((LPLONG)&m_bCloseSend,		TRUE); return 0; }
	INLINE DWORD CloseReceive() { InterlockedExchangeEx((LPLONG)&m_bCloseReceive,	TRUE); return 0; }

};

//-----------------------------------------------------------------------------
// ����
//-----------------------------------------------------------------------------
INLINE BOOL Client::Send(LPVOID pMsg, DWORD dwMsgSize)
{
	ASSERT(pMsg);
	ASSERT(dwMsgSize > 0 && dwMsgSize < 0xFFFFFF);	// ���ݹ��ֳ�

	// ��������Ϣ��ӵ�������Ϣ����
	if( FALSE == m_bConnected )
		return FALSE;

	// ���ӷ�����Ϣ����
	++m_dwSendPackNum;
	return m_SendMsgQueue.AddMsg((LPBYTE)pMsg, dwMsgSize);
}


//-----------------------------------------------------------------------------
// �õ�δ���Ͱ��������������Դ����ж����縺��
//-----------------------------------------------------------------------------
INLINE INT Client::GetUnsend()
{
	if( !m_bConnected )
		return 0;

	return m_SendMsgQueue.GetMsgNum();
}


//-----------------------------------------------------------------------------
// �õ��ѽ��ܰ��������������Դ����ж����縺��
//-----------------------------------------------------------------------------
INLINE INT Client::GetUnreceive()
{
	if( !m_bConnected )
		return 0;

	return m_RecvMsgQueue.GetMsgNum();
}


}
}
