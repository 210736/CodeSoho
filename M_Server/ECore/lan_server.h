//-----------------------------------------------------------------------------
// author Lyp
// date 2007-11-27
//-----------------------------------------------------------------------------
#pragma once
namespace ECore {
namespace Lan {
	
struct LogHandler
{  
	virtual DWORD Logon(LPBYTE pByte, DWORD dwSize)=0;
	virtual VOID Logoff(DWORD dwClientID)=0;


	// m_bAutoClearClient �� ����ͻ��˶������Ƿ��Զ�����ͻ�����Ϣ������δ�����������Ϣ
	// �������ΪFALSE. ���пͻ��˶���ʱ���ⲿ�����ֶ�����ClearClient()����
	BOOL m_bAutoClearClient;	
	
	LogHandler() { m_bAutoClearClient = TRUE; }
};


//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------
class ECORE_API Server
{
public:
	BOOL	Init(LogHandler* pLogHandler, INT nPort);
	BOOL	Destroy();

	//-----------------------------------------------------------------------------
	// ��������
	//-----------------------------------------------------------------------------
	INLINE INT		Send(DWORD dwClientID, LPVOID pMsg, DWORD dwSize);
	INLINE LPBYTE	Recv(DWORD dwClientID, DWORD& dwSize, INT& nUnRecved);
	INLINE VOID		FreeRecved(DWORD dwClientID, LPBYTE pMsg) ;

	BOOL	Kick(DWORD dwClientID);
	BOOL	ClearClient(DWORD dwClientID);
	BOOL	GetClientAddress(sockaddr_in* pAddr, DWORD dwClientID);

	//-----------------------------------------------------------------------------
	// ��Ϣͳ��
	//-----------------------------------------------------------------------------
	INLINE INT		GetPort()					{ return (INT)ntohs(m_Address.sin_port); }
	INLINE INT		GetClientNum()				{ return m_mapClient.Size(); }  
	INLINE BOOL		IsClientExist(DWORD dwID)	{ return m_mapClient.IsExist(dwID); }
	INLINE DWORD	GetMsgSend()				{ return m_dwMsgSend; }
	INLINE DWORD	GetMsgRecv()				{ return m_dwMsgRecv; }
	INLINE DWORD	GetBytesSend()				{ return m_dwBytesSend; }
	INLINE DWORD	GetBytesRecv()				{ return m_dwBytesRecv; }


	Server();
	~Server();


private:
	struct Client
	{
		SOCKET			Sock;				// �׽��ֽӿ�
		DWORD			dwID;				// �ͻ���ID
		BOOL VOLATILE 	bClose;				// �Ƿ��Ѿ���ֹ
		DWORD			dwSendID;			// �����߳�ID
		DWORD			dwRecvID;			// �����߳�ID
		MsgQueueTS*		pSendQueue;			// ���Ͷ���
		MsgQueueTS*		pRecvQueue;			// ���ն���
		DWORD			dwConnectTime;		// ����ʱ��
		INT				nRecvNum;			// �հ�����

		Client() { ZeroMemory(this, sizeof(*this)); dwID = INVALID; }
	};

	ThreadMgr*					m_pThreadMgr;			// �̹߳�����
	LogHandler*					m_pLogHandler;			// ����ǳ�����ӿ�

	Mutex						m_DisconnectLock;		// �Ͽ�����ʱ������

	MapTS<DWORD, Client*>		m_mapClient;			// ��¼�û�nCDIndex��ID���
	ListTS<Client*>				m_listDisClient;		// ��Ҫɾ���Ŀͻ��˶���

	DWORD						m_dwAcceptID;			// ���������̵߳��̳߳�ID
	DWORD						m_dwDisconnectID;		// �ر������̵߳��̳߳�ID
	BOOL VOLATILE 				m_bCloseAccept;			// �Ƿ���Ҫ�ر�Accept�߳�
	BOOL VOLATILE 				m_bCloseDisconnect;		// �Ƿ���Ҫ�رն����߳�
	sockaddr_in					m_Address;				// server address
	SOCKET						m_Sock;					// ����Accept��socket

	DWORD VOLATILE 				m_dwMsgSend;			// ����ͳ��
	DWORD VOLATILE 				m_dwMsgRecv;			// �հ�ͳ��
	DWORD VOLATILE 				m_dwBytesSend;			// �����ֽ���ͳ��
	DWORD VOLATILE 				m_dwBytesRecv;			// �հ��ֽ���ͳ��

	DWORD	DoAccept();
	DWORD	DoDisconnect();
	DWORD	DoSend(Client*);
	DWORD	DoRecv(Client*);
	VOID	DisconnetClient(Client* pClient);
	DWORD	CloseAccept();
	DWORD	CloseDisconnect();
	
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
INLINE INT Server::Send(DWORD dwClientID, LPVOID pMsg, DWORD dwSize)
{
	Client* pClient = m_mapClient.Peek(dwClientID);
	if( !VALID(pClient) || pClient->bClose )
		return INVALID;

	pClient->pSendQueue->AddMsg(pMsg, dwSize);
	return pClient->pSendQueue->GetMsgNum();
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
INLINE LPBYTE Server::Recv(DWORD dwClientID, DWORD& dwSize, INT& nUnRecved)
{
	Client* pClient = m_mapClient.Peek(dwClientID);
	if( !VALID(pClient) ) 
		return NULL;

	nUnRecved = pClient->pRecvQueue->GetMsgNum();
	return pClient->pRecvQueue->GetMsg(dwSize);
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
INLINE VOID Server::FreeRecved(DWORD dwClientID, LPBYTE pMsg) 
{
	Client* pClient = m_mapClient.Peek(dwClientID);
	if( !VALID(pClient) ) 
		return;

	pClient->pRecvQueue->FreeMsg(pMsg);
}

}
}